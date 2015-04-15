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

#ifndef DB_MAP_TBL_H_INCLUDED
#define DB_MAP_TBL_H_INCLUDED

#define MAP_TABLE_SQL "CREATE TABLE MAP_TABLE( \
        MAP_ID              INTEGER PRIMARY KEY     NOT NULL, \
        MAP_NAME            TEXT, \
        ELM_FILE_NAME       TEXT, \
        ELM_FILE            BLOB \
        )"

/** RESULT  : loads data from the map table into the map array

    RETURNS : number of rows read from the map table

    PURPOSE : Loads map data from the database to memory.

    NOTES   :
**/
int load_db_maps();


/** RESULT  : adds a map to the map table

    RETURNS : void

    PURPOSE : loads a map to the map table

    NOTES   :
**/
void add_db_map(int map_id, char *map_name, char *elm_file_name);


/** RESULT  : updates a map to the map table

    RETURNS : void

    PURPOSE : updates a a map to the map table

    NOTES   : used in command line option
**/
void update_db_map(int map_id, char *map_name, char *elm_file_name);


/** RESULT  : determines if map id exists on database

    RETURNS : TRUE is map_id is already in MAP TABLE, otherwise FALSE

    PURPOSE : to enable the -L command line option to determine if a new map entry should be added
              or an existing one updated

    NOTES   :
**/
int get_db_map_exists(int map_id);

#endif // DB_MAP_TBL_H_INCLUDED
