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
