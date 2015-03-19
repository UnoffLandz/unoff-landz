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

#include "maps.h"
#include "movement.h"
#include "logging.h"
#include "characters.h"

void initialise_movement_vectors(){

    vector[0].x=0; vector[0].y=1; vector[0].move_cmd=actor_cmd_move_n;
    vector[1].x=1; vector[1].y=1; vector[1].move_cmd=actor_cmd_move_ne;
    vector[2].x=1; vector[2].y=0; vector[2].move_cmd=actor_cmd_move_e;
    vector[3].x=1; vector[3].y=-1; vector[3].move_cmd=actor_cmd_move_se;
    vector[4].x=0; vector[4].y=-1; vector[4].move_cmd=actor_cmd_move_s;
    vector[5].x=-1; vector[5].y=-1; vector[5].move_cmd=actor_cmd_move_sw;
    vector[6].x=-1; vector[6].y=0; vector[6].move_cmd=actor_cmd_move_w;
    vector[7].x=-1; vector[7].y=1; vector[7].move_cmd=actor_cmd_move_nw;
}

int is_map_tile_occupied(int map_id, int map_tile){

    /** RESULT  : finds nearest unoccupied tile

        RETURNS : MAP_TILE_UNOCCUPIED / MAP_TILE_OCCUPIED /MAP_TILE_UNTRAVERSABLE

        PURPOSE : used by get_nearest_unoccupied_tile

        NOTES   :
    */

    // first check the height map and see if tile is walkable
    if(maps.map[map_id].height_map[map_tile]<MIN_TRAVERSABLE_VALUE) return TILE_NON_TRAVERSABLE;

    //now check through clients and see if any have characters occupying that tile
    int i=0;
    for(i=0; i<clients.client_count; i++){

        if(clients.client[i].map_tile==map_tile && clients.client[i].map_id==map_id) return TILE_OCCUPIED;
    }

    return TILE_UNOCCUPIED;
}

int get_nearest_unoccupied_tile(int map_id, int map_tile){

    /** public function - see header */

    int i=0, j=0;
    int map_axis=maps.map[map_id].map_axis;

    if(is_map_tile_occupied(map_id, map_tile)==TILE_UNOCCUPIED) return map_tile;

    do{

        //examine all adjacent tiles
        for(i=0; i<8; i++){

            int next_tile=map_tile+vector[i].x+(vector[i].y * map_axis * j);

            // keep with bounds of map
            if(next_tile>0 && next_tile<maps.map[map_id].height_map_size) {

                //check next best tile
                if(is_map_tile_occupied(map_id, next_tile)==TILE_UNOCCUPIED) return next_tile;
            }
        }

        //widen search of unoccupied tile
        j++;

    } while(j<MAX_UNOCCUPIED_TILE_SEARCH);

    return 0; //return 0 if an unoccupied tile cannot be found
}
