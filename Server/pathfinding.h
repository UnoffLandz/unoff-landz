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

#ifndef PATHFINDING_H_INCLUDED
#define PATHFINDING_H_INCLUDED

#include <stdbool.h> // support for bool data type

/** RESULT  : fills client path array with an a-star path

    RETURNS : true - path was created, otherwise false

    PURPOSE : to provide a list of explored tiles from which a path can be determined

    USAGE   : pathfinding.c get_astar_path
*/
bool get_astar_path(int actor_node, int start_tile, int destination_tile);


/** RESULT  : displays an ascii grid indicating tiles covered by the explore_path function

    RETURNS : void

    PURPOSE : debugging

    NOTES   : used in function: hash_trace_explore

*/
void debug_explore_path(int actor_node, int destination);


#endif // PATHFINDING_H_INCLUDED
