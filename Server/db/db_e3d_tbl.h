/*****************************************************************************************************
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
******************************************************************************************************/

#ifndef DB_E3D_TBL_H_INCLUDED
#define DB_E3D_TBL_H_INCLUDED

#define E3D_TABLE_SQL "CREATE TABLE E3D_TABLE( \
        E3D_ID    INTEGER PRIMARY KEY     NOT NULL, \
        E3D_FILENAME   TEXT, \
        OBJECT_ID      INT)"

/** RESULT  : loads data from the e3d table into the e3d array

    RETURNS : void

    PURPOSE : retrieve e3d data from permanent storage

    NOTES   :
**/
void load_db_e3ds();


/** RESULT  : adds an e3d to the e3d table

    RETURNS : void

    PURPOSE : a test function to e3ds to the e3d table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_e3d(int e3d_id, char *e3d_file_name, int object_id);


/** RESULT  : loads e3d data specified in a text file

    RETURNS : void

    PURPOSE : batch loading of e3d data

    NOTES   :
*/
void batch_add_e3ds(char *file_name);

#endif // DB_E3D_TBL_H_INCLUDED
