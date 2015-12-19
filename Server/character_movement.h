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

#ifndef CHARACTER_MOVEMENT_H_INCLUDED
#define CHARACTER_MOVEMENT_H_INCLUDED

#define PATH_MAX 1000    // maximum tiles of precalculated character movement

/** RESULT  : Removes actor from map

    RETURNS : void

    PURPOSE : used at log-out to remove a char from map. Also on map jumps
**/
void remove_char_from_map(int actor_node);


/** RESULT  : adds a character to a map

    RETURNS : true/false

    PURPOSE : used at log-in to add a char to a map. Also by function move_char_between_maps
**/
bool add_char_to_map(int actor_node, int new_map_id, int map_tile);


/** RESULT  : moves a character between maps

    RETURNS : true/false

    PURPOSE : supports map jumps
**/
bool move_char_between_maps(int actor_node, int new_map_id, int new_map_tile);


/** RESULT  : moves a character one step along the path

    RETURNS : void

    PURPOSE : makes the character move
**/
void process_char_move(int actor_node, time_t current_utime);


/** RESULT  : starts the character moving

    RETURNS : void

    PURPOSE : makes the character move
**/
void start_char_move(int actor_node, int destination);

#endif // CHARACTER_MOVEMENT_H_INCLUDED
