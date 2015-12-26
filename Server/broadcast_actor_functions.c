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

#include <stdio.h> //support for printf
#include <string.h> // support for memset

#include "clients.h"
#include "maps.h"
#include "server_protocol_functions.h"
#include "character_race.h"
#include "characters.h"
#include "server_messaging.h"
#include "colour.h"
#include "chat.h"
#include "global.h"
#include "logging.h"
#include "bags.h"
#include "packet.h"

#define DEBUG_BROADCAST_ACTOR_FUNCTIONS 0


void broadcast_add_new_enhanced_actor_packet(int actor_node){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;
    int char_tile=clients.client[actor_node].map_tile;

    //pre-create the add_new_enhanced_actor packet so we don't have to repeat this on each occasion when
    //it needs to sent to other actors
    unsigned char packet[1024]={0};
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
    unsigned char packet[1024]={0};
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

    unsigned char packet[1024];// sending char packets
    size_t packet_length=0;

    unsigned char packet1[1024];// receiving char add_actor packet
    size_t packet1_length=0;

    unsigned char packet2[1024];// receiving char add_enhanced_actor packet
    size_t packet2_length=0;

    unsigned char packet3[1024];// receiving char remove_actor packet
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
                else if(proximity_before_move<=sender_visual_range && proximity_after_move<=sender_visual_range){

                    if(i==actor_node) {

                        // sending char sees itself move
                        add_actor_packet(clients.client[i].socket, move, packet, &packet_length);
                    }
                    else{

                        // sending char sees itself stationery
                        add_actor_packet(clients.client[i].socket, 0, packet, &packet_length);
                    }

                    send_packet(socket, packet, packet_length);
                }
            }
        }
    }
}


void broadcast_local_chat(int actor_node, char *text_in){

    /** public function - see header */

    int map_id=clients.client[actor_node].map_id;

    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            if(map_id==clients.client[i].map_id) {

                int socket=clients.client[i].socket;

                //broadcast to receiving chars
                if(i!=actor_node){

                    if(get_proximity(clients.client[actor_node].map_tile, clients.client[i].map_tile, map_id)<LOCAL_CHAT_RANGE){

                        send_text(socket, CHAT_LOCAL, text_in);
                    }
                }

                //broadcast to sending char
                else {

                    send_text(socket, CHAT_LOCAL,"%c%s: %s", c_grey1+127, clients.client[i].char_name, text_in);
                }
            }
        }
    }
}


void broadcast_guild_chat(int guild_id, int actor_node, char *text_in){

    /** public function - see header */

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //filter players who are in this guild
            if(clients.client[i].guild_id==guild_id){

                int socket=clients.client[i].socket;

                send_text(socket, CHAT_GM, "%c[%s]: %s", c_blue1+127, clients.client[actor_node].char_name, text_in);
            }
        }
    }
}


void broadcast_server_message(char *text_in){

    /** public function - see header */

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            send_text(i, CHAT_SERVER, "%cSERVER MESSAGE: %s", c_red1+127, text_in);
        }
    }
}


void broadcast_channel_chat(int chan, int actor_node, char *text_in){

    /** public function - see header */

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            int socket=clients.client[i].socket;

            //filter out players who are not in this chan
            if(player_in_chan(i,chan)!=-1){

                int active_chan_slot=clients.client[i].active_chan;
                int active_chan=clients.client[i].chan[active_chan_slot-1];

                //show non-active chan in darker grey
                int text_colour = (active_chan==chan) ? c_grey1+127 : c_grey2+127;

                if(i!=actor_node){

                    //broadcast to receiving chars
                    send_text(socket, CHAT_CHANNEL_0, "%c[%s]: %s", text_colour, clients.client[actor_node].char_name, text_in);
                }
                else {

                    //broadcast to sending char
                    send_text(socket, CHAT_CHANNEL_0 + active_chan_slot,
                    "%c[%s @ %i]: %s", c_grey1+127,
                    clients.client[actor_node].char_name,
                    CHAT_CHANNEL_0 + active_chan_slot, text_in);
                }
            }
        }
    }
}


void broadcast_channel_event(int chan, int actor_node, char *text_in){

    /** public function - see header */

    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //filter out self and players who are not in this chan
            if(actor_node!=i || player_in_chan(i,chan)!=-1){

                send_text(i, CHAT_CHANNEL_0 + clients.client[i].active_chan, text_in);
            }
        }
    }
}


void broadcast_get_new_bag_packet(int actor_node, int bag_id) {

    /** public function - see header */

    //pre-create the packet so we don't have to repeat this on each occasion when it needs to
    //sent to other actors
    unsigned char packet[1024]={0};
    size_t packet_length=0;
    int socket=clients.client[actor_node].socket;

    get_new_bag_packet(socket, bag_id, packet, &packet_length);

    //cycle through all the clients
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //restrict to chars on the same map as broadcasting char
            if(bag[bag_id].map_id==clients.client[i].map_id){

                send_packet(clients.client[i].socket, packet, packet_length);
            }
        }
    }
}


void broadcast_destroy_bag_packet(int bag_id) {

    /** public function - see header */

    //cycle through all the clients
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //restrict to chars on the same map as bag
            if(bag[bag_id].map_id==clients.client[i].map_id){

                //if client has bag grid open then close it
                if(clients.client[i].open_bag_id==bag_id){

                    clients.client[i].open_bag_id=0;
                    clients.client[i].bag_open=false;

                    send_close_bag(clients.client[i].socket);
                }

                send_destroy_bag(clients.client[i].socket, bag_id);
            }
        }
    }

    //clear bag contents
    memset(&bag[bag_id], 0, sizeof(bag[bag_id]));
}
