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

#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

#define MAX_MAPS 10

#define TILE_MAP_MAX 50000
#define HEIGHT_MAP_MAX 150000
#define TWOD_OBJECT_MAP_MAX 800000
#define THREED_OBJECT_MAP_MAX 800000

//#define START_MAP_ID 1       // map_id of the map on which characters are created
//#define START_MAP_TILE 27225 // tile_pos at which characters are created

#define MIN_TRAVERSABLE_VALUE 1 //lowest value on height map that is traversable

#define MIN_MAP_AXIS 10 //used to bounds check maps

#define ELM_FILE_HEADER_LEN 124// remove after new map load functions are completed

#define ELM_FILE_HEADER_LENGTH 124

#define ELM_FILE_VERSION 1
#define MAX_ELM_FILE_SIZE 500000

#define CLIENT_MAP_PATH "./maps/" //the path that needs to be sent with the send_map packet

#include "clients.h"

enum{//tile bounds returned from check_tile_bounds function
    TILE_OUTSIDE_BOUNDS=-2,
    TILE_NON_TRAVERSABLE=-1,
    TILE_TRAVERSABLE=0
};

enum {// return values for is_map_tile_occupied
    TILE_OCCUPIED,
    TILE_UNOCCUPIED
};

enum { //return values from add_char_to_map / remove_char_from_map
    LEGAL_MAP=0,
    ILLEGAL_MAP=-1
};

struct map_node_type{

    char map_name[1024];    // eg Isla Prima
    char elm_filename[1024];// eg startmap.elm

    int map_axis;

    unsigned char tile_map[TILE_MAP_MAX];
    int tile_map_size;

    unsigned char height_map[HEIGHT_MAP_MAX];
    int height_map_size;

    unsigned char threed_object_map[THREED_OBJECT_MAP_MAX];
    int threed_object_map_size;
    int threed_object_structure_len;
    int threed_object_count;

    unsigned char twod_object_map[TWOD_OBJECT_MAP_MAX];
    int twod_object_map_size;
    int twod_object_structure_len;
    int twod_object_count;

    //unsigned char lights_object_map[LIGHTS_OBJECT_MAP_MAX];
    int lights_object_map_size;
    int lights_object_structure_len;
    int lights_object_count;
};

struct map_list_type {
    int count;
    struct map_node_type map[MAX_MAPS];
};
struct map_list_type maps;


/** RESULT  : calculates the distance between two entities on a map

    RETURNS : the tile distance

    PURPOSE : required by broadcast_add_new_enhanced_actor_packet

    NOTES   :
*/
int get_proximity(int tile_pos_1, int tile_pos_2, int map_axis);


#endif // MAPS_H_INCLUDED
