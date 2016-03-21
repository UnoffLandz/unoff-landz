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

#ifndef MAP_OBJECT_H_INCLUDED
#define MAP_OBJECT_H_INCLUDED

#define MAP_OBJECT_TABLE_SQL "CREATE TABLE MAP_OBJECT_TABLE( \
        ID    INTEGER PRIMARY KEY     NOT NULL, \
        THREEDOL_ID   INT, \
        MAP_ID        INT, \
        TILE          INT, \
        E3D_ID        INT, \
        HARVESTABLE   INT, \
        RESERVE       INT, \
        E3D_FILENAME  TEXT)"


/** RESULT  : loads data from the map object table into the map object array

    RETURNS : void

    PURPOSE : retrieve map object data from permanent storage

    NOTES   :
**/
void load_db_map_objects();


/** RESULT  : adds the threed object list for a map to the database

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void add_db_map_objects(int map_id, char *elm_filename);


/** RESULT  : updates the map_object table with the e3d table

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void update_db_map_objects(int map_id);


/** RESULT  : synchronises the map_object table with changes in the e3d table

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void batch_update_map_objects(char *file_name);


/** RESULT  : loads map object data

    RETURNS : void

    PURPOSE : batch loading of map object data

    NOTES   :
*/
void batch_add_map_objects();

#endif // MAP_OBJECT_H_INCLUDED
