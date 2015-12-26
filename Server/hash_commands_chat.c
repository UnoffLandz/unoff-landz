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

#include <stdio.h> //supports sprintf function
#include <stdlib.h> //support atoi function
#include <string.h> //supports strcmp function

#include "string_functions.h"
#include "logging.h"
#include "clients.h"
#include "characters.h"
#include "character_race.h"
#include "colour.h"
#include "server_messaging.h"
#include "maps.h"
#include "movement.h"
#include "date_time_functions.h"
#include "server_protocol_functions.h"
#include "gender.h"
#include "character_type.h"
#include "chat.h"
#include "game_data.h"
#include "guilds.h"
#include "idle_buffer2.h"
#include "broadcast_actor_functions.h"
#include "hash_commands.h"


int hash_jc(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    int chan_id=0;

    if(sscanf(text, "%*s %i", &chan_id)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #JC [channel number]", c_red3+127);
        return 0;
    }

    join_channel(actor_node, chan_id);
    return 0;
}


int hash_lc(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    int chan_id=0;

    if(sscanf(text, "%*s %i", &chan_id)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #LC [channel number]", c_red3+127);
        return 0;
    }

    leave_channel(actor_node, chan_id);
    return 0;
}


int hash_cl(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;

    send_text(socket, CHAT_SERVER, "\n%cNo   Channel    Description", c_blue1+127);

    for(int i=0; i<MAX_CHANNELS; i++){

        if(channel[i].chan_type!=CHAN_VACANT) {

            send_text(socket, CHAT_SERVER, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channel[i].channel_name, channel[i].description);
        }
    }

    return 0;
}


int hash_cp(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;

    int active_chan_slot=clients.client[actor_node].active_chan;

    if(active_chan_slot==0){

        send_text(socket, CHAT_SERVER, "%cNo active channel", c_red3+127);
        return 0;
    }

    int chan_id=clients.client[socket].chan[active_chan_slot-31];

    send_text(socket, CHAT_SERVER, "%cListing for channel [%i]: %s", c_blue1+127, chan_id, channel[chan_id].channel_name);
    send_text(socket, CHAT_SERVER, "%cDescription: %s", c_blue1+127, channel[chan_id].description);
    send_text(socket, CHAT_SERVER, "%cCharacters in channel...", c_blue1+127);

    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_USED && clients.client[i].player_type==PLAYER){

            if(player_in_chan(i, chan_id)!=-1){

                send_text(socket, CHAT_SERVER, "%c%s ", c_blue1+127, clients.client[i].char_name);
            }
        }
    }

    return 0;
}


int hash_pm(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char char_name[80]="";
    char message[1024]="";

    if(sscanf(text, "%*s %s %[^\n]", char_name, message)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #PM [character name][message]", c_red3+127);
        return 0;
    }

    send_pm(actor_node, char_name, message);
    return 0;
}
