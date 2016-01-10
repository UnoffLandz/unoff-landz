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
#include "broadcast_chat.h"

void process_log_in(int actor_node, const unsigned char *packet){

    /** public function - see header **/

    char text[80]="";
    int socket=clients.client[actor_node].socket;

    size_t packet_length=get_packet_length(packet);

    char char_name_and_password[MAX_CHAR_NAME_LEN + MAX_CHAR_PASSWORD_LEN]="";
    char char_name[MAX_CHAR_NAME_LEN]="";
    char password[MAX_CHAR_PASSWORD_LEN]="";

    //we know that there are always 3 bytes before the char name
    //and password, so we can extract this adding
    //3 to the packet start and subtracting 3 from the packet length
    strncpy(char_name_and_password, (char*)packet+3, (size_t)packet_length-3);

    //we know that the char name and password are separated by an
    //ascii space, so we can extract each separately by scanning
    //for two strings separated by a space
    if(sscanf(char_name_and_password, "%s %s", char_name, password)!=2){

        send_text(socket, CHAT_SERVER, "%cSorry, but that caused an error", c_red1+127);

        send_login_not_ok(socket);
        log_event(EVENT_ERROR, "malformed login attempt [%s]", text);
    }

    log_event(EVENT_SESSION, "login attempt char name [%s] password [%s]", char_name, password);

    //get the char_id corresponding to the char name and load char from the database into
    //the character struct
    int char_id=get_db_char_data(char_name, 0);

    if(char_id==-1) {

        send_you_dont_exist(socket);
        log_event(EVENT_SESSION, "login rejected - unknown char name");
        return;
    }

    //check we have the correct password for our char
    if(strcmp(password, character.password)!=0){

        send_login_not_ok(socket);
        log_event(EVENT_SESSION, "login rejected for char [%s] - incorrect password", char_name);
        return;
    }

    //add connection data to character struct
    character.socket=clients.client[actor_node].socket;
    character.client_node_status=clients.client[actor_node].client_node_status;
    character.player_type=clients.client[actor_node].player_type;

    //copy the character struct to the client struct
    clients.client[actor_node]=character;

    //set the socket status to show that client has changed from [connected] to [logged in]
    client_socket[socket].socket_node_status=CLIENT_LOGGED_IN;

    //send this otherwise warning messages will not be sent to the client screen
    send_you_are(socket);

    //prevent concurrent login on same char
    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].socket!=socket
        && clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].character_id==clients.client[actor_node].character_id){

            //transport duplicate char to an empty map
            send_change_map(socket, "./maps/nomap.elm");

            send_login_ok(socket);//need to send login_ok otherwise screen will not display
                                  //message indicating that char is already logged in
            log_event(EVENT_SESSION, "concurrent login attempt for char [%s] on ip [%c.%c.%c.%c]", char_name,
            client_socket[socket].ip_address[0], client_socket[socket].ip_address[1], client_socket[socket].ip_address[2], client_socket[socket].ip_address[3]);

            send_text(socket, CHAT_SERVER, "%cthat character is already logged in!", c_red3+127);

            //issue kill command for the connection
            client_socket[socket].kill_connection=true;
            return;
        }
    }

    //add char to map
    int map_id=clients.client[actor_node].map_id;
    int map_tile=clients.client[actor_node].map_tile;

    if(add_char_to_map(actor_node, map_id, map_tile)==false){

        log_event(EVENT_ERROR, "cannot add char [%s] to map [%s] in function %s: module %s: line %i", char_name, maps.map[map_id].map_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //when reconnecting to the client after a disconnect, this closes any open bag inventory.
    send_close_bag(socket);

    // TODO (themuntdregger#1#): add close inventory grid to login handling

    switch(clients.client[actor_node].char_status){

        case CHAR_ALIVE:{

            log_event(EVENT_SESSION, "login accepted");

            //record when session commenced so we can calculate time in-game
            clients.client[actor_node].session_commenced=time(NULL);

            send_login_ok(socket);
            send_here_your_stats(socket);
            send_here_your_inventory(socket);

            /* when the client is disconnected from server, it still allows chat chan tabs to
            be closed. However, if the client reconnects, the chan tabs are not automatically
            reopened, leading to a situation in which the client remains connected to a
            chat channel but without a corresponding chan tab. The solution to this
            appears to be to send a copy of the get_active_channels protocol to the client
            with all channel data set to zero */
            //clear_client_channels(actor_node);

            /*Now resend the get_active_channels protocol with the required channel data and
            this will ensure that any closed chan tabs are reopened */
            send_client_channels(actor_node);

            int guild_id=clients.client[actor_node].guild_id;

            if(guild_id>0){

                //if char is a member of a guild, send blank #gm to open GM chat tab on client
                send_raw_text(socket, CHAT_GM, " ");

                // notify guild that char has logged on
                char text_out[80]="";
                sprintf(text_out, "%c%s JOINED THE GAME", c_blue3+127, clients.client[actor_node].char_name);
                broadcast_guild_chat(guild_id, actor_node, text_out);
            }

            break;
        }

        case CHAR_DEAD:{

            send_login_not_ok(socket);

            log_event(EVENT_SESSION, "login rejected for [%s] - dead char", clients.client[actor_node].char_name);

            send_text(socket, CHAT_SERVER, "%cThat character is dead!", c_red1+127);

            //issue kill command for the socket
            client_socket[socket].kill_connection=true;
            break;
        }

        case CHAR_BANNED:{

            send_login_not_ok(socket);

            log_event(EVENT_SESSION, "login rejected for [%s] - banned char", clients.client[actor_node].char_name);

            send_text(socket, CHAT_SERVER, "%cThat character is banned!", c_red1+127);

            //issue kill command for the socket
            client_socket[socket].kill_connection=true;
            break;
        }

        default:{

            log_event(EVENT_ERROR, "login rejected for [%s] - unknown char status [%i]", clients.client[actor_node].char_name, clients.client[actor_node].char_status);
            stop_server();
        }
    }
}
