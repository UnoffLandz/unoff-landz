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

#ifndef DB_CHARACTER_GENDER_TBL_H_INCLUDED
#define DB_CHARACTER_GENDER_TBL_H_INCLUDED

#define GENDER_TABLE_SQL "CREATE TABLE GENDER_TABLE( \
        GENDER_ID    INTEGER PRIMARY KEY     NOT NULL, \
        GENDER_NAME  TEXT)"

/** RESULT  : loads data from the gender table into the gender array

    RETURNS : void

    PURPOSE : retrieve gender data from permanent storage

    NOTES   :
**/
void load_db_genders();


/** RESULT  : loads gender data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of gender data

    NOTES   :
*/
void batch_add_gender(char *file_name);


#endif // DB_CHARACTER_GENDER_TBL_H_INCLUDED
