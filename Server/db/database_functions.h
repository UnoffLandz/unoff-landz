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

//Explanation of database table relationships
//
//CHARACTER_TYPE_TABLE     RACE_TABLE             ATTRIBUTE_TABLE            ATTRIBUTE VALUE TABLE
//character_type_id   +--> race_id >--------+     attribute_id >--------+    attribute_value_id
//race_id >-----------+    race_name        +     attribute_description +--> attribute_id
//sex_id  >-----------+    race_description +     attribute_type_id**        attribute_type_id**
//                    +                     +-->  race_id                    pickpoints
//                    +    GENDER_TABLE           max_pickpoints             attribute_value
//                    +--> gender_id
//                         gender_name

#ifndef DATABASE_FUNCTIONS_H_INCLUDED
#define DATABASE_FUNCTIONS_H_INCLUDED

#include <sqlite3.h>

#define DEFAULT_DATABASE_FILE_NAME "unoff.sqlite"
#define MAX_SQL_LEN 1024
#define MAX_DB_TABLES 10

struct database_table_type{

    char tbl_name[80];
    char sql[1024];
};
extern struct database_table_type database_table[MAX_DB_TABLES];

extern sqlite3 *db; // database handle which is set when function open_database is called

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT   : processes a sql string

    RETURNS  : void

    PURPOSE  : used to process sql statements where there is no output

    NOTES    :
**/
void process_sql(const char *sql, const char *module, const char *func, const int line);

#ifdef __cplusplus
}
#endif

#include <stdbool.h>

/** RESULT  :   Opens an existing sqlite database file and creates a global handle [db]
                which can then be called by other database functions.

    RETURNS :   void

    PURPOSE :   provides a single loggable function to open the database

    NOTES   :
**/
void open_database(const char *db_filename, const char *module, const char *func, const int line);


/** RESULT  : Closes the currently opened sqlite database file

    RETURNS : void

    PURPOSE : provides a single loggable function to close the database

    NOTES   :
**/
void close_database(const char *module, const char *func, const int line);


/** RESULT  : Determines the number of tables in the database

    RETURNS : The number of tables in the database

    PURPOSE : To determine if a database table structure needs to be created.

    NOTES   :
**/
int database_table_count();


/** RESULT  : creates a database table

    RETURNS  : void

    PURPOSE  : convenience wrapper for process_sql function which provides logging when
               adding database tables

    NOTES    :
**/
void create_database_table(char *sql);


/** RESULT   : populates an empty sqlite database with data

    RETURNS  : void

    PURPOSE  :

    NOTES    :
**/
void populate_database(const char *db_filename);


/** RESULT   : checks if a database table exists

    RETURNS  : true or false

    PURPOSE  : check database structure on server start

    NOTES    :
**/
bool table_exists(const char *table_name);


/** RESULT   : gets the version of the sqlite database

    RETURNS  : the sqlite version number

    PURPOSE  : used to check if the sqlite database matches the current codebase

    NOTES    :
**/
int get_database_version();


/** RESULT   : reports if database is closed when it should be open

    RETURNS  : void

    PURPOSE  : reporting wrapper

    NOTES    :
**/
void check_db_open(const char *module, const char *func, const int line);


/** RESULT   : reports if database is open when it should be closed

    RETURNS  : void

    PURPOSE  : reporting wrapper

    NOTES    :
**/
void check_db_closed(const char *module, const char *func, const int line);


/** RESULT   : reports if a table exists within the database

    RETURNS  : void

    PURPOSE  : reporting wrapper for table_exists function

    NOTES    :
**/
void check_table_exists(char *table_name, const char *module, const char *func, const int line);


/** RESULT   : wrapper for sqlite3_prepare_v2 function

    RETURNS  : void

    PURPOSE  : wraps reporting code

    NOTES    :
**/
void prepare_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line);


/** RESULT   : wrapper for sqlite3_finalize function

    RETURNS  : void

    PURPOSE  : wraps reporting code

    NOTES    :
**/
void destroy_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line);


/** RESULT   : wrapper for sqlite3_step function

    RETURNS  : void

    PURPOSE  : wraps reporting code

    NOTES    :
**/
void step_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line);


#endif // DATABASE_FUNCTIONS_H_INCLUDED
