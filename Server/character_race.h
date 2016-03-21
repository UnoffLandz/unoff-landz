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


#ifndef CHARACTER_RACES_H_INCLUDED
#define CHARACTER_RACES_H_INCLUDED

#include <stdbool.h>

#define MAX_RACES 7
#define RACE_FILE "race.lst"

struct race_type{

    char race_name[20];
    char race_description[160];
    int char_count;
};

struct race_list_type {

    bool data_loaded;
    struct race_type race[MAX_RACES];
};
extern struct race_list_type races;


/** RESULT  : gets the character race

    RETURNS : the character race id

    PURPOSE :

    NOTES   :
**/
int get_char_race_id(int actor_node);


#endif // CHARACTER_RACES_H_INCLUDED
