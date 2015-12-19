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

#include <stdio.h> //support for sprintf function
#include <stdlib.h> //support for exit function
#include <string.h> // support for memset

#include "pathfinding.h"
#include "client_protocol.h"
#include "maps.h"
#include "logging.h"
#include "movement.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "server_start_stop.h"

#define DEBUG_PATHFINDING 0

struct{

    int tile;
    int value;
    bool explored;

}explore_stack[HEIGHT_MAP_MAX];

int explore_stack_count;

struct{

    int tile;
    int value;
    bool explored;

}path_stack[HEIGHT_MAP_MAX];

int path_stack_count;


void debug_explore_path(int actor_node, int destination){


    /** RESULT  : displays an ascii grid representation of the explored map tiles

        RETURNS : void

        PURPOSE : debugging maps

        USAGE   : function - hash_trace_explore
    */

    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    //create duplicate height map so we can fill it indicators for the explored tiles
    unsigned char height_map[HEIGHT_MAP_MAX]={0};
    memcpy(height_map, maps.map[map_id].height_map, sizeof(maps.map[map_id].height_map));

    //append the explore path to the height map
    for(int i=0; i<explore_stack_count; i++){

        int z=explore_stack[i].tile;
        height_map[z]=99;
    }

    //print the height map
    for(int x=pos_x-20; x<pos_x+20; x++){

        char str[81]="";

        for(int y=pos_y-40; y<pos_y+40; y++){

            int z=(y * map_axis) + x;

            if(z==clients.client[actor_node].map_tile){

                #if DEBUG_PATHFINDING==1
                printf("@");
                #endif // DEBUG_PATHFINDING

                strcat(str, "@");
            }

            else if(z==destination){

                #if DEBUG_PATHFINDING==1
                printf("+");
                #endif // DEBUG_PATHFINDING

                strcat(str, "+");
            }
            else {

                if(y>=0 && y<map_axis && x>=0 && x<map_axis){

                    if(height_map[z]==NON_TRAVERSABLE_TILE){

                        #if DEBUG_PATHFINDING==1
                        printf("#");
                        #endif // DEBUG_PATHFINDING

                        strcat(str, "#");
                    }
                    else if (height_map[z]!=99){

                        #if DEBUG_PATHFINDING==1
                        printf(".");
                        #endif // DEBUG_PATHFINDING

                        strcat(str, ".");
                    }
                    else{

                        if(height_map[z]==destination){

                            #if DEBUG_PATHFINDING==1
                            printf("*");
                            #endif // DEBUG_PATHFINDING

                            strcat(str, "*");
                        }
                        else {

                            #if DEBUG_PATHFINDING==1
                            printf("X");
                            #endif // DEBUG_PATHFINDING

                            strcat(str, "X");
                        }
                    }
                }
                else{

                    #if DEBUG_PATHFINDING==1
                    printf(":");
                    #endif // DEBUG_PATHFINDING

                    strcat(str, ":");
                }
            }
        }

        send_text(clients.client[actor_node].socket, CHAT_SERVER, "%c%s", c_grey1+127, str);

        #if DEBUG_PATHFINDING==1
        printf("\n");
        #endif // DEBUG_PATHFINDING
    }
}


