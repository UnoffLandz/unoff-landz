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

#ifndef CHARACTER_GENDER_H_INCLUDED
#define CHARACTER_GENDER_H_INCLUDED

#define MAX_GENDERS 2
#define GENDER_FILE "gender.lst"

#include <stdbool.h> //supports bool datatype

struct gender_type{
    char gender_name[20];
    char char_count;
};

struct gender_list_type {

    bool data_loaded;
    struct gender_type gender[MAX_GENDERS];
};
extern struct gender_list_type genders;

/** RESULT  : gets the character gender

    RETURNS : the character gender id

    PURPOSE :

    NOTES   :
**/
int get_char_gender_id(int actor_node);

#endif // CHARACTER_GENDER_H_INCLUDED
