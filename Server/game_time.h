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

#ifndef GAME_TIME_H_INCLUDED
#define GAME_TIME_H_INCLUDED

#define GAME_DAY_MINUTES 360 //number of game minutes in each game day
#define GAME_MINUTE_INTERVAL 30 //number of real seconds in each game minute

/** RESULT   : updates the game time and database

    RETURNS  : void

    PURPOSE  : reduces code size in main.c

    NOTES    :
**/
void update_game_time();



#endif // GAME_TIME_H_INCLUDED
