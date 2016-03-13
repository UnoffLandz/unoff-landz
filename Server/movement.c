/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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

#include <string.h> //support for memcpy function

#include "movement.h"
#include "maps.h"
#include "logging.h"
#include "characters.h"
#include "server_protocol_functions.h"

struct vector_type vector[] = {

//   Vector  Enum
//  -------  ----------------
    {0, 1,   actor_cmd_move_n},
    {1, 1,   actor_cmd_move_ne},
    {1, 0,   actor_cmd_move_e},
    {1, -1,  actor_cmd_move_se},
    {0, -1,  actor_cmd_move_s},
    {-1, -1, actor_cmd_move_sw},
    {-1, 0,  actor_cmd_move_w},
    {-1, 1,  actor_cmd_move_nw}
};


bool tile_walkable(int map_id, int tile){

    /** RESULT  : determines if a tile is walkable

        RETURNS : true/false

        PURPOSE : used by function: get_nearest_unoccupied_tile

        NOTES   :
    */

    //check the height map and see if tile is walkable
    if(maps.map[map_id].height_map[tile]==NON_TRAVERSABLE_TILE) return false;

    return true;
}


bool tile_unoccupied(int map_id, int map_tile){

    /** RESULT  : determines if tile is unoccupied

        RETURNS : true/false

        PURPOSE : used by get_nearest_unoccupied_tile

        NOTES   :
    */

    //now check through clients and see if any have characters occupying that tile
    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].map_tile==map_tile && clients.client[i].map_id==map_id) return false;
    }

    return true;
}


bool tile_in_lateral_bounds(int tile, int next_tile, int map_id){

    /**  RESULT  : indicates if moving from one tile to the next will cross lateral bounds

        RETURNS : TRUE / FALSE

        PURPOSE : to prevent path crossing lateral bounds

        USAGE   : pathfinding.c get_astar_path / add_ajacent_tiles
    */

    int map_axis=maps.map[map_id].map_axis;
    int tile_x=tile % map_axis;
    int next_tile_x=next_tile % map_axis;
    int vector_x=next_tile_x-tile_x;

    if(tile_x==0 && vector_x==1) return false;
    if(tile_x==1 && vector_x==-1) return false;

    return true;
}


int get_nearest_unoccupied_tile(int map_id, int map_tile){

    /** public function - see header */

    int map_axis=maps.map[map_id].map_axis;

    unsigned char height_map[HEIGHT_MAP_MAX]={0};
    memcpy(height_map, maps.map[map_id].height_map, (size_t)maps.map[map_id].height_map_size);

    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].map_id==map_id){

            height_map[clients.client[i].map_tile]=NON_TRAVERSABLE_TILE;
        }
    }

    //check target tile to see if it is walkable and unoccupied
    if(height_map[map_tile]!=NON_TRAVERSABLE_TILE) return map_tile;

    //if target tile is not walkable and unoccupied then search adjacent tiles
    int start=0, finish=0, inc=0, last_tile=0;

    for(int i=0; i<MAX_UNOCCUPIED_TILE_SEARCH; i++){

        for(int j=0; j<4; j++){

            switch(j){

                case 0:{
                    start=map_tile+(map_axis*i)-i;
                    finish=map_tile+(map_axis*i)+i;
                    inc=1;
                    break;
                }

                case 1:{
                    start=finish=map_tile+(map_axis*i)+i;
                    finish=map_tile-(map_axis*i)+i;
                    inc=-map_axis;
                    break;
                }

                case 2:{
                    start=map_tile-(map_axis*i)+i;
                    finish=map_tile-(map_axis*i)-i;
                    inc=-1;
                    break;
                }

                case 3:{
                    start=map_tile-(map_axis*i)-i;
                    finish=map_tile+(map_axis*i)-i;
                    inc=map_axis;
                }
            }

            for(int j=start; j<=finish; j+=inc){

                //make sure we don't cross horizontal bounds
                if(tile_in_lateral_bounds(last_tile, j, map_id)==false) break;

                //make sure we don't cross vertical bounds
                if(j>(int)maps.map[map_id].height_map_size || j<0) break;

                //check that tile is unoccupied and walkable
                if(height_map[j]!=NON_TRAVERSABLE_TILE) return j;

                last_tile=j;
            }
        }
    }

    return -1;
}



void broadcast_add_new_enhanced_actor_packet(int actor_node){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int char_tile=clients.client[actor_node].map_tile;

    //pre-create the add_new_enhanced_actor packet so we don't have to repeat this on each occasion when
    //it needs to sent to other actors
    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length=0;

    add_new_enhanced_actor_packet(actor_node, packet, &packet_length);

    //cycle through all the actors
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to used nodes
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //exclude the broadcasting char
            if(i!=actor_node){

                //restrict to chars on the same map as broadcasting char
                if(map_id==clients.client[i].map_id){

                    //select this char and those characters in visual range of this char
                    int receiver_char_visual_range=get_char_visual_range(i);
                    int receiver_char_tile=clients.client[i].map_tile;

                    //restrict to those chars that can see the broadcasting char
                    if(get_proximity(char_tile, receiver_char_tile, map_id) < receiver_char_visual_range){

                        send_packet(clients.client[i].socket, packet, packet_length);
                    }
                }
            }
        }
    }
}


