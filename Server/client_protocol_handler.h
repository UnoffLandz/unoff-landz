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

#ifndef CLIENT_PROTOCOL_HANDLER_H_INCLUDED
#define CLIENT_PROTOCOL_HANDLER_H_INCLUDED


enum{ //values for the SIT_DOWN protocol
    SIT=0,
    STAND=1,
};

/** RESULT  : processes a data packet received from the client

    RETURNS : void

    PURPOSE : reduce need for code in main.c

    NOTES   :
*/
void process_packet(int actor_node, unsigned char *packet);

#endif // CLIENT_PROTOCOL_HANDLER_H_INCLUDED
