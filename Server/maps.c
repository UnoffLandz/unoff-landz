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
#include <stdlib.h> //supports abs function
#include <stdio.h> //supports printf (testing)
#include <string.h> //supports memcpy (testing)

#include "maps.h"
#include "logging.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "movement.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "global.h"
#include "broadcast_actor_functions.h"
#include "server_start_stop.h"
#include "e3d.h"
#include "colour.h"
#include "date_time_functions.h"

struct map_list_type maps;


int get_proximity(int tile_pos_1, int tile_pos_2, int map_axis){

    /** public function - see header */

    int x_diff=abs((tile_pos_1 % map_axis) - (tile_pos_2 % map_axis));
    int y_diff=abs((tile_pos_1 / map_axis) - (tile_pos_2 / map_axis));

    if(x_diff>y_diff) return x_diff; else return y_diff;
}


int get_tile(int x_pos, int y_pos, int map_id){

     /** public function - see header */

    return x_pos + (y_pos * maps.map[map_id].map_axis);
}


int get_x_pos(int tile, int map_id){

    /** public function - see header */

    return tile % maps.map[map_id].map_axis;
}


int get_y_pos(int tile, int map_id){

    /** public function - see header */

    return tile / maps.map[map_id].map_axis;
}

int get_map_id(char *map_name){

    /** public function - see header */

    str_conv_upper(map_name);

    for(int i=0; i<MAX_MAPS; i++){

        //create upper case version of map name
        char map_name_uc[80]="";
        strcpy(map_name_uc, maps.map[i].map_name);
        str_conv_upper(map_name_uc);

        //compare with map name
        if(strcmp(map_name, map_name_uc)==0) return i;
    }

    return -1;
}


void get_map_details(int actor_node, int map_id){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    send_text(socket, CHAT_SERVER, "%cYou are in %s %s", c_green3+127,
        maps.map[map_id].map_name,
        maps.map[map_id].description);

    int tile=clients.client[actor_node].map_tile;
    int x=tile / maps.map[map_id].map_axis;
    int y=tile % maps.map[map_id].map_axis;

    send_text(socket, CHAT_SERVER, "%cAt coordinates x %i y %i (tile %i)", c_green3+127, x, y, tile);
}


void get_map_developer_details(int actor_node, int map_id){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    send_text(socket, CHAT_SERVER, "%cMap     :'%s' %s", c_green3+127,
        maps.map[map_id].map_name,
        maps.map[map_id].description);

    send_text(socket, CHAT_SERVER, "%cSize     : %i x %i", c_green3+127, maps.map[map_id].map_axis, maps.map[map_id].map_axis);
    send_text(socket, CHAT_SERVER, "%cAuthor   : %s", c_green3+127, maps.map[map_id].author);
    send_text(socket, CHAT_SERVER, "%cEmail    : %s", c_green3+127, maps.map[map_id].author_email);

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    get_time_stamp_str(maps.map[map_id].upload_date, time_stamp_str);
    get_verbose_date_str(maps.map[map_id].upload_date, verbose_date_stamp_str);
    send_text(socket, CHAT_SERVER, "%cUploaded: %s %s", c_green3+127, verbose_date_stamp_str, time_stamp_str);

    switch (maps.map[map_id].development_status){

        case DEVELOPMENT:
        send_text(socket, CHAT_SERVER, "%cStatus: DEVELOPMENT", c_green3+127);
        break;

        case TESTING:
        send_text(socket, CHAT_SERVER, "%cStatus: TESTING", c_green3+127);
        break;

        case FINAL:
        send_text(socket, CHAT_SERVER, "%cStatus: FINAL", c_green3+127);
        break;
    }
}


void read_elm_header(char *elm_filename){

    /** public function - see header */

    //open elm file
    FILE *file;

    if((file=fopen(elm_filename, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read the header
    if(fread(&elm_header, sizeof(elm_header), 1, file)!=1){

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //close the elm file
    fclose(file);

    //check the magic number
    if(elm_header.magic_number[0]!='e' ||
            elm_header.magic_number[1]!='l' ||
            elm_header.magic_number[2]!='m' ||
            elm_header.magic_number[3]!='f') {

        log_event(EVENT_ERROR, "elm file magic number [%c%c%c%] != [elmf] in function %s: module %s: line %i",
                  elm_header.magic_number[0],
                  elm_header.magic_number[1],
                  elm_header.magic_number[2],
                  elm_header.magic_number[3],
                  __func__, __FILE__, __LINE__);
        stop_server();
    }

    //check the vertical and horizontal tile counts are equal
    if(elm_header.h_tiles!=elm_header.v_tiles) {

        log_event(EVENT_ERROR, "horizontal tile count [%i] and vertical tile count [%i] are unequal in function %s: module %s: line %i", elm_header.h_tiles, elm_header.v_tiles, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //check the header length
    if(elm_header.tile_map_offset!=ELM_FILE_HEADER) {

        log_event(EVENT_ERROR, "elm file header [%i] is not equal to [%i] in function %s: module %s: line %i", elm_header.tile_map_offset, ELM_FILE_HEADER, __func__, __FILE__, __LINE__);
        stop_server();
    }
}


void read_height_map(char *elm_filename, unsigned char *height_map, int *height_map_size, int *map_axis){

    /** public function - see header */

    read_elm_header(elm_filename);

    FILE *file;

    if((file=fopen(elm_filename, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    *height_map_size=elm_header.threed_object_offset-elm_header.height_map_offset;
    *map_axis=elm_header.h_tiles * 6;

    //bounds check the height map size
    if(*height_map_size>HEIGHT_MAP_MAX){

        log_event(EVENT_ERROR, "height map size [%i] exceeds maximum [%i] in function %s: module %s: line %i", *height_map_size, HEIGHT_MAP_MAX, elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read data proceding the height map
    if(fseek(file, elm_header.height_map_offset, SEEK_SET)!=0){

        log_event(EVENT_ERROR, "unable to seek file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read the height map
    if(fread(height_map, (size_t) *height_map_size, 1, file)!=1) {

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    fclose(file);
}
