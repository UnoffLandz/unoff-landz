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

#include "stdio.h" //supports sprintf

#include "chat.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "clients.h"
#include "broadcast_actor_functions.h"
#include "logging.h"
#include "characters.h"
#include "global.h"
#include "db/database_functions.h"
#include "idle_buffer2.h"

struct channel_node_type channel[MAX_CHANNELS];

int player_in_chan(int connection, int chan){

    /** public function - see header */

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[connection].chan[i]==chan){

            return i;
        }
    }

    return -1;
}


int join_channel(int connection, int chan){

    /** public function - see header */

    char text_out[80]="";

    // check for valid channel number
    if(chan<0 || chan>=MAX_CHANNELS){

        send_text(connection, CHAT_SERVER, "%cyou tried to join an invalid channel", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //stop players from joining system channels
    else if(channel[chan].chan_type==CHAN_SYSTEM){

        send_text(connection, CHAT_SERVER, "%cchannel is reserved for system use", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //stop players from joining vacant channels
    else if(channel[chan].chan_type==CHAN_VACANT){

        send_text(connection, CHAT_SERVER, "%cThat channel is not open", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //stop players from joining guild channels
    else if(channel[chan].chan_type==CHAN_GUILD){

        send_text(connection, CHAT_SERVER, "%cThat channel is for a guild", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //check if player is already in chan
    if(player_in_chan(connection, chan)!=-1){

        send_text(connection, CHAT_SERVER, "%cYou have already joined that chan", c_red3+127);
        return CHANNEL_NOT_JOINED;
    }

    //check if client has a free chan slot
    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        //found free slot
        if(clients.client[connection].chan[i]==0) {

            sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[connection].char_name, channel[chan].channel_name);
            broadcast_channel_event(chan, connection, text_out);

            clients.client[connection].chan[i]=chan;

            char sql[MAX_SQL_LEN]="";
            snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET CHAN_%i=%i WHERE CHAR_ID=%i;", i, chan, clients.client[connection].character_id);
            push_sql_command(sql);

            clients.client[connection].active_chan=i;
            send_get_active_channels(connection);

            snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=%i WHERE CHAR_ID=%i;", clients.client[connection].active_chan, clients.client[connection].character_id);
            push_sql_command(sql);

            //echo back to player which channel was just joined and its description etc
            send_text(connection, CHAT_SERVER, "%cYou joined channel %s", c_green3+127, channel[chan].channel_name);
            send_text(connection, CHAT_SERVER, "%cDescription : %s", c_green2+127, channel[chan].description);

            //THE FOLLOWING IS COMMENTED OUT AS ITS TOO RESOURCE HEAVY...
            //sprintf(text_out, "%cIn channel :", c_green1+127);
            //send_raw_text(connection, CHAT_SERVER, text_out);
            //list_characters_in_chan(connection, chan);

            //tell other in chan that player has joined
            sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[connection].char_name, channel[chan].channel_name);
            broadcast_channel_event(chan, connection, text_out);

            return CHANNEL_JOINED;
        }
    }

    //free chan slot not found
    send_text(connection, CHAT_SERVER, "%cyou have no free channel slots left", c_red3+127);

    return CHANNEL_NOT_JOINED;
}


int leave_channel(int connection, int chan){

    /** public function - see header */

    char text_out[80]="";
    char sql[MAX_SQL_LEN]="";

    int slot=player_in_chan(connection, chan);

    if(slot==-1){

        send_text(connection, CHAT_SERVER, "%cyou are not in this channel", c_red3+127);
        return CHANNEL_NOT_LEFT;
    }

    //echo back to player which channel was just left
    sprintf(text_out, "%c%s has left channel %s", c_yellow2+127, clients.client[connection].char_name, channel[chan].channel_name);
    broadcast_channel_event(chan, connection, text_out);

    //null the channel slot
    clients.client[connection].chan[slot]=0;

    snprintf(sql, MAX_SQL_LEN,"UPDATE CHARACTER_TABLE SET CHAN_%i=%i WHERE CHAR_ID=%i;", slot, chan, clients.client[connection].character_id);
    push_sql_command(sql);

    // need to echo back to player which channel was just joined and its description etc
    send_text(connection, CHAT_SERVER, "%cyou left channel %s", c_green3+127, channel[chan].channel_name);

    //tell others in chan that player has left
    sprintf(text_out, "%c%s has left channel %s", c_yellow2+127, clients.client[connection].char_name, channel[chan].channel_name);
    broadcast_channel_event(chan, connection, text_out);

    //set active channel to next used channel slot or 0
    clients.client[connection].active_chan=0;

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[connection].chan[i]>0){

            clients.client[connection].active_chan=i;
            break;
        }
    }

    send_get_active_channels(connection);

    sprintf(sql, "UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=%i WHERE CHAR_ID=%i;", clients.client[connection].active_chan, clients.client[connection].character_id);
    push_sql_command(sql);

    if(clients.client[connection].active_chan==0){

        send_text(connection, CHAT_SERVER, "%cyou have no channels", c_red3+127);
    }

    return CHANNEL_LEFT;
}


void send_pm(int connection, char *target_name, char *message) {

    // echo message back to sender
    send_text(connection, CHAT_PERSONAL, "%c[PM to %s: %s]", c_orange1+127, target_name, message);

    //check if message recipient is in game
    int target_connection=char_in_game(target_name);

    if(target_connection==NOT_FOUND){

        log_text(EVENT_CHAT, "target character not logged in");
        return;
    }

    send_text(target_connection, CHAT_PERSONAL, "%c[PM from %s: %s]", c_orange1+127, target_name, message);

    return;
}
