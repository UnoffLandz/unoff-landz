#ifndef DATABASE_FUNCTIONS_H_INCLUDED
#define DATABASE_FUNCTIONS_H_INCLUDED

#define DATABASE_FILE_NAME "unoff.db"
#define MAX_SQL_LEN 1024

#include <sqlite3.h>

sqlite3 *db; // database handle which is set when function open_database is called

/** RESULT  : Opens sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : void

    PURPOSE : Reduces the need to open and close the database each time a query is used
**/
void open_database(char *database_name);


/** RESULT  : Determines the number of tables in the database

    RETURNS : The number of tables in the database

    PURPOSE : To determine if a database table structure needs to be created.
**/
int database_table_count();


/** RESULT  : creates a new database table structure and populates it with data

    RETURNS : void

    PURPOSE : To create a new database
**/
void create_new_database();

/** RESULT  : creates a database table

    RETURNS  : void

    PURPOSE  : used by function create_new_database. Made public so that individual tables can be
               added from main.c during development
**/
void create_database_table(char *sql);


/** RESULT  : processes a sql string

    RETURNS  : void

    PURPOSE  : used by function db_buffer to execute sql to update or add to database.
**/
void process_sql(char *sql_str);

#endif // DATABASE_FUNCTIONS_H_INCLUDED
