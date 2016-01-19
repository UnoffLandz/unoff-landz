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

#ifndef DB_GAME_DATA_TBL_H_INCLUDED
#define DB_GAME_DATA_TBL_H_INCLUDED

#define GAME_DATA_TABLE_SQL "CREATE TABLE GAME_DATA_TABLE( \
        GAME_DATA_ID        INTEGER PRIMARY KEY     NOT NULL, \
        BEAM_MAP_ID         INT, \
        BEAM_MAP_TILE       INT, \
        START_MAP_ID        INT, \
        START_MAP_TILE      INT, \
        GAME_MINUTES        INT, \
        GAME_DAYS           INT, \
        YEAR_LENGTH         INT, \
        DB_VERSION          INT  \
        )"


/** RESULT  : loads data from the game data table into the game data array

    RETURNS : void

    PURPOSE : retrieve guild data from permanent storage

    NOTES   :
**/
void load_db_game_data();


/** RESULT  : adds game data to the game data table

    RETURNS : void

    PURPOSE : a test function to load game data to the game data table

    NOTES   : to eventually be outsourced to a separate utility
**/
void add_db_game_data(int beam_map_id, int beam_map_tile, int start_map_id, int start_map_tile, int year_length, int db_version);

#endif // DB_GAME_DATA_TBL_H_INCLUDED
