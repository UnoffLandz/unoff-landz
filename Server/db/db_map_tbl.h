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
*******************************************************************************************************************

                              HOW MAP DATA IS LOADED AND USED

On server-start, the function load_db_maps parses each map in the database MAP TABLE and extracts
relevant data from the map blob which is then used to populate the map struct. The map blob contains
a list of 3d objects used in the map, comprising of the e3d file name and position, which is
extracted to an array in the map struct.

MAP TABLE                    MAP STRUCT

map blob with                1) position of each object in the 3d object list
3d object list               2) the e3d filename of the object >-----------------------
                        ---> 3) the image id for the object in the OBJECT STRUCT      |
                        |                                                             |
OBJECT E3D TABLE        |    OBJECT E3D STRUCT                                        |
                        |                                                             |
1) entry id             |    1) entry id                                              |
2) e3d_filename         |    2) e3d_filename  <----------------------------------------
                        |    3) the image id for the object in the OBJECT STRUCT >-----
                        |                                                             |
OBJECT TABLE            |    OBJECT STRUCT                                            |
                        |                                                             |
image id                ---< 1) image id <---------------------------------------------
name                         2) name
harvestable                  3) harvestable
edible                       4) edible

*****************************************************************************************************/

#ifndef DB_MAP_TBL_H_INCLUDED
#define DB_MAP_TBL_H_INCLUDED

//#define TWOD_OBJECT_MAP_BYTE_MAX 800000
#define THREED_OBJECT_MAP_BYTE_MAX 800000
#define THREED_OBJECT_MAP_ENTRY_MAX 5000
#define THREED_OBJECT_HASH_LENGTH 132

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


/** RESULT  : determines if map id exists on database

    RETURNS : TRUE is map_id is already in MAP TABLE, otherwise FALSE

    PURPOSE : to enable the -L command line option to determine if a new map entry should be added
              or an existing one updated

    NOTES   :
**/
int get_db_map_exists(int map_id);


void add_db_map2(int map_id, char *map_name, char *elm_file_name);


#endif // DB_MAP_TBL_H_INCLUDED