void broadcast_remove_actor_packet(int actor_node){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int char_tile=clients.client[actor_node].map_tile;

    //pre-create the remove char packet so we don't have to repeat this on each occasion when it needs to
    //sent to other actors
    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length=0;
    remove_actor_packet(actor_node, packet, &packet_length);

    //cycle through all the actors
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to clients that are logged in
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //exclude the broadcasting char
            if(i!=actor_node){

                //restrict to chars on the same map as broadcasting char
                if(map_id==clients.client[i].map_id){

                    int receiver_char_visual_range=get_char_visual_range(i);
                    int receiver_char_tile=clients.client[i].map_tile;

                    //restrict to those chars that can see the broadcasting char
                    if(get_proximity(char_tile, receiver_char_tile, map_id) < receiver_char_visual_range){

                        send_packet(clients.client[i].socket, packet, packet_length);
                    }
                }
            }
        }
    }
}


void broadcast_actor_packet(int actor_node, unsigned char move, int sender_destination_tile){

    /** public function - see header */

    unsigned char packet[MAX_PACKET_SIZE];// sending char packets
    size_t packet_length=0;

    unsigned char packet1[MAX_PACKET_SIZE];// receiving char add_actor packet
    size_t packet1_length=0;

    unsigned char packet2[MAX_PACKET_SIZE];// receiving char add_enhanced_actor packet
    size_t packet2_length=0;

    unsigned char packet3[MAX_PACKET_SIZE];// receiving char remove_actor packet
    size_t packet3_length=0;

    int sender_current_tile=clients.client[actor_node].map_tile;
    int sender_visual_range=get_char_visual_range(actor_node);

    int map_id=clients.client[actor_node].map_id;

    int proximity_before_move=0;
    int proximity_after_move=0;

    int socket=clients.client[actor_node].socket;

    // pre-create packets that will be sent more than once in order to save time
    add_actor_packet(socket, move, packet1, &packet1_length);
    add_new_enhanced_actor_packet(actor_node, packet2, &packet2_length);
    remove_actor_packet(actor_node, packet3, &packet3_length);

    // broadcast sender char move to all clients
    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_USED){

            if(clients.client[i].map_id==clients.client[actor_node].map_id){

                int receiver_tile=clients.client[i].map_tile;
                int receiver_visual_range=get_char_visual_range(i);

                proximity_before_move=get_proximity(sender_current_tile, receiver_tile, map_id);
                proximity_after_move=get_proximity(sender_destination_tile, receiver_tile, map_id);

                //This block deals with receiving char vision (exclude npc's)
                if(i!=actor_node && clients.client[i].player_type==PLAYER){

                    //sending char moves into visual proximity of receiving char (send enhanced actor packet)
                    if(proximity_before_move > receiver_visual_range && proximity_after_move <= receiver_visual_range){

                        send_packet(clients.client[i].socket, packet2, packet2_length);
                    }

                    //sending char moves out of visual proximity of receiving char (send remove actor packet)
                    else if(proximity_before_move<=receiver_visual_range && proximity_after_move>receiver_visual_range){

                        send_packet(clients.client[i].socket, packet3, packet3_length);
                    }

                    //sending char moving within visual proximity of receiving char (send add actor packet)
                     else if(proximity_before_move<=receiver_visual_range && proximity_after_move<=receiver_visual_range){

                        send_packet(clients.client[i].socket, packet1, packet1_length);
                    }

                    //sending char moving outside visual proximity of receiving char (do nothing)
                    else {

                        //do nothing - block is for debug purposes only
                    }
                }

                //sending char moves into visual proximity of receiving char
                if(proximity_before_move>sender_visual_range && proximity_after_move <= sender_visual_range){

                    add_new_enhanced_actor_packet(i, packet, &packet_length);
                    send_packet(socket, packet, packet_length);
                }

                //sending char moves out of visual proximity of receiving char
                else if(proximity_before_move<=sender_visual_range && proximity_after_move>sender_visual_range){

                    remove_actor_packet(i, packet, &packet_length);
                    send_packet(socket, packet, packet_length);
                }

                //sending char moves within visual proximity of receiving char
                else if(proximity_before_move<=sender_visual_range
                && proximity_after_move<=sender_visual_range
                && clients.client[i].player_type==PLAYER){

                    if(i==actor_node) {

                        // sending char sees itself move
                        add_actor_packet(clients.client[i].socket, move, packet, &packet_length);
                    }
                    else {
                        // sending char sees itself stationery
                        add_actor_packet(clients.client[i].socket, actor_cmd_nothing, packet, &packet_length);
                    }

                    send_packet(socket, packet, packet_length);
                }
            }
        }
    }
}


