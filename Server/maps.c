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
#include "logging.h"
#include "server_protocol_functions.h"
#include "movement.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "global.h"
#include "broadcast_actor_functions.h"
#include "server_start_stop.h"

int get_proximity(int tile_pos_1, int tile_pos_2, int map_axis){

    /** RESULT  : calculates the distance between two entities on a map

    RETURNS : the distance

    PURPOSE : required by broadcast_add_new_enhanced_actor_packet

    NOTES   :
    */

    int x_diff=abs((tile_pos_1 % map_axis) - (tile_pos_2 % map_axis));
    int y_diff=abs((tile_pos_1 / map_axis) - (tile_pos_2 / map_axis));

    if(x_diff>y_diff) return x_diff; else return y_diff;
}
