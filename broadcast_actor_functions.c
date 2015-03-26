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

#include <stdio.h> //support for printf
//#include <sys/socket.h> //support for send function

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

#define DEBUG_BROADCAST 0

void broadcast_add_new_enhanced_actor_packet(int connection){

    /** public function - see header */

    int i=0;

    int map_id=clients.client[connection].map_id;
    int char_tile=clients.client[connection].map_tile;
    int map_axis=maps.map[map_id].map_axis;

    //pre-create the add_new_enhanced_actor packet so we don't have to repeat this on each occasion when
    //it needs to sent to other actors
    unsigned char packet[1024];
    int packet_length=0;
    add_new_enhanced_actor_packet(connection, packet, &packet_length);

    //cycle through all the clients
    for(i=0; i<MAX_CLIENTS; i++){

        //restrict to clients that are logged in
        if(clients.client[i].client_status==LOGGED_IN){

            //exclude the broadcasting char
            if(i!=connection){

                //restrict to chars on the same map as broadcasting char
                if(map_id==clients.client[i].map_id){

                    //select this char and those characters in visual range of this char
                    int receiver_char_visual_range=get_char_visual_range(i);
                    int receiver_char_tile=clients.client[i].map_tile;

                    //restrict to those chars that can see the broadcasting char
                    if(get_proximity(char_tile, receiver_char_tile, map_axis) < receiver_char_visual_range){

                        //send(i, packet, packet_length, 0);
                        send_packet(i, packet, packet_length);
                    }
                }
            }
        }
    }
}

void broadcast_remove_actor_packet(int connection) {

    /** public function - see header */

    int i=0;

    int map_id=clients.client[connection].map_id;
    int char_tile=clients.client[connection].map_tile;
    int map_axis=maps.map[map_id].map_axis;

    //pre-create the remove char packet so we don't have to repeat this on each occasion when it needs to
    //sent to other actors
    unsigned char packet[1024];
    int packet_length=0;
    remove_actor_packet(connection, packet, &packet_length);

    //cycle through all the clients
    for(i=0; i<MAX_CLIENTS; i++){

        //restrict to clients that are logged in
        if(clients.client[i].client_status==LOGGED_IN){

            //exclude the broadcasting char
            if(i!=connection){

                //restrict to chars on the same map as broadcasting char
                if(map_id==clients.client[i].map_id){

                    int receiver_char_visual_range=get_char_visual_range(i);
                    int receiver_char_tile=clients.client[i].map_tile;

                    //restrict to those chars that can see the broadcasting char
                    if(get_proximity(char_tile, receiver_char_tile, map_axis) < receiver_char_visual_range){

                        //send(i, packet, packet_length, 0);
                        send_packet(connection, packet, packet_length);
                    }
                }
            }
        }
    }
}

