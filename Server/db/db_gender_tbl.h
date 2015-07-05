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

#ifndef DB_CHARACTER_GENDER_TBL_H_INCLUDED
#define DB_CHARACTER_GENDER_TBL_H_INCLUDED

#define GENDER_TABLE_SQL "CREATE TABLE GENDER_TABLE( \
        GENDER_ID    INTEGER PRIMARY KEY     NOT NULL, \
        GENDER_NAME  TEXT)"

/** RESULT  : loads data from the gender table into the gender array

    RETURNS : number of rows read from the gender table

    PURPOSE : Loads gender data from the database to memory.

    NOTES   :
**/
int load_db_genders();


/** RESULT  : adds a gender to the gender table

    RETURNS : void

    PURPOSE : a test function to load genders to the gender table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_gender(int gender_id, char *gender_name);

#endif // DB_CHARACTER_GENDER_TBL_H_INCLUDED
