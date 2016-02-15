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

#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <unistd.h> //support for ssize_t datatype

#define MAX_PACKET_SIZE 1024


/** RESULT  : sends packet from the server

    RETURNS : void

    PURPOSE : groups all server communications so as packets sent from server can be monitored
              from a single source

    NOTES   :
*/
void send_packet(int actor_node, void *packet, size_t packet_length);


/** RESULT  : reads a packet to determine the packet length

    RETURNS : packet length

    PURPOSE : code reusability

    NOTES   :
*/
size_t get_packet_length(const unsigned char *packet);


#endif // PACKET_H_INCLUDED
