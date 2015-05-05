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

#ifndef CHARACTER_TYPE_H_INCLUDED
#define CHARACTER_TYPE_H_INCLUDED

#define MAX_CHARACTER_TYPES 42 //the highest character type code used by the client

struct character_type_type{
    int race_id;
    int gender_id;
    int char_count;
};
extern struct character_type_type character_type[MAX_CHARACTER_TYPES];

#endif // CHARACTER_TYPE_H_INCLUDED
