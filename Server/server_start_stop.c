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

#include <stdio.h> //supports printf function
#include <stdlib.h> //supports exit function and EXIT_FAILURE definition

#include "date_time_functions.h"
#include "logging.h"

void stop_server(){

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    time_t stop_time=time(NULL);

    get_time_stamp_str(stop_time, time_stamp_str);
    get_verbose_date_str(stop_time, verbose_date_stamp_str);

    printf("SERVER STOP at %s on %s\n", time_stamp_str, verbose_date_stamp_str);
    printf("check 'error.log' for details\n");

    log_event(EVENT_ERROR, "SERVER STOP at %s on %s\n", time_stamp_str, verbose_date_stamp_str);

    exit(EXIT_FAILURE);
}

