/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

#include <stdlib.h>     //supports exit function
#include <stdio.h>      //support sprintf function
#include <sys/socket.h> //support for send function

#include "logging.h"
#include "clients.h"
#include "maps.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "broadcast_actor_functions.h"
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
#include "idle_buffer.h"

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
        log_event(EVENT_MOVE_ERROR, "illegal move in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_MOVE_ERROR, "current tile [%i] destination tile [%i] move distance [%i]", tile_pos, tile_dest, move);
        stop_server();
    }

    return vector[i].move_cmd;
}


void process_char_move(int connection, time_t current_utime){

    /** public function - see header **/

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int current_tile=clients.client[connection].map_tile;
    int next_tile=0;
    int move_cmd=0;

    // move actor one step along the path
    if(clients.client[connection].path_count>0){

        //adjust timer to compensate for wrap-around>
        if(clients.client[connection].time_of_last_move>current_utime) current_utime+=1000000;

        // check for time of next movement
        if(current_utime>clients.client[connection].time_of_last_move+290000) {

            //get destination tile from the path queue
            next_tile=clients.client[connection].path[clients.client[connection].path_count-1];

            clients.client[connection].path_count--;

            // filter out moves where position and destination are the same
            if(current_tile!=next_tile){

                #if DEBUG_MOVEMENT==1
                printf("move char [%s] from tile [%i] to tile [%i]\n", clients.client[connection].char_name, current_tile, next_tile);
                #endif

                //update the time of move
                gettimeofday(&time_check, NULL);
                clients.client[connection].time_of_last_move=time_check.tv_usec;

                //calculate the move_cmd and broadcast to clients
                move_cmd=get_move_command(current_tile, next_tile, map_axis);
                broadcast_actor_packet(connection, move_cmd, next_tile);

                //update char current position and save
                clients.client[connection].map_tile=next_tile;

                //update_db_char_position(connection);
                char sql[MAX_SQL_LEN]="";
                snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET MAP_TILE=%i, MAP_ID=%i WHERE CHAR_ID=%i",next_tile, map_id, clients.client[connection].character_id);
                db_push_buffer(sql, 0, IDLE_BUFFER_PROCESS_SQL, NULL);

            }
        }
    }
}


