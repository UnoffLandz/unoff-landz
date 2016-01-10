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

#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

#include <stdint.h> //supports int32_t datatype

#include "clients.h"

#define MAX_MAPS 50
#define ELM_FILE_HEADER 124
#define ELM_FILE_VERSION 1
#define MAX_ELM_FILE_SIZE 1000000
#define TILE_MAP_MAX 50000
#define HEIGHT_MAP_MAX 150000
#define NON_TRAVERSABLE_TILE 0
#define STEP_TILE_RATIO 6
#define CLIENT_MAP_PATH "./maps/" //the path that needs to be sent with the send_map packet

#define MAX_E3D_FILENAME 80
#define MAX_MAP_OBJECTS 2000

//#define START_MAP_ID 1       // map_id of the map on which characters are created
//#define START_MAP_TILE 27225 // tile_pos at which characters are created


struct map_node_type{

    char map_name[80];    // eg Isla Prima
    char description[160]; // eg land of the trolls
    char elm_filename[80];// eg startmap.elm
    char author[80];
    char author_email[80];
    int upload_date;
    int map_axis;
    enum {DEVELOPMENT, TESTING, FINAL}development_status;

    unsigned char tile_map[TILE_MAP_MAX];
    size_t tile_map_size;

    unsigned char height_map[HEIGHT_MAP_MAX];
    size_t height_map_size;

    int threed_object_map_size;
    int threed_object_structure_len;
    int threed_object_count;
    int threed_object_offset;

    int twod_object_map_size;
    int twod_object_structure_len;
    int twod_object_count;
    int twod_object_offset;

    int lights_object_map_size;
    int lights_object_structure_len;
    int lights_object_count;
};

struct map_list_type {

    struct map_node_type map[MAX_MAPS];
};
extern struct map_list_type maps;

struct __attribute__((__packed__)){

    unsigned char magic_number[4];
    int32_t h_tiles;
    int32_t v_tiles;
    int32_t tile_map_offset;
    int32_t height_map_offset;

    int32_t threed_object_hash_len;
    int32_t threed_object_count;
    int32_t threed_object_offset;

    int32_t twod_object_hash_len;
    int32_t twod_object_count;
    int32_t twod_object_offset;

    int32_t lights_object_hash_len;
    int32_t lights_object_count;
    int32_t lights_object_offset;

    unsigned char dungeon_flag;
    unsigned char version_flag;
    unsigned char reserved1;
    unsigned char reserved2;

    int32_t ambient_red;
    int32_t ambient_green;
    int32_t ambient_blue;

    int32_t particles_object_hash_len;
    int32_t particles_object_count;
    int32_t particles_object_offset;

    int32_t clusters_offset;

    int32_t reserved_9;
    int32_t reserved_10;
    int32_t reserved_11;
    int32_t reserved_12;
    int32_t reserved_13;
    int32_t reserved_14;
    int32_t reserved_15;
    int32_t reserved_16;
    int32_t reserved_17;
} elm_header;


/** RESULT  : calculates the distance between two entities on a map

    RETURNS : the tile distance

    PURPOSE : required by broadcast_add_new_enhanced_actor_packet

    NOTES   :
*/
int get_proximity(int tile_pos_1, int tile_pos_2, int map_id);


/** RESULT  : calculates the tile based on the x,y coordinates

    RETURNS : the tile number

    PURPOSE :

    NOTES   :
*/
int get_tile(int x_pos, int y_pos, int map_axis);


/** RESULT  : calculates the x coordinate of the map tile

    RETURNS : the x coordinate

    PURPOSE :

    NOTES   :
*/
int get_x_pos(int tile, int map_id);


/** RESULT  : calculates the y coordinate of the map tile

    RETURNS : the y coordinate

    PURPOSE :

    NOTES   :
*/
int get_y_pos(int tile, int map_id);


/** RESULT  : finds the map id for a map name

    RETURNS : the map id

    PURPOSE : used in function:

    NOTES   :
*/
int get_map_id(char *map_name);


/** RESULT  : sends basic map details to client

    RETURNS : void

    PURPOSE : used in functions: hash_where_am_i, get_map_developer_details

    NOTES   :
*/
void get_map_details(int actor_node, int map_id);


/** RESULT  : sends map developer details to client

    RETURNS : void

    PURPOSE : used in functions: hash_map

    NOTES   :
*/
void get_map_developer_details(int actor_node, int map_id);


/** RESULT  : reads an elm file header into the global elm_header struct so that it can
              then be processed by other functions

    RETURNS : void

    PURPOSE : used in functions: read_height_map

    NOTES   :
*/
void read_elm_header(char *elm_filename);


/** RESULT  : reads the height map data from an elm file header into an array so that
              it can then be processed by other functions

    RETURNS : void

    PURPOSE : used in functions: load_db_maps

    NOTES   :
*/
void read_height_map(char *elm_filename, unsigned char *height_map);


/** RESULT  : reads the tile map data from an elm file header into an array so that
              it can then be processed by other functions

    RETURNS : void

    PURPOSE : used in functions: load_db_maps

    NOTES   :
*/
void read_tile_map(char *elm_filename, unsigned char *tile_map);


/** RESULT  : determines in a map node has a map

    RETURNS : void

    PURPOSE : used in function hash_jump

    NOTES   :
*/
bool map_exists(int map_id);

#endif // MAPS_H_INCLUDED