void broadcast_actor_packet(int connection, unsigned char move, int sender_destination_tile){

    /** public function - see header */

    int i=0;

    unsigned char packet[1024];// sending char packets
    int packet_length=0;

    unsigned char packet1[1024];// receiving char add_actor packet
    int packet1_length=0;

    unsigned char packet2[1024];// receiving char add_enhanced_actor packet
    int packet2_length=0;

    unsigned char packet3[1024];// receiving char remove_actor packet
    int packet3_length=0;

    int sender_current_tile=clients.client[connection].map_tile;
    int sender_visual_range=get_char_visual_range(connection);

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int proximity_before_move=0;
    int proximity_after_move=0;

    // pre-create packets that will be sent more than once in order to save time
    add_actor_packet(connection, move, packet1, &packet1_length);
    add_new_enhanced_actor_packet(connection, packet2, &packet2_length);
    remove_actor_packet(connection, packet3, &packet3_length);

    // broadcast sender char move to all receiver clients
    for(i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].client_status==LOGGED_IN){

            if(clients.client[i].map_id==clients.client[connection].map_id){

                int receiver_tile=clients.client[i].map_tile;
                int receiver_visual_range=get_char_visual_range(i);

                proximity_before_move=get_proximity(sender_current_tile, receiver_tile, map_axis);
                proximity_after_move=get_proximity(sender_destination_tile, receiver_tile, map_axis);

                //This block deals with receiving char vision
                if(i!=connection){

                    if(proximity_before_move>receiver_visual_range && proximity_after_move<=receiver_visual_range){

                        //sending char moves into visual proximity of receiving char
                        #if DEBUG_BROADCAST==1
                        printf("char [%i] sees sending char [%i] added\n", i, connection);
                        #endif

                        //send(i, packet2, packet2_length, 0);
                        send_packet(i, packet2, packet2_length);
                    }
                    else if(proximity_before_move<=receiver_visual_range && proximity_after_move>receiver_visual_range){

                        //sending char moves out of visual proximity of receiving char
                        #if DEBUG_BROADCAST==1
                        printf("char [%i] sees sending char [%i] removed\n", i, connection);
                        #endif

                        //send(i, packet3, packet3_length, 0);
                        send_packet(i, packet3, packet3_length);
                    }
                    else if(proximity_before_move<=receiver_visual_range && proximity_after_move<=receiver_visual_range){

                        //sending char moving within visual proximity of receiving char
                        #if DEBUG_BROADCAST==1
                        printf("char [%i] sees sending char [%i] move\n", i, connection);
                        #endif

                        //send(i, packet1, packet1_length, 0);
                        send_packet(i, packet1, packet1_length);
                    }
                    else {
                        #if DEBUG_BROADCAST==1
                        printf("char [%i] can't see sending char [%i]\n", i, connection);
                        #endif
                    }
                }

                //this block deals with sending char vision
                if(proximity_before_move>sender_visual_range && proximity_after_move<=sender_visual_range){

                    //sending char moves into visual proximity of receiving char
                    #if DEBUG_BROADCAST==1
                    printf("sending char [%i] sees another char [%i] added\n", connection, i);
                    #endif

                    add_new_enhanced_actor_packet(i, packet, &packet_length);
                    //send(connection, packet, packet_length, 0);
                    send_packet(connection, packet, packet_length);
                }
                else if(proximity_before_move<=sender_visual_range && proximity_after_move>sender_visual_range){

                    //sending char moves out of visual proximity of receiving char
                    #if DEBUG_BROADCAST==1
                    printf("sending char [%i] sees another char [%i] removed\n", connection, i);
                    #endif

                    remove_actor_packet(i, packet, &packet_length);
                    //send(connection, packet, packet_length, 0);
                    send_packet(connection, packet, packet_length);
                }
                else if(proximity_before_move<=sender_visual_range && proximity_after_move<=sender_visual_range){

                    //sending char moves within visual proximity of receiving char
                    if(i==connection) {

                        // sending char sees itself move
                        #if DEBUG_BROADCAST==1
                        printf("sending char [%i] sees itself move\n", i);
                        #endif

                        add_actor_packet(i, move, packet, &packet_length);

                    }
                    else{

                        // sending char sees itself stationery
                        #if DEBUG_BROADCAST==1
                        printf("sending char [%i] sees itself stationary\n", i);
                        #endif

                        add_actor_packet(i, 0, packet, &packet_length);
                    }

                    send_packet(connection, packet, packet_length);
                    //send(connection, packet, packet_length, 0);
                }
            }
        }
    }
}


void broadcast_local_chat(int connection, char *text_in){

    /** public function - see header */

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int i=0;

    for(i=0; i<MAX_CLIENTS; i++){

        if(map_id==clients.client[i].map_id) {

            if(get_proximity(clients.client[connection].map_tile, clients.client[i].map_tile, map_axis)<LOCAL_CHAT_RANGE){

                if(connection!=i) send_raw_text(i, CHAT_LOCAL, text_in);
            }
        }
    }
}

void broadcast_channel_chat(int chan, int connection, char *text_in){

    /** public function - see header */

    char text_out[1024]="";

    //send to channel
    int i=0;

    for(i=0; i<MAX_CLIENTS; i++){

        //don't echo to self
        if(connection!=i){

            //filter out players who are not in this chan
            if(is_player_in_chan(i,chan)!=NOT_FOUND){

                int active_chan=clients.client[i].chan[clients.client[i].active_chan-1];

                //show non-active chan in darker grey
                if(active_chan==chan){

                    sprintf(text_out, "%c[%s@%i]: %s", c_grey1+127, clients.client[connection].char_name, chan, text_in);
                }
                else {

                    sprintf(text_out, "%c[%s@%i]: %s", c_grey2+127, clients.client[connection].char_name, chan, text_in);
                }

                send_raw_text(i, CHAT_SERVER, text_out);
            }
        }
    }
}

void broadcast_channel_event(int chan, int connection, char *text_in){

    /** public function - see header */

    int i=0;

    for(i=0; i<MAX_CLIENTS; i++){

        //don't echo to self
        if(connection!=i){

            //filter out clients who are not in chan
            int in_chan=FALSE;
            int j=0;

            for(j=0; j<MAX_CHAN_SLOTS; j++){

                if(clients.client[i].chan[j]==chan){

                    in_chan=TRUE;
                    break;
                }
            }

            if(in_chan==TRUE){

                send_raw_text(i, CHAT_SERVER, text_in);
            }
        }
    }
}
