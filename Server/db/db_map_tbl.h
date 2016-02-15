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
//#define TWOD_OBJECT_MAP_ENTRY_MAX 5000_
//#define TWOD_OBJECT_MAP_HASH_LENGTH ???

#define THREED_OBJECT_MAP_BYTE_MAX 800000
#define THREED_OBJECT_MAP_ENTRY_MAX 5000
#define THREED_OBJECT_HASH_LENGTH 144

#define MAP_TABLE_SQL "CREATE TABLE MAP_TABLE( \
        MAP_ID              INTEGER PRIMARY KEY     NOT NULL, \
        MAP_NAME            TEXT, \
        MAP_DESCRIPTION     TEXT, \
        ELM_FILE_NAME       TEXT, \
        MAP_AXIS            INT,  \
        TILE_MAP            BLOB, \
        HEIGHT_MAP          BLOB, \
        MAP_AUTHOR          TEXT, \
        MAP_AUTHOR_EMAIL    TEXT, \
        MAP_UPLOAD_DATE     INT,  \
        MAP_STATUS          INT, \
        THREED_OBJECT_COUNT INT \
        )"

/** RESULT  : loads data from the map table into the map array

    RETURNS : void

    PURPOSE : retrieve map data from permanent storage

    NOTES   :
**/
void load_db_maps();


/** RESULT  : adds a map to the map table

    RETURNS : void

    PURPOSE : loads a map to the map table

    NOTES   :
**/
void add_db_map(int map_id, char *map_name);


/** RESULT  : determines if map id exists on database

    RETURNS : TRUE is map_id is already in MAP TABLE, otherwise FALSE

    PURPOSE : to enable the -L command line option to determine if a new map entry should be added
              or an existing one updated

    NOTES   :
**/
bool get_db_map_exists(int map_id);


/** RESULT  : lists maps in database

    RETURNS : void

    PURPOSE : enables server users to determine which maps are loaded to database without having to directly
              interrogate the database

    NOTES   :
**/
void list_db_maps();


/** RESULT  : deletes a map from the database

    RETURNS : void

    PURPOSE : enables an existing map to be replaced

    NOTES   : used in function main.c
**/
void delete_map(int map_id);


/** RESULT  : adds/changes map name to the database

    RETURNS : void

    PURPOSE :

    NOTES   :used in function create_database
**/
void change_db_map_name(int map_id, char *map_name);


/** RESULT  : adds/changes map description to the database

    RETURNS : void

    PURPOSE :

    NOTES   :used in function create_database
**/
void change_db_map_description(int map_id, char *map_description);


/** RESULT  : adds/changes map author to the database

    RETURNS : void

    PURPOSE :

    NOTES   :used in function create_database
**/
void change_db_map_author(int map_id, char *map_author);


/** RESULT  : adds/changes map author email to the database

    RETURNS : void

    PURPOSE :

    NOTES   :used in function create_database
**/
void change_db_map_author_email(int map_id, char *map_author_email);


/** RESULT  : adds/changes map development status to the database

    RETURNS : void

    PURPOSE :

    NOTES   :used in function create_database
**/
void change_db_map_development_status(int map_id, int development_status);


/** RESULT  : loads map data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of maps

    NOTES   :
*/
void batch_add_maps(char *file_name);

#endif // DB_MAP_TBL_H_INCLUDED
