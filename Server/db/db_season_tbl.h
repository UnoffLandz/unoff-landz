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

#ifndef DB_SEASON_TBL_H_INCLUDED
#define DB_SEASON_TBL_H_INCLUDED

#define SEASON_TABLE_SQL "CREATE TABLE SEASON_TABLE( \
        SEASON_ID        INTEGER PRIMARY KEY     NOT NULL, \
        SEASON_NAME         TEXT, \
        SEASON_DESCRIPTION  TEXT, \
        START_DAY           INT, \
        END_DAY             INT \
        )"


/** RESULT  : loads data from the seasons table into the seasons data array

    RETURNS : void

    PURPOSE : retrieve season data from permanent storage

    NOTES   :
**/
void load_db_seasons();


/** RESULT  : adds season data to the season data table

    RETURNS : void

    PURPOSE : a test function to load season data to the season data table

    NOTES   : to eventually be outsourced to a separate utility
**/

void add_db_season(int season_id, char *season_name, char *season_description, int start_day, int end_day);

#endif // DB_SEASON_TBL_H_INCLUDED
