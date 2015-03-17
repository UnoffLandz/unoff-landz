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
