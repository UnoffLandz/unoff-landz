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

#ifndef DB_CHARACTER_TYPE_TBL_H_INCLUDED
#define DB_CHARACTER_TYPE_TBL_H_INCLUDED

#define CHARACTER_TYPE_TABLE_SQL "CREATE TABLE CHARACTER_TYPE_TABLE( \
        CHARACTER_TYPE_ID   INTEGER PRIMARY KEY     NOT NULL, \
        CHARACTER_TYPE_NAME TEXT, \
        RACE_ID             INT, \
        SEX_ID              INT, \
        CHAR_COUNT          INT)"

/** RESULT  : loads data from the character type table into the character type array

    RETURNS : number of rows read from the character type table

    PURPOSE : Loads character type data from the database to memory.

    NOTES   :
**/
int load_db_char_types();

/** RESULT  : loads an entry to the character type table

    RETURNS : void

    PURPOSE : Loads character type data to the database

    NOTES   :
**/
void add_db_char_type(int char_type_id, int race_id, int gender_id);


#endif // DB_CHARACTER_TYPE_TBL_H_INCLUDED
