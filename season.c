/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

#include <string.h> //support for strcpy
#include <stdio.h> //support for sprintf

#include "season.h"
#include "game_data.h"
#include "logging.h"
#include "server_start_stop.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"

void get_game_season(int game_days, char *season_name, char *season_description){

    int season_days=game_days % game_data.year_length;

    int i=0;
    for(i=0; i<MAX_SEASONS; i++){

        //printf("%i %i\n", season[i].start_day, season[i].end_day );

        if(season_days>=season[i].start_day && season_days<season[i].end_day){

            strcpy(season_name, season[i].season_name);
            strcpy(season_description, season[i].season_description);
            return;
        }
    }

    log_event(EVENT_ERROR, "invalid season in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    stop_server();
}

void send_verbose_date(int connection, int game_days){

    char text_out[160]="";

    int game_year=game_data.game_days / 360;
    int year_days=game_data.game_days % 360;

    char season_name[80]="";
    char season_description[160]="";
    get_game_season(year_days, season_name, season_description);

    sprintf(text_out, "The day is %02i in the season of %s, year %i",  game_days, season_name, game_year);
    send_raw_text(connection, CHAT_SERVER, text_out);
}
