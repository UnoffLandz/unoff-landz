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
#include "date_time_functions.h"

#include <stdio.h> //support for sprintf
#include <time.h> //support for time_t datatype
#include <string.h> //support for strcpy

#include "game_data.h"
#include "season.h"
#include "logging.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_start_stop.h"
#include "server_protocol_functions.h"

struct timeval time_check;//output struct for gettimeofday function
struct season_type season[MAX_SEASONS];

void get_time_stamp_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_time;

    // Convert to struct in gmt representation.
    cooked_time = gmtime (&raw_time);

    strftime (buffer, 9, "%H:%M:%S", cooked_time);
}

void get_date_stamp_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 11, "%d:%m:%C%y", cooked_date);
}

void get_verbose_date_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 50, "%A %d %B %C%y", cooked_date);
}

void get_time_up_str(time_t raw_time, char *buffer){

    /** public function - see header */

    struct tm *cooked_date;

    // Convert to struct in gmt representation.
    cooked_date = gmtime (&raw_time);

    strftime (buffer, 15, "%H hrs %M mins", cooked_date);
}

void get_game_season(int game_days, char *season_name, char *season_description){

    /** RESULT  : gets the season name and description based on the number of game days

        RETURNS : void

        PURPOSE : code modularity

        NOTES   : used by send_verbose_date
    **/

    int season_days=game_days % game_data.year_length;

    int i=0;
    for(i=0; i<MAX_SEASONS; i++){

        if(season_days>=season[i].start_day && season_days<season[i].end_day){

            strcpy(season_name, season[i].season_name);
            strcpy(season_description, season[i].season_description);
            return;
        }
    }

    log_event(EVENT_ERROR, "invalid season in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    stop_server();
}

void send_verbose_date(int socket, int game_days){

    /** public function - see header */

    int game_year=game_data.game_days / 360;
    int year_days=game_data.game_days % 360;

    char season_name[80]="";
    char season_description[160]="";
    get_game_season(year_days, season_name, season_description);

    send_text(socket, CHAT_SERVER, "%cThe day is %02i in the season of %s, year %i", 127+c_purple1, game_days, season_name, game_year);
}

void send_verbose_time(int socket, int game_minutes){

    send_text(socket, CHAT_SERVER, "%cThe time is %02i:%02i",  127+c_purple1, game_minutes / 60, game_minutes % 60);
}
