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

#include "stdio.h" //supports sprintf

#include "chat.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "clients.h"
#include "broadcast_chat.h"
#include "logging.h"
#include "characters.h"
#include "db/database_functions.h"
#include "idle_buffer2.h"


struct channel_node_type channel[MAX_CHANNELS];


int player_in_chan(int actor_node, int chan){

    /** public function - see header */

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[actor_node].chan[i]==chan){

            return i;
        }
    }

    return -1;
}


bool is_chan_active(int actor_node, int chan){

    /** public function - see header */

    int active_chan_slot=clients.client[actor_node].active_chan;
    int active_chan=clients.client[actor_node].chan[active_chan_slot];

    if(active_chan==chan) return true;

    return false;
}


bool chat_chan_open(int actor_node){

    /** public function - see header */

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[actor_node].chan[i]>0) return true;
    }

    return false;
}


int join_channel(int actor_node, int chan){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char text_out[80]="";

    // check for valid channel number
    if(chan<0 || chan>=MAX_CHANNELS){

        send_text(socket, CHAT_SERVER, "%cyou tried to join an invalid channel", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //stop players from joining system channels
    else if(channel[chan].chan_type==CHAN_SYSTEM){

        send_text(socket, CHAT_SERVER, "%cchannel is reserved for system use", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //stop players from joining vacant channels
    else if(channel[chan].chan_type==CHAN_VACANT){

        send_text(socket, CHAT_SERVER, "%cThat channel is not open", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //check if player is already in chan
    if(player_in_chan(actor_node, chan)!=-1){

        send_text(socket, CHAT_SERVER, "%cYou have already joined that chan", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //check if client has a free chan slot
    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        //found free slot
        if(clients.client[actor_node].chan[i]==0) {

            sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[actor_node].char_name, channel[chan].channel_name);
            broadcast_channel_event(chan, actor_node, text_out);

            //set the new chan ans make it active
            clients.client[actor_node].chan[i]=chan;
            clients.client[actor_node].active_chan=i;

            //send chans to client
            send_get_active_channels(clients.client[actor_node].socket);

            //update database
            push_sql_command("UPDATE CHARACTER_TABLE SET CHAN_0=%i, CHAN_1=%i, CHAN_2=%i, ACTIVE_CHAN=%i WHERE CHAR_ID=%i;",
            clients.client[actor_node].chan[0],
            clients.client[actor_node].chan[1],
            clients.client[actor_node].chan[2],
            clients.client[actor_node].active_chan,
            clients.client[actor_node].character_id);

            //echo back to player which channel was just joined and its description etc
            send_text(socket, CHAT_SERVER, "%cYou joined channel %s", c_green3+127, channel[chan].channel_name);
            send_text(socket, CHAT_SERVER, "%cDescription : %s", c_green2+127, channel[chan].description);

            //tell other in chan that player has joined
            sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[actor_node].char_name, channel[chan].channel_name);
            broadcast_channel_event(chan, actor_node, text_out);

            return CHANNEL_JOINED;
        }
    }

    //free chan slot not found
    send_text(socket, CHAT_SERVER, "%cyou have no free channel slots left", c_red3+127);

    return CHANNEL_NOT_JOINED;
}


int leave_channel(int actor_node, int chan){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char text_out[80]="";

    int slot=player_in_chan(actor_node, chan);

    if(slot==-1){

        send_text(socket, CHAT_SERVER, "%cyou are not in this channel", c_red3+127);
        return CHANNEL_NOT_LEFT;
    }

    //echo back to player which channel was just left
    sprintf(text_out, "%c%s has left channel %s", c_yellow2+127, clients.client[actor_node].char_name, channel[chan].channel_name);
    broadcast_channel_event(chan, actor_node, text_out);

    //null the channel slot
    clients.client[actor_node].chan[slot]=0;

    // need to echo back to player which channel was just joined and its description etc
    send_text(socket, CHAT_SERVER, "%cyou left channel %s", c_green3+127, channel[chan].channel_name);

    //tell others in chan that player has left
    sprintf(text_out, "%c%s has left channel %s", c_yellow2+127, clients.client[actor_node].char_name, channel[chan].channel_name);
    broadcast_channel_event(chan, actor_node, text_out);

    //set active channel to next used channel slot or 0
    clients.client[actor_node].active_chan=0;

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[actor_node].chan[i]>0){

            clients.client[actor_node].active_chan=i;
            break;
        }
    }

    send_get_active_channels(clients.client[actor_node].socket);

    //update database
    push_sql_command("UPDATE CHARACTER_TABLE SET CHAN_0=%i, CHAN_1=%i, CHAN_2=%i, ACTIVE_CHAN=%i WHERE CHAR_ID=%i;",
    clients.client[actor_node].chan[0],
    clients.client[actor_node].chan[1],
    clients.client[actor_node].chan[2],
    clients.client[actor_node].active_chan,
    clients.client[actor_node].character_id);

    if(clients.client[actor_node].active_chan==0){

        send_text(socket, CHAT_SERVER, "%cyou have no channels", c_red3+127);
    }

    return CHANNEL_LEFT;
}


void send_pm(int actor_node, char *target_name, char *message) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    //check if message recipient is in game
    int receiver_actor_node=char_in_game(target_name);

    log_event(EVENT_CHAT, "PM from [%s] to [%s]", clients.client[actor_node].char_name, target_name);

    if(receiver_actor_node==-1){

        //send message back to sending char
        send_text(socket, CHAT_PERSONAL, "%cMessage failed - the intended recipient of your message does not exist or is not in game", c_red3+127, target_name);

        log_event(EVENT_CHAT, "Message not sent as intended recipient [%s] did not exist or was not in-game", target_name);
    }
    else {

        //send message back to sending char
        send_text(socket, CHAT_PERSONAL, "%c[PM to %s: %s]", c_orange1+127, target_name, message);

        //send message to receiving char
        int receiver_actor_socket=clients.client[receiver_actor_node].socket;
        send_text(receiver_actor_socket, CHAT_PERSONAL, "%c[PM from %s: %s]", c_orange1+127, target_name, message);
    }

    return;
}


void send_client_channels(int actor_node){

    /** public function - see header */

    send_get_active_channels(clients.client[actor_node].socket);
}
