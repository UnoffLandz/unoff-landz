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

#ifndef DATABASE_BUFFER_H_INCLUDED
#define DATABASE_BUFFER_H_INCLUDED

#include "database_functions.h" //access to MAX_SQL_LEN

#define DB_BUFFER_MAX 100
#define MAX_PROTOCOL_PACKET_SIZE 160

struct buffer_node_type{

    char sql[MAX_SQL_LEN];
    int connection;
    int process_type;
    unsigned char packet[MAX_PROTOCOL_PACKET_SIZE];
};

struct buffer_list_type {

    int buffer_count;
    struct buffer_node_type buffer[DB_BUFFER_MAX];
};
struct buffer_list_type db_buffer;

enum{//database buffer processing types
    DB_BUFFER_PROCESS_SQL,
    DB_BUFFER_PROCESS_HASH_DETAILS,
    DB_BUFFER_PROCESS_CHECK_NEWCHAR,
    DB_BUFFER_PROCESS_ADD_NEWCHAR,
    DB_BUFFER_PROCESS_LOGIN,
};

/** RESULT  : pushes items on the database buffer

    RETURNS  : void

    PURPOSE  : allows for db actions can be processed during server idle events

    NOTES    :
**/
void db_push_buffer(char *sql, int connection, int process_type, unsigned char *packet);


/** RESULT  : processes an item from the database buffer

    RETURNS  : void

    PURPOSE  : allows for db actions can be processed during server idle events

    NOTES    :
**/
void db_process_buffer();

#endif // DATABASE_BUFFER_H_INCLUDED
