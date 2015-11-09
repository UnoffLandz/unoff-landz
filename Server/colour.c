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

#include <string.h>

#include "colour.h"
#include "string_functions.h"

struct colour_array_entry {

    char colour_name[80];
    int colour_code;
};


struct colour_array_entry colour_entries[] = {

//   Colour      Enum
//   ---------- -----------
    {"red1",     c_red1},
    {"orange1",  c_orange1},
    {"yellow1",  c_yellow1},
    {"green1",   c_green1},
    {"blue1",    c_blue1},
    {"purple1",  c_purple1},
    {"grey1",    c_grey1},
    {"red2",     c_red2},
    {"orange2",  c_orange2},
    {"yellow2",  c_yellow2},
    {"green2",   c_green2},
    {"blue2",    c_blue2},
    {"purple2",  c_purple2},
    {"grey2",    c_grey2},
    {"red3",     c_red3},
    {"orange3",  c_orange3},
    {"yellow3",  c_yellow3},
    {"green3",   c_green3},
    {"blue3",    c_blue3},
    {"purple3",  c_purple3},
    {"grey3",    c_grey3},
    {"red4",     c_red4},
    {"orange4",  c_orange4},
    {"yellow4",  c_yellow4},
    {"green4",   c_green4},
    {"blue4",    c_blue4},
    {"purple4",  c_purple4},
    {"grey4",    c_grey4}
};


int get_colour_code( char *colour_name){

    /** public function - see header */

    str_conv_lower(colour_name);

    for(int i=MIN_COLOUR_CODE; i<MAX_COLOUR_CODE; i++){

        if(strcmp(colour_entries[i].colour_name, colour_name)==0) return i;
    }

    return -1;
}
