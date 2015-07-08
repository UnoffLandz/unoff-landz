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


#ifndef DB_OBJECT_TBL_H_INCLUDED
#define DB_OBJECT_TBL_H_INCLUDED

#define OBJECT_TABLE_SQL "CREATE TABLE OBJECT_TABLE( \
        OBJECT_ID    INTEGER PRIMARY KEY     NOT NULL, \
        E3D_FILE_NAME   TEXT, \
        OBJECT_NAME     TEXT, \
        IMAGE_ID        INT, \
        HARVESTABLE     INT, \
        EDIBLE          INT)"

/** RESULT  : loads data from the object table into the object array

    RETURNS : number of rows read from the object table

    PURPOSE : Loads object data from the database to memory.

    NOTES   :
**/
int load_db_objects();


/** RESULT  : adds an object to the object table

    RETURNS : void

    PURPOSE : a test function to load objects to the object table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_object(int object_id, char *object_name, int image_id, int harvestable, int edible);


#endif // DB_OBJECT_TBL_H_INCLUDED