void add_tile_to_explore_stack(int new_value, int new_tile){

    /** RESULT  : adds a unique tile to the a-star stack

        RETURNS : void

        PURPOSE : add's tiles to the a-star stack

        USAGE   : pathfinding.c add_adjacent_tiles_to_path_stack explore_path
    */

    //parse the stack
    for(int i=0; i<explore_stack_count; i++){

        //if we already have that tile in the stack then exit function without adding tile to stack
        if(explore_stack[i].tile==new_tile) return;

        if(explore_stack[i].value>new_value){

            //insert the new tile is lower than others on the stack, insert the new tile in
            //order of its value
            for(int j=explore_stack_count+1; j>i; j--){

                explore_stack[j]=explore_stack[j-1];
            }

            explore_stack[i].value=new_value;
            explore_stack[i].tile=new_tile;
            explore_stack[i].explored=false;

            explore_stack_count++;

            #if DEBUG_PATHFINDING==1
            printf("add tile %i to explore stack %i\n", new_tile, explore_stack_count);
            #endif

            //catch if explore stack array is exceeded
            if(explore_stack_count>HEIGHT_MAP_MAX-1){

                log_event(EVENT_ERROR, "explore stack array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                stop_server();
            }

            return;
        }
    }

    //if the new tile value is greater than any existing tile, add the new tile to the
    //end of the stack
    explore_stack[explore_stack_count].value=new_value;
    explore_stack[explore_stack_count].tile=new_tile;
    explore_stack[explore_stack_count].explored=false;

    explore_stack_count++;

    //catch if explore stack array is exceeded
    if(explore_stack_count>HEIGHT_MAP_MAX-1){

        log_event(EVENT_ERROR, "explore stack array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }
}


int get_heuristic_value(int tile, int dest_tile, int map_id){

    /** RESULT  : gets a heuristic value for a tile based on distance from the destination

        RETURNS : heuristic value of a tile for pathfinding purposes

        PURPOSE : to enable a-star path to determine tiles to be explored

        NOTES   : pathfinding.c add_adjacent_tiles_to_path explore_path
    */

    int map_axis=maps.map[map_id].map_axis;

    int tile_x=tile % map_axis;
    int tile_y=tile / map_axis;

    int dest_tile_x=dest_tile % map_axis;
    int dest_tile_y=dest_tile / map_axis;

    return abs(tile_x-dest_tile_x)+abs(tile_y-dest_tile_y);
}


bool tile_adjacent(int tile, int test_tile, int map_id){

    /** RESULT  : indicates if one tile is adjacent to another

        RETURNS : true/false

        PURPOSE : to enable a-star path to find a path from the list of explored tiles

        NOTES   : pathfinding.c get_astar_path
    */

    int map_axis=maps.map[map_id].map_axis;

    for(int i=0; i<MAX_VECTORS; i++){

        int adj_tile=tile+vector[i].x + (map_axis*vector[i].y);

        if(tile_in_lateral_bounds(tile, adj_tile, map_id)==true){

            if(adj_tile==test_tile) return true;
        }
    }

    return false;
}


void add_adjacent_tiles_to_explore_stack(int target_tile, int dest_tile, int map_id){

    /** RESULT  : adds all tiles adjacent to the target tile to the a-star stack, together with a heuristic value

        RETURNS : void

        PURPOSE : to provide a single function to add adjacent tiles to the a-star stack

        NOTES   : pathfinding.c explore_path

    */

    int map_axis=maps.map[map_id].map_axis;

    for(int i=0; i<MAX_VECTORS; i++){

        int adj_tile=target_tile + vector[i].x + (map_axis * vector[i].y);

        //ensure adjacent tile is within lateral bounds
        if(tile_in_lateral_bounds(target_tile, adj_tile, map_id)==true){

            //ensure adjacent tile is traversable
            if(maps.map[map_id].height_map[adj_tile]!=NON_TRAVERSABLE_TILE) {

                //calculate heuristic distance from adjacent tile to destination
                int heuristic_value=get_heuristic_value(adj_tile, dest_tile, map_id);

                add_tile_to_explore_stack(heuristic_value, adj_tile);
            }
        }
    }
}


int get_next_unexplored_tile(){

    /** RESULT  : finds next unexplored tile in the explore_stack

        RETURNS : the vector of the next unexplored tile in the explore stack or -1

        PURPOSE : code readability

        NOTES   : pathfinding.c explore_path

    */

    for(int i=0; i<explore_stack_count; i++){

        if(explore_stack[i].explored==false){

            explore_stack[i].explored=true;
            return i;
        }
    }

    return -1;
}


bool explore_path(int actor_node, int destination_tile){

    /** RESULT  : fills array path_stack with a list of tiles explored between start and destination

        RETURNS : true if path was explored from start to destination, otherwise false

        PURPOSE : to provide a list of explored tiles from which a path can be determined

        NOTES   : pathfinding.c get_astar_path
    */

    int start_tile=clients.client[actor_node].map_tile;
    int map_id=clients.client[actor_node].map_id;

    #if DEBUG_PATHFINDING==1
    printf("explore path from %i to %i\n", start_tile, destination_tile);
    #endif

    //filter out paths where start = destination
    if(start_tile==destination_tile) {

        #if DEBUG_PATHFINDING==1
        printf("cancel explore as start is the same as destination\n");
        #endif

        return false;
    }

    //add first tile to the explore stack
    int node=0;

    explore_stack[node].tile=start_tile;
    explore_stack[node].value=get_heuristic_value(start_tile, destination_tile, map_id);
    explore_stack[node].explored=true;

    #if DEBUG_PATHFINDING==1
    printf("cancel explore as start is the same as destination\n");
    #endif

    explore_stack_count=1;

    //explore from start to destination
    do{

        add_adjacent_tiles_to_explore_stack(explore_stack[node].tile, destination_tile, map_id);

        //get next unexplored tile
        node=get_next_unexplored_tile();

        if(node==-1 ) {

            send_text(clients.client[actor_node].socket, CHAT_PERSONAL, "%cthat destination is unreachable", c_red1+127);
            log_event(EVENT_ERROR, "no explorable tiles remain in function %s: module %s: line %i", __func__, __FILE__, __LINE__);

            return false;
        }

        #if DEBUG_PATHFINDING==1
        printf("get unexplored tile %i\n", explore_stack[node].tile);
        #endif

    } while(explore_stack[node].tile!=destination_tile);

    #if DEBUG_PATHFINDING==1
    printf("found destination %i\n", explore_stack[node].tile);
    #endif

    //transfer explore stack into the path stack
    for(int i=0; i<explore_stack_count; i++) {

        if(explore_stack[i].explored==true){

            path_stack[path_stack_count].tile=explore_stack[i].tile;
            path_stack[path_stack_count].explored=false;

             //reverse the heuristic value so we get distance from tile to start tile
            path_stack[path_stack_count].value=get_heuristic_value(path_stack[path_stack_count].tile, start_tile, map_id);

            path_stack_count++;

            //catch if explore stack array is exceeded
            if(path_stack_count==HEIGHT_MAP_MAX-1){

                log_event(EVENT_ERROR, "path stack array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                stop_server();
            }
        }
    }

    return true;
}


bool get_astar_path(int actor_node, int start_tile, int destination_tile){

    /** public function - see header */

    int j=0;

    path_stack_count=0;

    int lowest_value=0;
    int next_tile=0;
    int map_id=clients.client[actor_node].map_id;

    if(explore_path(actor_node, destination_tile)==false) return false;

    //if #EXPLORE_TRACE command has been set then send ascii representation of the
    //local area to the client
    if(clients.client[actor_node].debug_explore_path==true){

        debug_explore_path(actor_node, destination_tile);
        clients.client[actor_node].debug_explore_path=false;
    }

    //start path at destination tile
    next_tile=destination_tile;
    path_stack[0].explored=true;

    //load destination tile to the path
    clients.client[actor_node].path_count=1;
    clients.client[actor_node].path[ clients.client[actor_node].path_count-1]=next_tile;

    //loop through explored tiles finding the best adjacent moves from destination to start
    do{
        lowest_value=9999;//works for paths up to 9999 tiles long which ought to be sufficient
        bool found=false;

        for(int i=0; i<path_stack_count; i++){

            if(tile_adjacent(next_tile, path_stack[i].tile, map_id)==true){

                if(path_stack[i].value<lowest_value && path_stack[i].explored==false){

                    //ensure path doesn't cross lateral bounds
                    if(tile_in_lateral_bounds(next_tile, path_stack[i].tile, map_id)==true){

                        lowest_value=path_stack[i].value;
                        j=i;
                        found=true;
                    }
                }
            }
        }

        //if no adjacent tiles then start is unreachable from destination so abort function
        if(found==false) {

            send_text(clients.client[actor_node].socket, CHAT_PERSONAL, "%cthat destination is unreachable", c_red1+127);
            return false;
        }

        next_tile=path_stack[j].tile;
        path_stack[j].explored=true;

        clients.client[actor_node].path_count++;

        if(clients.client[actor_node].path_count>PATH_MAX-1) {

            log_event(EVENT_ERROR, "client path array exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        clients.client[actor_node].path[ clients.client[actor_node].path_count-1]=next_tile;

    }while(clients.client[actor_node].path[clients.client[actor_node].path_count-1]!=start_tile);

    return true;
}
