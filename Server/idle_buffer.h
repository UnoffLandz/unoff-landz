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

#ifndef IDLE_BUFFER_H_INCLUDED
#define IDLE_BUFFER_H_INCLUDED

#include "db/database_functions.h" //access to MAX_SQL_LEN

#define IDLE_BUFFER_MAX 100
#define MAX_PROTOCOL_PACKET_SIZE 160

struct buffer_node_type{

    char sql[MAX_SQL_LEN];
    int actor_node;
    int process_type;
    unsigned char packet[MAX_PROTOCOL_PACKET_SIZE];
};

struct buffer_list_type {

    int buffer_count;
    struct buffer_node_type buffer[IDLE_BUFFER_MAX];
};
extern struct buffer_list_type idle_buffer;

enum{//database buffer processing types
    IDLE_BUFFER_PROCESS_SQL,
    IDLE_BUFFER_PROCESS_HASH_DETAILS,
    IDLE_BUFFER_PROCESS_CHECK_NEWCHAR,
    IDLE_BUFFER_PROCESS_ADD_NEWCHAR,
    IDLE_BUFFER_PROCESS_LOGIN,
};

/** RESULT  : pushes items on the database buffer

    RETURNS  : void

    PURPOSE  : allows for db actions can be processed during server idle events

    NOTES    :
**/
void Xpush_idle_buffer(char *sql, int actor_node, int process_type, unsigned char *packet);


/** RESULT  : processes an item from the database buffer

    RETURNS  : void

    PURPOSE  : allows for db actions can be processed during server idle events

    NOTES    :
**/
void Xprocess_idle_buffer();

#endif // IDLE_BUFFER_H_INCLUDED
