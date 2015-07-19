/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

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

#ifndef MAP_OBJECT_H_INCLUDED
#define MAP_OBJECT_H_INCLUDED

#include <stdbool.h> // support for bool data type

#include "maps.h" // required for MAX_MAPS

#define MAX_MAP_OBJECTS 10000

struct map_object_type{

    int threedol_id;
    int tile;
    int e3d_id;
    bool harvestable;
    int reserve;
};
extern struct map_object_type map_object[MAX_MAP_OBJECTS][MAX_MAPS];

/** RESULT  : finds the object id based on the position of an entry in a map threed object list

    RETURNS : the object id

    PURPOSE : used to process the result from LOOK_AT_MAP_OBJECT and HARVEST protocols

    NOTES   :
*/
int get_object_id(int map_id, int threed_object_list_pos);


/** RESULT  : calculates the object tile based on the position of an entry in a map threed object list

    RETURNS : the tile number of the object position

    PURPOSE : used to process the result from LOOK_AT_MAP_OBJECT and HARVEST protocols

    NOTES   :
*/
int get_object_tile(int map_id, int threed_object_list_pos);

#endif // MAP_OBJECT_H_INCLUDED
