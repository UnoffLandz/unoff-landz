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
#include <stdio.h> //supports FILE datatype
#include <stdlib.h> //supports EXIT_FAILURE
#include <string.h> //supports memset function

#include "e3d.h"
#include "logging.h"
#include "server_start_stop.h"
#include "objects.h"
#include "string_functions.h"
#include "db/db_map_object_tbl.h"

struct map_object_type map_object[MAX_MAP_OBJECTS][MAX_MAPS];


int get_object_id(int map_id, int threed_object_list_pos){

    /** public function - see header */

    int e3d_id=map_object[threed_object_list_pos][map_id].e3d_id;

    return e3d[e3d_id].object_id;
}


int get_object_tile(int map_id, int threed_object_list_pos){

    /** public function - see header */

    return map_object[threed_object_list_pos][map_id].tile;
}


