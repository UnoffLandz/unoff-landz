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


#ifndef MOVEMENT_H_INCLUDED
#define MOVEMENT_H_INCLUDED

#define MAX_UNOCCUPIED_TILE_SEARCH 10

struct vector_type{
    int x;
    int y;
    unsigned char move_cmd;
};
extern struct vector_type vector[8];


/** RESULT  : loads movement vector array with values

    RETURNS : void

    PURPOSE : initialise the vector array

*/
void initialise_movement_vectors();


/** RESULT  : finds nearest unoccupied tile

    RETURNS : nearest unoccupied tile or 0 if there is no unoccupied tiles within MAX_UNOCCUPIED_TILE_SEARCH

    PURPOSE : To support map jumps

*/
int get_nearest_unoccupied_tile(int map_id, int map_tile);

#endif // MOVEMENT_H_INCLUDED
