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

#ifndef SEASON_H_INCLUDED
#define SEASON_H_INCLUDED

#define MAX_SEASONS 4
#define SEASON_FILE "season.lst"

#include <stdbool.h> //supports bool datatype

struct season_type{

    int season_id;
    char season_name[80];
    char season_description[160];
    int start_day;
    int end_day;
};

struct season_list_type {

    bool data_loaded;
    struct season_type season[MAX_SEASONS];
};
extern struct season_list_type seasons;


/** RESULT  : gets the season name and description based on the number of game days

    RETURNS : void

    PURPOSE : code modularity

    NOTES   : used by send_verbose_date
**/
void get_game_season(int game_days, char *season_name, char *season_description);

#endif // SEASON_H_INCLUDED
