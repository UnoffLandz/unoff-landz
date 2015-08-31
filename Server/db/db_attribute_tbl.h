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

#ifndef DB_ATTRIBUTE_TBL_H_INCLUDED
#define DB_ATTRIBUTE_TBL_H_INCLUDED

#define ATTRIBUTE_TABLE_SQL "CREATE TABLE ATTRIBUTE_TABLE( \
        ATTRIBUTE_ID  INTEGER PRIMARY KEY        NOT NULL,  \
        RACE_ID             INT, \
        ATTRIBUTE_TYPE_ID   INT, \
        PICKPOINTS          INT, \
        ATTRIBUTE_VALUE     INT)"

/** RESULT   : adds an attribute to the database

    RETURNS  : void

    PURPOSE  : avoids having to directly edit the database

    NOTES    :
**/
void add_db_attribute(int race_id, int attribute_type_id, int attribute_value[50]);


/** RESULT   : loads attributes and associated attribute values from the database to memory array

    RETURNS  : void

    PURPOSE  : retrieves attribute/attribute value data from permanent storage

    NOTES    :
**/
void load_db_attributes();

#endif // DB_ATTRIBUTE_TBL_H_INCLUDED
