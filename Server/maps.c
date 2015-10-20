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


void get_map_details(int connection, int map_id){

    /** public function - see header */

    send_text(connection, CHAT_SERVER, "%cYou are in %s %s", c_green3+127,
        maps.map[map_id].map_name,
        maps.map[map_id].description);

    send_text(connection, CHAT_SERVER, "%cIt is (%i x %i) steps wide", c_green3+127, maps.map[map_id].map_axis, maps.map[map_id].map_axis);
}

void get_map_developer_details(int connection, int map_id){

    /** public function - see header */

   send_text(connection, CHAT_SERVER, "%cMap     :'%s' %s", c_green3+127,
        maps.map[map_id].map_name,
        maps.map[map_id].description);

    send_text(connection, CHAT_SERVER, "%cArea   :  %i x %i", c_green3+127, maps.map[map_id].map_axis, maps.map[map_id].map_axis);
    send_text(connection, CHAT_SERVER, "%cAuthor : %s", c_green3+127, maps.map[map_id].author);
    send_text(connection, CHAT_SERVER, "%cEmail  : %s", c_green3+127, maps.map[map_id].author_email);

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";
    //get_time_stamp_str(  , time_stamp_str);
    //get_verbose_date_str(    , verbose_date_stamp_str);
    send_text(connection, CHAT_SERVER, "%cUploaded: %s %s", c_green3+127, verbose_date_stamp_str, time_stamp_str);

    switch (maps.map[map_id].development_status){

        case DEVELOPMENT:
        send_text(connection, CHAT_SERVER, "%cStatus: DEVELOPMENT", c_green3+127);
        break;

        case TESTING:
        send_text(connection, CHAT_SERVER, "%cStatus: TESTING", c_green3+127);
        break;

        case FINAL:
        send_text(connection, CHAT_SERVER, "%cStatus: FINAL", c_green3+127);
        break;
    }
}
