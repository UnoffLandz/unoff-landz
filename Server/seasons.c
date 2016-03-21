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

#include <string.h> //support for strcpy function

#include "season.h"
#include "game_data.h"
#include "logging.h"
#include "server_start_stop.h"

struct season_list_type seasons;


void get_game_season(int game_days, char *season_name, char *season_description){

    /** public function - see header */

   int season_days=game_days % game_data.year_length;

    for(int i=0; i<MAX_SEASONS; i++){

        if(season_days>=seasons.season[i].start_day && season_days<seasons.season[i].end_day){

            strcpy(season_name, seasons.season[i].season_name);
            strcpy(season_description, seasons.season[i].season_description);
            return;
        }
    }

    log_event(EVENT_ERROR, "cannot find season in function %s: module %s: line %i", GET_CALL_INFO);
    stop_server();
}


