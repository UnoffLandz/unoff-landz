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

#include <stdio.h> //support for sprintf function
#include <stdlib.h> //support for exit function

#include "global.h"
#include "pathfinding.h"
#include "client_protocol.h"
#include "maps.h"
#include "logging.h"
#include "movement.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "server_start_stop.h"

int add_tile_to_explore_stack(int new_value, int new_tile, int *explore_stack_count, int explore_stack[][3]){

    /** RESULT  : adds a unique tile to the a-star stack

        RETURNS : DUPLICATE_TILE - tile not added as its already in the stack
                  NEW_TILE       - tile added as stack
                  ABORT          - explore stack exceeded

        PURPOSE : add's tiles to the a-star stack

        USAGE   : pathfinding.c add_adjacent_tiles_to_path_stack explore_path
    */

    int i=0, j=0;
    int insert_value=0;
    int insert_tile=0;
    int insert_status=0;
    int new_status=UNEXPLORED;

    //parse the stack
    for(i=0; i<*explore_stack_count; i++){

        //if we already have that tile in the stack then exit function without adding tile to stack
        if(explore_stack[i][TILE]==new_tile) return ADD_TILE_COMPLETE;

        //insert the new tile in order of its distance value
        if(explore_stack[i][VALUE]>new_value){

            //move the rest of the stack to make room for the new tile
            for(j=i; j<*explore_stack_count; j++){

                insert_value=explore_stack[j][VALUE];
                insert_tile=explore_stack[j][TILE];
                insert_status=explore_stack[j][STATUS];

                explore_stack[j][VALUE]=new_value;
                explore_stack[j][TILE]=new_tile;
                explore_stack[j][STATUS]=new_status;

                new_value=insert_value;
                new_tile=insert_tile;
                new_status=insert_status;
            }

            break;
        }
    }

    explore_stack[*explore_stack_count][VALUE]=new_value;
    explore_stack[*explore_stack_count][TILE]=new_tile;
    explore_stack[*explore_stack_count][STATUS]=new_status;

    *explore_stack_count=*explore_stack_count+1;

    //catch if explore stack array is exceeded
    if(*explore_stack_count>HEIGHT_MAP_MAX-1){

        //if stack array is exceeded then pass message to calling function to recover gracefully rather than crash
        log_event(EVENT_MOVE_ERROR, "explore stack array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    return ADD_TILE_COMPLETE;
}

int get_heuristic_value(int tile, int dest_tile, int map_id){

    /** RESULT  : gets a heuristic value for a tile based on distance from the destination

        RETURNS : heuristic value of a tile for pathfinding purposes

        PURPOSE : to enable a-star path to determine tiles to be explored

        USAGE   : pathfinding.c add_adjacent_tiles_to_path explore_path
    */

    int map_axis=maps.map[map_id].map_axis;
    int tile_x=tile % map_axis;
    int tile_y=tile / map_axis;
    int dest_tile_x=dest_tile % map_axis;
    int dest_tile_y=dest_tile / map_axis;

    return abs(tile_x-dest_tile_x)+abs(tile_y-dest_tile_y);
}

int is_tile_in_lateral_bounds(int tile, int next_tile, int map_id){

    /**  RESULT  : indicates if moving from tile to next tile will cross lateral bounds

        RETURNS : TRUE / FALSE

        PURPOSE : to prevent path crossing lateral bounds

        USAGE   : pathfinding.c get_astar_path / add_ajacent_tiles
    */

    int map_axis=maps.map[map_id].map_axis;
    int tile_x=tile % map_axis;
    int next_tile_x=next_tile % map_axis;
    int vector_x=next_tile_x-tile_x;

    if(tile_x==0 && vector_x==1) return FALSE;
    if(tile_x==1 && vector_x==-1) return FALSE;

    return TRUE;
}

int is_tile_adjacent(int tile, int test_tile, int map_id){

    /** RESULT  : indicates if one tile is adjacent to another

        RETURNS : TRUE / FALSE

        PURPOSE : to enable a-star path to find a path from the list of explored tiles

        USAGE   : pathfinding.c get_astar_path
    */

    int i=0;
    int adj_tile=0;
    int map_axis=maps.map[map_id].map_axis;

    for(i=0; i<8; i++){

        //adj_tile=tile+vector_x[i]+(map_axis*vector_y[i]);
        adj_tile=tile+vector[i].x+(map_axis*vector[i].y);

        if(adj_tile==test_tile && is_tile_in_lateral_bounds(tile, adj_tile, map_id)==TRUE) return TRUE;
    }

    return FALSE;
}

int add_adjacent_tiles_to_explore_stack(int target_tile, int dest_tile, int map_id, int *explore_stack_count, int explore_stack[][3]){

    /** RESULT  : adds all tiles adjacent to the target tile to the a-star stack, together with a heuristic value

        RETURNS : void

        PURPOSE : to provide a single function to add adjacent tiles to the a-star stack

        USAGE   : pathfinding.c explore_path
    */

    int i=0;
    int adj_tile=0;
    int map_axis=maps.map[map_id].map_axis;
    int heuristic_value=0;

    for(i=0; i<8; i++){

        //adj_tile=target_tile+vector_x[i]+(map_axis*vector_y[i]);
        adj_tile=target_tile+vector[i].x+(map_axis*vector[i].y);

        //ensure adjacent tile is within lateral bounds
        if(is_tile_in_lateral_bounds(target_tile, adj_tile, map_id)==TRUE){

            //ensure adjacent tile is traversable
            if(maps.map[map_id].height_map[adj_tile]>=MIN_TRAVERSABLE_VALUE) {

                //calculate heuristic distance from adjacent tile to destination
                heuristic_value=get_heuristic_value(adj_tile, dest_tile, map_id);

                if(add_tile_to_explore_stack(heuristic_value, adj_tile, explore_stack_count, explore_stack)==ADD_TILE_ABORT) {

                    //if something went wrong in the last function then abort this function
                    return ADD_TILE_ABORT;
                }

            }
        }
    }

    return ADD_TILE_COMPLETE;
}

int explore_path(int connection, int destination_tile, int *path_stack_count, int path_stack[][3]){

    /** RESULT  : fills array path_stack with a list of tiles explored between start and destination

        RETURNS : EXPLORE_ABORT       - the explore stack was exceeded
                  EXPLORE_UNREACHABLE - could not explore to destination
                  EXPLORE_REACHABLE   - explored to destination

        PURPOSE : to provide a list of explored tiles from which a path can be determined

        USAGE   : pathfinding.c get_astar_path
    */

    int i=0, j=0;
    int explore_stack[HEIGHT_MAP_MAX][3];
    int explore_stack_count=0;
    int node=0;
    int heuristic_value=0;
    int found=FALSE;
    char text_out[1024]="";
    int start_tile=clients.client[connection].map_tile;
    int map_id=clients.client[connection].map_id;

    //filter out paths where start = destination
    if(start_tile==destination_tile) return NOT_FOUND;

    //add start tile to stack
    heuristic_value=get_heuristic_value(start_tile, destination_tile, map_id);

    if(add_tile_to_explore_stack(heuristic_value, start_tile, &explore_stack_count, explore_stack)==ADD_TILE_ABORT) {

        //if something went wrong in sub function then pass message to calling function to recover gracefully rather than crash
        log_event(EVENT_MOVE_ERROR, "abort in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        return NOT_FOUND;
    }

    explore_stack[node][STATUS]=EXPLORED;
    explore_stack_count=1;

    //explore from start to destination
    do{

        if(add_adjacent_tiles_to_explore_stack(explore_stack[node][TILE], destination_tile, map_id, &explore_stack_count, explore_stack)==ADD_TILE_ABORT){

            log_event(EVENT_MOVE_ERROR, "abort in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            return NOT_FOUND;
        }

        //get next unexplored tile
        found=FALSE;

        for(i=0; i<explore_stack_count; i++){

            if(explore_stack[i][STATUS]==UNEXPLORED){
                explore_stack[i][STATUS]=EXPLORED;
                node=i;
                found=TRUE;
                break;
            }
        }

        if(found==FALSE) {

            sprintf(text_out, "%cthat destination is unreachable", c_red1+127);
            send_raw_text(connection, CHAT_PERSONAL, text_out);
            log_event(EVENT_MOVE_ERROR, "destination unreachable - no explorable tiles left in stack in function %s: module %s: line %i", __func__, __FILE__, __LINE__);

            return NOT_FOUND;
        }

    } while(explore_stack[node][TILE]!=destination_tile);

    //filter the list of explored tiles that will be used to create the path
    for(i=0; i<explore_stack_count; i++) {

        if(explore_stack[i][STATUS]==EXPLORED){

            path_stack[j][TILE]=explore_stack[i][TILE];
            path_stack[j][STATUS]=UNEXPLORED;

             //reverse the heuristic value so we get distance from tile to start tile
            path_stack[j][VALUE]=get_heuristic_value(path_stack[j][TILE], start_tile, map_id);

            j++;
        }
    }

    *path_stack_count=j;

    return FOUND;
}

int get_astar_path(int connection, int start_tile, int destination_tile){

    /** public function - see header */

    int i=0, j=0;

    int path_stack[PATH_MAX][3];
    int path_stack_count=0;

    int lowest_value=0;
    int next_tile=0;
    int map_id=clients.client[connection].map_id;
    char text_out[1024]="";
    int found=FALSE;

    if(explore_path(connection, destination_tile, &path_stack_count, path_stack)==NOT_FOUND) return NOT_FOUND;

    //start path at destination tile
    next_tile=destination_tile;
    path_stack[0][STATUS]=EXPLORED;

    //load destination tile to the path
    clients.client[connection].path_count=1;
    clients.client[connection].path[ clients.client[connection].path_count-1]=next_tile;

    //loop through explored tiles finding the best adjacent moves from destination to start
    do{
        lowest_value=9999;//works for paths up to 9999 tiles long which ought to be sufficient
        found=FALSE;

        for(i=0; i<path_stack_count; i++){

            if(is_tile_adjacent(next_tile, path_stack[i][TILE], map_id)==TRUE){

                if(path_stack[i][VALUE]<lowest_value && path_stack[i][STATUS]==UNEXPLORED){

                    //ensure path doesn't cross lateral bounds
                    if(is_tile_in_lateral_bounds(next_tile, path_stack[i][TILE], map_id)==TRUE){

                        lowest_value=path_stack[i][VALUE];
                        j=i;
                        found=TRUE;
                    }
                }
            }
        }

        //if no adjacent tiles then start is unreachable from destination so abort function
        if(found==FALSE) {

            sprintf(text_out, "%cthat destination is unreachable", c_red1+127);
            send_raw_text(connection, CHAT_PERSONAL, text_out);
            return NOT_FOUND;
        }

        next_tile=path_stack[j][TILE];
        path_stack[j][STATUS]=EXPLORED;

        clients.client[connection].path_count++;

        if(clients.client[connection].path_count>PATH_MAX-1) {

            log_event(EVENT_MOVE_ERROR, "client path array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        clients.client[connection].path[ clients.client[connection].path_count-1]=next_tile;

    }while(clients.client[connection].path[clients.client[connection].path_count-1]!=start_tile);

    return FOUND;
}
