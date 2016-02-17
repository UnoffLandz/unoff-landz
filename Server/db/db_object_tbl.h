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


#ifndef DB_OBJECT_TBL_H_INCLUDED
#define DB_OBJECT_TBL_H_INCLUDED

#define OBJECT_TABLE_SQL "CREATE TABLE OBJECT_TABLE( \
        OBJECT_ID           INTEGER PRIMARY KEY     NOT NULL, \
        OBJECT_NAME         TEXT, \
        HARVESTABLE         INT,  \
        EDIBLE              INT,  \
        HARVEST_INTERVAL    INT,  \
        EMU                 INT, \
        EQUIPABLE_ITEM_TYPE INT,  \
        EQUIPABLE_ITEM_ID   INT)"

/** RESULT  : loads data from the object table into the object array

    RETURNS : void

    PURPOSE : retrieve object data from permanent storage

    NOTES   :
**/
void load_db_objects();


/** RESULT  : loads object data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of objects

    NOTES   :
*/
void batch_add_objects(char *file_name);

#endif // DB_OBJECT_TBL_H_INCLUDED
