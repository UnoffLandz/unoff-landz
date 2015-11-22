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

#include <stdio.h> //support for sprintf
#include <string.h> //support for strcpy

#include "log_in.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "string_functions.h"
#include "logging.h"
#include "server_messaging.h"
#include "global.h"
#include "clients.h"
#include "characters.h"
#include "db/db_character_tbl.h"
#include "maps.h"
#include "server_start_stop.h"
#include "game_data.h"
#include "packet.h"
#include "broadcast_actor_functions.h"


void process_log_in(int connection, const unsigned char *packet){

    /** public function - see header **/

    char text[80]="";
    int map_id=0;

    size_t packet_length=get_packet_length(packet);

    char char_name_and_password[80]="";
    char char_name[80]="";
    char password[80]="";

    //we know that there are always 3 bytes before the char name
    //and password, so we can extract this adding
    //3 to the packet start and subtracting 3 from the packet length
    strncpy(char_name_and_password, (char*)packet+3, (size_t)packet_length-3);

    //we know that the char name and password are separated by an
    //ascii space, so we can extract each separately by scanning
    //for two strings separated by a space
    if(sscanf(char_name_and_password, "%s %s", char_name, password)!=2){

        send_text(connection, CHAT_SERVER, "%cSorry, but that caused an error", c_red1+127);

        send_login_not_ok(connection);
        log_event(EVENT_ERROR, "malformed login attempt [%s]", text);
    }

    log_event(EVENT_SESSION, "login attempt char name [%s] password [%s]", char_name, password);

    //get the char_id corresponding to the char name
    int char_id=get_db_char_data(char_name, 0);
    if(char_id==NOT_FOUND) {

        send_you_dont_exist(connection);

        send_text(connection, CHAT_SERVER, "%cunknown character name", c_red1+127);
        log_event(EVENT_SESSION, "login rejected - unknown char name");
        return;
    }

    //save data already inserted into the client struct (so we don't lose this when we
    //copy the contents of the character struct across)
    character.client_status=clients.client[connection].client_status;
    strcpy(character.ip_address, clients.client[connection].ip_address);
    character.time_of_last_heartbeat=clients.client[connection].time_of_last_heartbeat;

    //now copy the data in the character struct to the client struct
    clients.client[connection]=character;

    //check we have the correct password for our char
    if(strcmp(password, clients.client[connection].password)!=0){

        send_login_not_ok(connection);
        log_event(EVENT_SESSION, "login rejected - incorrect password");

        return;
    }

    //add char to map (makes scene visible in client). We need to do this before we inform
    //client if char is dead or banned, otherwise client will not display that message
    map_id=clients.client[connection].map_id;
    if(add_char_to_map(connection, map_id, clients.client[connection].map_tile)==false){

        log_event(EVENT_ERROR, "cannot add char [%s] to map [%s] in function %s: module %s: line %i", char_name, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //send this otherwise warning messages will not be sent to the client screen
    send_you_are(connection);

    //prevent concurrent login on same char
    for(int i=1; i<MAX_CLIENTS; i++){

        if(clients.client[connection].character_id==clients.client[i].character_id && clients.client[i].client_status==LOGGED_IN){

            send_login_not_ok(connection);
            remove_char_from_map(connection);

            log_event(EVENT_SESSION, "concurrent login attempt for char [%s]", char_name);
            send_text(connection, CHAT_SERVER, "%cYou've been kicked from the server because that character is already logged in!", c_red3+127);

            close_connection_slot(connection);
            return;
        }
    }

    //when reconnecting to the client after a disconnect, this closes any open bag inventory.
    send_close_bag(connection);

    switch(clients.client[connection].char_status){

        case CHAR_ALIVE:{

            clients.client[connection].client_status=LOGGED_IN;
            log_event(EVENT_SESSION, "login accepted");

            //record when session commenced so we can calculate time in-game
            clients.client[connection].session_commenced=time(NULL);

            send_login_ok(connection);
            send_here_your_stats(connection);
            send_here_your_inventory(connection);

            /* when the client is disconnected from server, it still allows chat chan tabs to
            be closed. However, if the client reconnects, the chan tabs are not automatically
            reopened, leading to a situation in which the client remains connected to a
            chat channel but without a corresponding chan tab. The solution to this
            appears to be to send a copy of the get_active_channels protocol to the client
            with all channel data set to zero */
            clear_client_channels(connection);

            /*Now resend the get_active_channels protocol with the required channel data and
            this will ensure that any closed chan tabs are reopened */
            send_client_channels(connection);

            int guild_id=clients.client[connection].guild_id;

            if(guild_id>0){

                //if char is a member of a guild, send blank #gm to open GM chat tab on client
                send_raw_text(connection, CHAT_GM, " ");

                // notify guild that char has logged on
                char text_out[80]="";
                sprintf(text_out, "%c%s JOINED THE GAME", c_blue3+127, clients.client[connection].char_name);
                broadcast_guild_chat(guild_id, connection, text_out);
             }

            break;
        }

        case CHAR_DEAD:{

            send_login_not_ok(connection);
            remove_char_from_map(connection);

            log_event(EVENT_SESSION, "login rejected - dead char");

            send_text(connection, CHAT_SERVER, "%cThat character is dead!", c_red1+127);

            close_connection_slot(connection);
            return;
        }

        case CHAR_BANNED:{

            send_login_not_ok(connection);
            remove_char_from_map(connection);

            log_event(EVENT_SESSION, "login rejected - banned char");

            send_text(connection, CHAT_SERVER, "%cThat character is banned!", c_red1+127);

            close_connection_slot(connection);
            return;
        }

        default:{

            send_login_not_ok(connection);
            remove_char_from_map(connection);

            log_event(EVENT_ERROR, "login rejected - unknown char status [%i]", clients.client[connection].char_status);

            close_connection_slot(connection);

            stop_server();
        }
    }
}
