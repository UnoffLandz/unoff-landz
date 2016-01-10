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

#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

#include <unistd.h> //support for ssize_t datatype

#define MAX_PACKET_SIZE 1024

/*
struct packet_element_type{

    enum {  PROTOCOL=1,     //a uint8
            DATA_LENGTH=2,  //a uint16 carrying an automatically calculated data length value
            UINT16=3,       //a uint16
            BYTE=4,         //a uint8
            STRING_NULL=5,  //a character string terminated by an ascii null (0)
            STRING_SPACE=6, //a character string terminated by an ascii space (32)
            UINT32=7        //a uint32
    } data_type;

    union{

        int numeric;
        char string[1024];
    }data;
};
*/

/** RESULT  : sends packet from the server

    RETURNS : void

    PURPOSE : groups all server communications so as packets sent from server can be monitored
              from a single source

    NOTES   :
*/
void send_packet(int connection, void *packet, size_t packet_length);


/** RESULT  : creates a packet

    RETURNS : packet length

    PURPOSE : provides a common packet creation framework

    NOTES   :
*/
//int build_packet(struct packet_element_type *element, int element_count, unsigned char *packet);


/** RESULT  : reads a packet

    RETURNS : packet length

    PURPOSE : provides a common packet reading framework

    NOTES   :
*/
//void read_packet(struct packet_element_type *element, int element_count, const unsigned char *packet);


/** RESULT  : reads a packet to determine the packet length

    RETURNS : packet length

    PURPOSE : code reusability

    NOTES   :
*/
size_t get_packet_length(const unsigned char *packet);


#endif // PACKET_H_INCLUDED
