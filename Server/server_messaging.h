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

#ifndef SERVER_MESSAGING_H_INCLUDED
#define SERVER_MESSAGING_H_INCLUDED

#define MOTD_FILE "motd.msg"
#define SERVER_WELCOME_MSG "\nWELCOME TO THE UNOFF SERVER\n--------------------------------------------------------------------------------------------"

#include <stdbool.h>

//#define CHAT_CHANNEL_START 5

enum { // channel types
    CHAT_LOCAL     =0,
    CHAT_PERSONAL  =1,
    CHAT_GM        =2,
    CHAT_SERVER    =3,
    CHAT_MOD       =4,
    CHAT_MODPM     =8,
    CHAT_SERVER_PM =9,
    CHAT_CHANNEL_0 =31
};

/** RESULT  : sends the motd header to connection

    RETURNS : void

    PURPOSE : used by function send_motd function in module server_messaging.c and
              process_packet function in module client_protocol_handler

    NOTES   :
**/
void send_motd_header(int socket);


/** RESULT  : sends the content of the motd file to connection

    RETURNS : true/false

    PURPOSE : used by send_motd function and process_hash_commands

    NOTES   :
**/
bool send_motd_file(int socket);


/** RESULT  : sends the motd header and contents of the motd file to a connected client

    RETURNS : void

    PURPOSE : modularises code in function main of module main.c

    NOTES   :
*/
void send_motd(int connection);

#endif // SERVER_MESSAGING_H_INCLUDED
