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

#include <time.h> //support for time_t datatype
#include <string.h> //support for strcpy

#include "game_data.h"


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


