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
//
// ** attribute_type_id is required because ???

#ifndef DATABASE_FUNCTIONS_H_INCLUDED
#define DATABASE_FUNCTIONS_H_INCLUDED

#define CURRENT_DB_VERSION 2
#define DEFAULT_DATABASE_FILE_NAME "unoff.sqlite"
#define MAX_SQL_LEN 1024

#include <sqlite3.h>

extern sqlite3 *db; // database handle which is set when function open_database is called

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  : processes a sql string

    RETURNS  : void

    PURPOSE  : used by function idle_buffer to execute sql to update or add to database.

    NOTES    :
**/
void process_sql(const char *sql_str);

#ifdef __cplusplus
}
#endif

#include <stdbool.h>

/** RESULT  : Opens sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : void

    PURPOSE : provides a single loggable function to open the database

    NOTES   :
**/
void open_database(const char *database_name);


/** RESULT  : Closes the currently opened sqlite database file

    RETURNS : void

    PURPOSE : provides a single loggable function to close the database

    NOTES   :
**/
void close_database();


/** RESULT  : Determines the number of tables in the database

    RETURNS : The number of tables in the database

    PURPOSE : To determine if a database table structure needs to be created.

    NOTES   :
**/
int database_table_count();


/** RESULT  : creates a database table

    RETURNS  : void

    PURPOSE  : used by function create_new_database. Made public so that individual tables can be
               added from main.c during development

    NOTES    :
**/
void create_database_table(char *sql);


/** RESULT   : creates the default database

    RETURNS  : void

    PURPOSE  :

    NOTES    :
**/
void create_database(const char *db_filename);


/** RESULT   : checks if a database table exists

    RETURNS  : true or false

    PURPOSE  : check database structure on server start

    NOTES    :
**/
bool table_exists(const char *table_name);

#endif // DATABASE_FUNCTIONS_H_INCLUDED
