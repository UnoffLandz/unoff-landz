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

#ifndef DB_CHARACTER_TYPE_TBL_H_INCLUDED
#define DB_CHARACTER_TYPE_TBL_H_INCLUDED

#define CHARACTER_TYPE_TABLE_SQL "CREATE TABLE CHARACTER_TYPE_TABLE( \
        CHARACTER_TYPE_ID   INTEGER PRIMARY KEY     NOT NULL, \
        RACE_ID             INT, \
        GENDER_ID           INT, \
        CHAR_COUNT          INT)"

/** RESULT  : loads data from the character type table into the character type array

    RETURNS : void

    PURPOSE : retrieve character type data from permanent storage

    NOTES   :
**/
int load_db_char_types();


/** RESULT  : loads char type data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of char type data

    NOTES   :
*/
void batch_add_char_types(char *file_name);

#endif // DB_CHARACTER_TYPE_TBL_H_INCLUDED
