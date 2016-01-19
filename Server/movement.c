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

#include <string.h> //support for memcpy function

#include "movement.h"
#include "maps.h"
#include "logging.h"
#include "characters.h"

struct vector_type vector[] = {

//   Vector  Enum
//  -------  ----------------
    {0, 1,   actor_cmd_move_n},
    {1, 1,   actor_cmd_move_ne},
    {1, 0,   actor_cmd_move_e},
    {1, -1,  actor_cmd_move_se},
    {0, -1,  actor_cmd_move_s},
    {-1, -1, actor_cmd_move_sw},
    {-1, 0,  actor_cmd_move_w},
    {-1, 1,  actor_cmd_move_nw}
};


bool tile_walkable(int map_id, int tile){

    /** RESULT  : determines if a tile is walkable

        RETURNS : true/false

        PURPOSE : used by function: get_nearest_unoccupied_tile

        NOTES   :
    */

    //check the height map and see if tile is walkable
    if(maps.map[map_id].height_map[tile]==NON_TRAVERSABLE_TILE) return false;

    return true;
}


bool tile_unoccupied(int map_id, int map_tile){

    /** RESULT  : determines if tile is unoccupied

        RETURNS : true/false

        PURPOSE : used by get_nearest_unoccupied_tile

        NOTES   :
    */

    //now check through clients and see if any have characters occupying that tile
    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].map_tile==map_tile && clients.client[i].map_id==map_id) return false;
    }

    return true;
}


bool tile_in_lateral_bounds(int tile, int next_tile, int map_id){

    /**  RESULT  : indicates if moving from one tile to the next will cross lateral bounds

        RETURNS : TRUE / FALSE

        PURPOSE : to prevent path crossing lateral bounds

        USAGE   : pathfinding.c get_astar_path / add_ajacent_tiles
    */

    int map_axis=maps.map[map_id].map_axis;
    int tile_x=tile % map_axis;
    int next_tile_x=next_tile % map_axis;
    int vector_x=next_tile_x-tile_x;

    if(tile_x==0 && vector_x==1) return false;
    if(tile_x==1 && vector_x==-1) return false;

    return true;
}


int get_nearest_unoccupied_tile(int map_id, int map_tile){

    /** public function - see header */

    int map_axis=maps.map[map_id].map_axis;

    unsigned char height_map[HEIGHT_MAP_MAX]={0};
    memcpy(height_map, maps.map[map_id].height_map, (size_t)maps.map[map_id].height_map_size);

    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].map_id==map_id){

            height_map[clients.client[i].map_tile]=NON_TRAVERSABLE_TILE;
        }
    }

    //check target tile to see if it is walkable and unoccupied
    if(height_map[map_tile]!=NON_TRAVERSABLE_TILE) return map_tile;

    //if target tile is not walkable and unoccupied then search adjacent tiles
    int start=0, finish=0, inc=0, last_tile=0;

    for(int i=0; i<MAX_UNOCCUPIED_TILE_SEARCH; i++){

        for(int j=0; j<4; j++){

            switch(j){

                case 0:{
                    start=map_tile+(map_axis*i)-i;
                    finish=map_tile+(map_axis*i)+i;
                    inc=1;
                    break;
                }

                case 1:{
                    start=finish=map_tile+(map_axis*i)+i;
                    finish=map_tile-(map_axis*i)+i;
                    inc=-map_axis;
                    break;
                }

                case 2:{
                    start=map_tile-(map_axis*i)+i;
                    finish=map_tile-(map_axis*i)-i;
                    inc=-1;
                    break;
                }

                case 3:{
                    start=map_tile-(map_axis*i)-i;
                    finish=map_tile+(map_axis*i)-i;
                    inc=map_axis;
                }
            }

            for(int j=start; j<=finish; j+=inc){

                //make sure we don't cross horizontal bounds
                if(tile_in_lateral_bounds(last_tile, j, map_id)==false) break;

                //make sure we don't cross vertical bounds
                if(j>(int)maps.map[map_id].height_map_size || j<0) break;

                //check that tile is unoccupied and walkable
                if(height_map[j]!=NON_TRAVERSABLE_TILE) return j;

                last_tile=j;
            }
        }
    }

    return -1;
}
