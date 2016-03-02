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


#ifndef MOVEMENT_H_INCLUDED
#define MOVEMENT_H_INCLUDED

#include <stdbool.h> //support for bool datatype

#define MAX_UNOCCUPIED_TILE_SEARCH 10
#define MAX_VECTORS 8

struct vector_type{
    int x;
    int y;
    unsigned char move_cmd;
};
extern struct vector_type vector[8];


/** RESULT  : finds nearest unoccupied tile

    RETURNS : nearest unoccupied tile or -1 if there is no unoccupied tiles within MAX_UNOCCUPIED_TILE_SEARCH

    PURPOSE : To support map jumps

*/
int get_nearest_unoccupied_tile(int map_id, int map_tile);


/** RESULT  : indicates if moving from one tile to the next will cross lateral bounds

    RETURNS : true/false

    PURPOSE : to prevent path crossing lateral bounds

    USAGE   : functions get_astar_path, add_ajacent_tiles, get_nearest_unoccupied_tile
*/
bool tile_in_lateral_bounds(int tile, int next_tile, int map_id);

// TODO (themuntdregger#1#): document movement.h functions
bool tile_walkable(int map_id, int map_tile);
bool tile_unoccupied(int map_id, int map_tile);

/** RESULT  : broadcasts the enhanced_new_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character addition

    NOTES   :
*/
void broadcast_add_new_enhanced_actor_packet(int actor_node);


/** RESULT  : broadcasts the remove_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character removal

    NOTES   :
*/
void broadcast_remove_actor_packet(int actor_node);


/** RESULT  : broadcasts the actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character movement

    NOTES   :
*/
void broadcast_actor_packet(int actor_node, unsigned char move, int sender_destination_tile);


#endif // MOVEMENT_H_INCLUDED
