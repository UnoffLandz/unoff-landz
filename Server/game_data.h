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

#ifndef GAME_DATA_H_INCLUDED
#define GAME_DATA_H_INCLUDED

#include <sys/time.h>   // supports time_t data type

struct game_data_type {

    time_t server_start_time;
    char name_last_char_created[80];
    time_t date_last_char_created;
    int char_count;
    int game_minutes;
    int game_days;
    int beam_map_id;
    int beam_map_tile;
    int start_map_id;
    int start_map_tile;
    int year_length;
};
extern struct game_data_type game_data;

#endif // GAME_DATA_H_INCLUDED