int remove_char_from_map(int connection){

    /** public function - see header **/

    int map_id=clients.client[connection].map_id;

    //check for illegal map
    if(map_id>MAX_MAPS || map_id<1){

        log_event(EVENT_MOVE_ERROR, "attempt to remove char from illegal map (id[%i] map name [%s]) in function %s: module %s: line %i", map_id, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        return REMOVE_MAP_ILLEGAL;
    }

    //broadcast actor removal to other chars on map
    broadcast_remove_actor_packet(connection);
    log_event(EVENT_SESSION, "char %s removed from map %s", clients.client[connection].char_name, maps.map[map_id].map_name);

    return REMOVE_MAP_SUCESS;
}


void send_actors_to_client(int connection){

    /** RESULT  : make this actor and other actors visible to this client

        RETURNS : void

        PURPOSE : used by add_char_to_map function
    **/

    int i;
    unsigned char packet[1024];
    int packet_length;

    int map_id=clients.client[connection].map_id;
    int map_tile=clients.client[connection].map_tile;
    int char_visual_range=get_char_visual_range(connection);

    for(i=0; i<MAX_CLIENTS; i++){

        //restrict to characters on the same map
        if(map_id==clients.client[i].map_id){

            //restrict to characters within visual range of this character
            if(get_proximity(map_tile, clients.client[i].map_tile, maps.map[map_id].map_axis)<=char_visual_range){

                //send actors within visual proximity to this char
                add_new_enhanced_actor_packet(i, packet, &packet_length);
                send(connection, packet, packet_length, 0);
            }
        }
    }
}


int add_char_to_map(int connection, int map_id, int map_tile){

    /** public function - see header **/

    //check for illegal maps
    if(map_id>MAX_MAPS || map_id<0) {

        log_event(EVENT_MOVE_ERROR, "illegal map (id[%i] name [%s]) in function %s: module %s: line %i", map_id, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        return ADD_MAP_ILLEGAL;
    }

    //get nearest unoccupied tile to the clients position
    int unoccupied_tile=get_nearest_unoccupied_tile(map_id, map_tile);

    if(unoccupied_tile==0){

        log_event(EVENT_MOVE_ERROR, "Unable to find unoccupied tile within [%i] tiles on map (id[%i] name [%s]) in function %s: module %s: line %i", MAX_UNOCCUPIED_TILE_SEARCH, map_id, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        return ADD_MAP_UNREACHABLE;
    }

    clients.client[connection].map_tile=unoccupied_tile;

    //send map to client
    send_change_map(connection, maps.map[map_id].elm_filename);

    //make scene visible to this client
    send_actors_to_client(connection);

    //send existing bags on map to client
    //send_bags_to_client(connection);

    //add this char to other connected clients on this map
    broadcast_add_new_enhanced_actor_packet(connection);

    log_event(EVENT_SESSION, "char [%s] added to map [%s] at tile [%i]", clients.client[connection].char_name, maps.map[map_id].map_name, clients.client[connection].map_tile);

    return ADD_MAP_SUCESS;
}


void move_char_between_maps(int connection, int new_map_id, int new_map_tile){

    /** public function - see header */

    //check to see if old map is legal and, if not, transport char to Isla Prima
    if(remove_char_from_map(connection)==ILLEGAL_MAP) {

        new_map_id=game_data.beam_map_id;
        new_map_tile=game_data.beam_map_tile;
    }

    //check to see if new map is legal and, if not, transport char tp Isla Prima
    if(add_char_to_map(connection, new_map_id, new_map_tile)!=ADD_MAP_SUCESS){

        new_map_id=game_data.beam_map_id;
        new_map_tile=game_data.beam_map_tile;
    }

    //save char map id and position
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET MAP_TILE=%i, MAP_ID=%i WHERE CHAR_ID=%i", new_map_tile, new_map_id, clients.client[connection].character_id);
    db_push_buffer(sql, 0, IDLE_BUFFER_PROCESS_SQL, NULL);
}


void start_char_move(int connection, int destination){

    /** public function - see header */

    char text_out[1024]="";

    int map_id=clients.client[connection].map_id;
    int current_tile=clients.client[connection].map_tile;

/*
    //if char is harvesting then stop
    if(clients.client[connection].harvest_flag==TRUE){

        stop_harvesting2(connection, loop);
        return;
    }
*/

    //if char is sitting then stand before moving
    if(clients.client[connection].frame==frame_sit){

        clients.client[connection].frame=frame_stand;
        broadcast_actor_packet(connection, actor_cmd_stand_up, clients.client[connection].map_tile);

        char sql[MAX_SQL_LEN]="";
        snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i", clients.client[connection].frame, clients.client[connection].character_id);
        db_push_buffer(sql, 0, IDLE_BUFFER_PROCESS_SQL, NULL);
    }

    //check if the destination is walkable
    if(maps.map[map_id].height_map[destination]<MIN_TRAVERSABLE_VALUE){

        sprintf(text_out, "%cThe tile you clicked on can't be walked on", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        return;
    }

    //check for zero length path
    if(current_tile==destination){

        #if DEBUG_MOVEMENT==1
        printf("current tile = destination (ignored)\n");
        #endif

        return;
    }

    //get path
    if(get_astar_path(connection, current_tile, destination)==NOT_FOUND){

        log_event(EVENT_ERROR, "path not found in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

/*
    //if standing on a bag, close the bag grid
    if(clients.client[connection].bag_open==TRUE){

        clients.client[connection].bag_open=FALSE;
        send_s_close_bag(connection);
    }
*/

    #if DEBUG_MOVEMENT==1
    printf("character [%s] got a new path...\n", clients.client[connection].char_name);

    int i=0;
    for(i=0; i<clients.client[connection].path_count; i++){
        printf("%i %i\n", i, clients.client[connection].path[i]);
    }
    #endif

    //reset time of last move to zero so the movement is processed without delay
    clients.client[connection].time_of_last_move=0;
}

