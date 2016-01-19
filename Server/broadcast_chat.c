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
#include "character_race.h"
#include "characters.h"
#include "server_messaging.h"
#include "colour.h"
#include "chat.h"
#include "logging.h"
#include "bags.h"
#include "packet.h"


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
;
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

            //filter out players who are not in this chan
            if(player_in_chan(i,chan)!=-1){

                //show non-active chan in darker grey
                unsigned char text_colour = (is_chan_active(i, chan)==true) ? c_grey1+127 : c_grey2+127;

                //broadcast to chars
                send_text(clients.client[i].socket, CHAT_SERVER, "%c[%s @ %i]: %s",
                    text_colour,
                    clients.client[actor_node].char_name,
                    chan,
                    text_in);
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

                send_text(i, chan, text_in);
            }
        }
    }
}

