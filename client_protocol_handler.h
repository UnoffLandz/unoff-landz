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

#ifndef CLIENT_PROTOCOL_HANDLER_H_INCLUDED
#define CLIENT_PROTOCOL_HANDLER_H_INCLUDED


/** RESULT  : gets the character name and password from the create_char packet

    RETURNS : void

    PURPOSE : used in DB_BUFFER_PROCESS_ADD_NEWCHAR and DB_BUFFER_PROCESS_CHECK_NEWCHAR sections
              of the db_process_buffer function

    NOTES   :
*/
int get_name_and_password_from_newchar_packet(unsigned char *packet, char *char_name, char *password);


/** RESULT  : processes a data packet received from the client

    RETURNS : void

    PURPOSE : reduce need for code in main.c

    NOTES   :
*/
void process_packet(int connection, unsigned char *packet);
//void process_packet(int connection, unsigned char *packet, struct ev_loop *loop);

#endif // CLIENT_PROTOCOL_HANDLER_H_INCLUDED
