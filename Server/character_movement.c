/******************************************************************************************************************
    Copyright 2014, 2015 UnoffLandz

    This file is part of unoff_server_4.

    unoff_server_4 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    unoff_server_4 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/

#include <stdio.h>      //support sprintf function
#include <string.h>    //support strcmp function

#include "logging.h"
#include "clients.h"
#include "maps.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "broadcast_movement.h"
#include "global.h"
#include "colour.h"
#include "server_messaging.h"
#include "characters.h"
#include "movement.h"
#include "character_movement.h"
#include "db/db_character_tbl.h"
#include "pathfinding.h"
#include "server_start_stop.h"
#include "db/database_functions.h"
#include "game_data.h"
#include "idle_buffer2.h"
#include "harvesting.h"
#include "bags.h"
#include "packet.h"

#define DEBUG_MOVEMENT 0

int get_move_command(int tile_pos, int tile_dest, int map_axis){

    /** RESULT  : calculates the move command based on the current and destination tile

        RETURNS : move command

        PURPOSE :

        NOTES   :
    */

    int i=0;
    int move=tile_dest-tile_pos;

    if(move==map_axis) i=0;

    else if(move==map_axis+1) i=1;

    else if(move==map_axis-1) i=7;

    else if(move==(map_axis*-1)) i=4;

    else if(move==(map_axis*-1)+1) i=3;

    else if(move==(map_axis*-1)-1) i=5;

    else if(move==1) i=2;

    else if(move==-1) i=6;

    else {
        log_event(EVENT_ERROR, "illegal move in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "current tile [%i] destination tile [%i] move distance [%i]", tile_pos, tile_dest, move);
        stop_server();
    }

    return vector[i].move_cmd;
}


void process_char_move(int actor_node, time_t current_utime){

    /** public function - see header **/

    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int current_tile=clients.client[actor_node].map_tile;
    int next_tile=0;
    int move_cmd=0;

    // move actor one step along the path
    if(clients.client[actor_node].path_count>0){

        //adjust timer to compensate for wrap-around>
        if(clients.client[actor_node].time_of_last_move>current_utime) current_utime+=1000000;

        // check for time of next movement
        if(current_utime>clients.client[actor_node].time_of_last_move+290000) {// TODO (themuntdregger#1#): replace hard coded movement time value

            //get destination tile from the path queue
            next_tile=clients.client[actor_node].path[clients.client[actor_node].path_count-1];

            clients.client[actor_node].path_count--;

            // filter out moves where position and destination are the same
            if(current_tile!=next_tile){

                //update the time of move
                gettimeofday(&time_check, NULL);
                clients.client[actor_node].time_of_last_move=time_check.tv_usec;

                //calculate the move_cmd and broadcast to clients
                move_cmd=get_move_command(current_tile, next_tile, map_axis);

                broadcast_actor_packet(actor_node, (unsigned char)move_cmd, next_tile);

                // if #TRACK ON has been used display char position on console
                if(clients.client[actor_node].debug_status==DEBUG_TRACK){

                    send_text(clients.client[actor_node].socket, CHAT_SERVER, "%cPosition x=%i y=%i", c_green3+127, next_tile % map_axis, next_tile / map_axis);
                }

                //update char current position and save
                clients.client[actor_node].map_tile=next_tile;

                //update_database
                push_sql_command("UPDATE CHARACTER_TABLE SET MAP_TILE=%i, MAP_ID=%i WHERE CHAR_ID=%i",next_tile, map_id, clients.client[actor_node].character_id);
            }
        }
    }
}


void remove_char_from_map(int actor_node){

    /** public function - see header **/

    int map_id=clients.client[actor_node].map_id;

    //check bounds
    if(map_id>MAX_MAPS || map_id<1) return;

    //broadcast actor removal to other chars on map
    broadcast_remove_actor_packet(actor_node);
    log_event(EVENT_SESSION, "char %s removed from map %s", clients.client[actor_node].char_name, maps.map[map_id].map_name);
}


void send_actors_to_client(int actor_node){

    /** RESULT  : make this actor and other actors visible to this client

        RETURNS : void

        PURPOSE : used by add_char_to_map function
    **/

    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length;

    int map_id=clients.client[actor_node].map_id;
    int map_tile=clients.client[actor_node].map_tile;
    int char_visual_range=get_char_visual_range(actor_node);
    int socket=clients.client[actor_node].socket;

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors on the same map
        if(map_id==clients.client[i].map_id){

            //restrict to actors within visual range of this character
            if(get_proximity(map_tile, clients.client[i].map_tile, map_id)<=char_visual_range){

                //send actors within visual proximity to this char
                add_new_enhanced_actor_packet(i, packet, &packet_length);
                send_packet(socket, packet, packet_length);
            }
        }
    }
}


