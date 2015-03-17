#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h> //support for abs function

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
