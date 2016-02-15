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

#include <stdio.h> //support for printf
#include <string.h> // support for memset

#include "clients.h"
#include "maps.h"
#include "server_protocol_functions.h"
#include "characters.h"
#include "logging.h"
#include "packet.h"

void broadcast_actor_equip_item(int actor_node, int equipable_item_type, int equipable_item_id){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int char_tile=clients.client[actor_node].map_tile;

    //pre-create the add_new_enhanced_actor packet so we don't have to repeat this on each occasion when
    //it needs to sent to other actors
    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length=0;

    actor_wear_item_packet(actor_node, packet, &packet_length, equipable_item_type, equipable_item_id);

    //cycle through all the actors
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to used nodes
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

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


void broadcast_actor_unequip_item(int actor_node, int equipable_item_type, int equipable_item_id){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int char_tile=clients.client[actor_node].map_tile;

    //pre-create the add_new_enhanced_actor packet so we don't have to repeat this on each occasion when
    //it needs to sent to other actors
    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length=0;

    actor_unwear_item_packet(actor_node, packet, &packet_length, equipable_item_type, equipable_item_id);

    //cycle through all the actors
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to used nodes
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

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