bool add_char_to_map(int actor_node, int map_id, int map_tile){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    //if map is outside bounds then abort map move and alert client
    if(map_id>MAX_MAPS || map_id<0) {

        log_event(EVENT_ERROR, "illegal map id[%i] name %s])in function %s: module %s: line %i", map_id, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);

        send_text(socket, CHAT_SERVER, "%cmap %i is outside bounds", c_red1+127, map_id);
        return false;
    }

    //check that map exists
    if(strcmp(maps.map[map_id].elm_filename, "")==0){

        log_event(EVENT_ERROR, "map id[%i] doesn't exist in function %s: module %s: line %i", map_id, __func__, __FILE__, __LINE__);

        send_text(socket, CHAT_SERVER, "%cmap %i doesn't exist", c_red3+127, map_id);
        return false;
    }

    //if char is moving then cancel rest of path
    clients.client[actor_node].path_count=0;

    //ensure map tile is walkable
    if(tile_walkable(map_id, map_tile)==false){

        log_event(EVENT_ERROR, "tile [%i] of map [%i] is not walkable in function %s: module %s: line %i", map_tile, map_id, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);

        send_text(socket, CHAT_SERVER, "%ctile %i of map %s is not walkable", c_red3+127, map_tile, maps.map[map_id].map_name);
        return false;
    }

    clients.client[actor_node].map_id=map_id;
    clients.client[actor_node].map_tile=map_tile;

    //send change map to client
    send_change_map(socket, maps.map[map_id].elm_filename);

    //send other chars to client
    send_actors_to_client(actor_node);

    //send existing map bags to client
/*
    bag[0].bag_in_use=true;
    gettimeofday(&time_check, NULL);
    bag[0].bag_refreshed=time_check.tv_sec;
    bag[0].tile=27225;
    bag[0].map_id=1;
    bag[0].bag_type_id=0;
*/

    send_get_bags_list(socket); // TODO (themuntdregger#1#): check send_get_bags function works then remove remmed out code

/*
    for(int i=0; i<MAX_BAGS; i++){

        //restrict to bags on the same map
        if(map_id==bag[i].map_id){

            //display bag to client
            unsigned char packet[MAX_PACKET_SIZE]={0};
            size_t packet_length;
            get_new_bag_packet(actor_node, i, packet, &packet_length);

            send_packet(socket, packet, packet_length);
        }
    }
*/

    //show this char to other connected clients on this map
    broadcast_add_new_enhanced_actor_packet(actor_node);

    //update_database
    push_sql_command("UPDATE CHARACTER_TABLE SET MAP_TILE=%i, MAP_ID=%i WHERE CHAR_ID=%i", clients.client[actor_node].map_tile, clients.client[actor_node].map_id, clients.client[actor_node].character_id);

    log_event(EVENT_SESSION, "char [%s] added to map [%s] at tile [%i]", clients.client[actor_node].char_name, maps.map[map_id].map_name, clients.client[actor_node].map_tile);

    return true;
}


bool move_char_between_maps(int actor_node, int map_id, int map_tile){

    /** public function - see header */

    remove_char_from_map(actor_node);

    int old_map_id=clients.client[actor_node].map_id;
    int old_map_tile=clients.client[actor_node].map_tile;

    if(add_char_to_map(actor_node, map_id, map_tile)==false){

        //return char to original map if jump to new map fails
        if(add_char_to_map(actor_node, old_map_id, old_map_tile)==false){

            log_event(EVENT_ERROR, "cannot return char to original map in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        return false;

    }

    return true;
}


void start_char_move(int actor_node, int destination){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int current_tile=clients.client[actor_node].map_tile;
    int socket=clients.client[actor_node].socket;

    //if char is sitting then stand before moving
    if(clients.client[actor_node].frame==frame_sit){

        clients.client[actor_node].frame=frame_stand;
        broadcast_actor_packet(actor_node, actor_cmd_stand_up, clients.client[actor_node].map_tile);

        push_sql_command("UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i", clients.client[actor_node].frame, clients.client[actor_node].character_id);
    }

    //check if the destination is walkable
    if(maps.map[map_id].height_map[destination]==NON_TRAVERSABLE_TILE){

        send_text(socket, CHAT_SERVER, "%cThe tile you clicked on can't be walked on", c_red3+127);
        return;
    }

    //check for zero length path
    if(current_tile==destination) return;

    //get path
    if(get_astar_path(actor_node, current_tile, destination)==false) return;

    //if standing on a bag, close the bag grid
    if(clients.client[actor_node].bag_open==true){

        clients.client[actor_node].bag_open=false;
        send_close_bag(socket);
    }

    //reset time of last move to zero so the movement is processed without delay
    clients.client[actor_node].time_of_last_move=0;
}


void stop_char_move(int actor_node){

    /** public function - see header */

    clients.client[actor_node].path_count=0;
}
