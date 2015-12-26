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

#include <stdio.h> //support for FILE data type, sprintf, fopen, fgets, fclose
#include <string.h> //support for strcpy

#include "server_messaging.h"
#include "global.h"
#include "date_time_functions.h"
#include "colour.h"
#include "string_functions.h"
#include "clients.h"
#include "game_data.h"
#include "server_protocol_functions.h"
#include "logging.h"
#include "server_parameters.h"

void send_motd_header(int socket){

    /** public function - see header */

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    char chars_in_game_str[1024]="";
    int chars_in_game_count=0;

    //send separator line so MOTD is distinct from previous text
    send_text(socket, CHAT_SERVER, " ");

    //prepare and send server 'up' time
    long time_diff=time(NULL)-game_data.server_start_time;
    long int time_days=time_diff/(24*60*60);
    time_diff=time_diff-(time_days*24*60*60);
    long int time_hrs=time_diff/(60*60);
    time_diff=time_diff-(time_hrs*60*60);
    long int time_mins=time_diff/60;

    send_text(socket, CHAT_SERVER, "%cWelcome to the %s server", c_blue2+127, SERVER_NAME);
    send_text(socket, CHAT_SERVER, "%cWe're running on %s build %s\n", c_blue2+127, VERSION, BUILD);
    send_text(socket, CHAT_SERVER, "%cServer up                  : %i days %i hrs %i mins", c_blue2+127, time_days, time_hrs, time_mins);

    //prepare and send server start time
    get_time_stamp_str(game_data.server_start_time, time_stamp_str);
    get_verbose_date_str(game_data.server_start_time, verbose_date_stamp_str);
    send_text(socket, CHAT_SERVER, "%cServer started at          : %s on %s", c_blue2+127, time_stamp_str, verbose_date_stamp_str);

    //prepare list of names for characters in game
    for(int i=0; i<MAX_ACTORS; i++){

        int socket=clients.client[i].socket;

        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER
        && client_socket[socket].socket_node_status==CLIENT_LOGGED_IN){

            sprintf(chars_in_game_str, "%s %s",  chars_in_game_str, clients.client[i].char_name);
            chars_in_game_count++;
        }
    }

    if(chars_in_game_count==0) {

        send_text(socket, CHAT_SERVER, "%cCharacters in game         : [0] None", c_blue2+127);
    }
    else {

        send_text(socket, CHAT_SERVER, "%cCharacters in game         : [%i] %s", c_blue2+127, chars_in_game_count, chars_in_game_str);
    }

    send_text(socket, CHAT_SERVER, "%cCharacters created to date : %i", c_blue2+127, game_data.char_count);

    if(game_data.char_count>0){

        get_time_stamp_str(game_data.date_last_char_created, time_stamp_str);
        get_verbose_date_str(game_data.date_last_char_created, verbose_date_stamp_str);
     }
    else {

        get_time_stamp_str(time(NULL), time_stamp_str);
        get_verbose_date_str(time(NULL), verbose_date_stamp_str);

        strcpy(game_data.name_last_char_created, "None");
    }

    send_text(socket, CHAT_SERVER, "%cLast character created     : %s at %s %s", c_blue2+127, game_data.name_last_char_created, time_stamp_str, verbose_date_stamp_str);

    //prepare and send time and date of this connection
    get_time_stamp_str(time(NULL), time_stamp_str);
    get_verbose_date_str(time(NULL), verbose_date_stamp_str);
    send_text(socket, CHAT_SERVER, "%cConnection at              : %s on %s", c_blue2+127, time_stamp_str, verbose_date_stamp_str);

    //prepare and send connection ip address
    send_text(socket, CHAT_SERVER, "%cConnection IP address      : %s", c_blue2+127, client_socket[socket].ip_address);

    //send separator line so MOTD is distinct from subsequent text
    send_text(socket, CHAT_SERVER, " ");
}

bool send_motd_file(int socket){

    /** public function - see header */

    FILE *file;
    char line_in[1024]="";

    if((file=fopen(MOTD_FILE, "r"))==NULL) return false;

    while( fgets(line_in, 80, file) != NULL){

        str_trim_right(line_in);

        if(strcmp(line_in, "")==0) strcpy(line_in, " ");

        send_text(socket, CHAT_SERVER, line_in);
    }

    fclose(file);

    return true;
}

void send_motd(int connection){

    /** public function - see header */

    send_motd_header(connection);

    //if there's an motd file then send to client otherwise log that no motd file was found
    if(send_motd_file(connection)!=true){

        log_event(EVENT_SESSION, "no motd file available for connection [%i]", connection);
    }
}

