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

#ifndef DB_CHARACTER_RACE_TBL_H_INCLUDED
#define DB_CHARACTER_RACE_TBL_H_INCLUDED

#define RACE_TABLE_SQL "CREATE TABLE RACE_TABLE( \
        RACE_ID             INTEGER PRIMARY KEY     NOT NULL, \
        RACE_NAME           TEXT, \
        RACE_DESCRIPTION    TEXT, \
        INITIAL_EMU         INT  \
        )"

/** RESULT  : loads data from the race table into the race array

    RETURNS : void

    PURPOSE : retrieve guild data from permanent storage

    NOTES   :
**/
void load_db_char_races();


/** RESULT  : loads an entry to the race table

    RETURNS : void

    PURPOSE : Loads race data to the database

    NOTES   :
**/
void add_db_race(int race_id, char *race_name, char *race_description);


/** RESULT  : loads race data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of race data

    NOTES   :
*/
void batch_add_races(char *file_name);


#endif // DB_CHARACTER_RACE_TBL_H_INCLUDED
