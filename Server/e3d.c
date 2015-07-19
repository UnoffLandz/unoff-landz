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
#include "e3d.h"

#include <string.h> //support for strcmp
#include <stdlib.h> // support for exit

#include "logging.h"

struct e3d_type e3d[MAX_E3D];

int get_e3d_id(char *e3d_filename){

    /** public function - see header */

    for(int i=0; i<MAX_E3D; i++){

        if(strcmp(e3d[i].e3d_filename, e3d_filename)==0){

            return i;
        }
    }

    //log_event(EVENT_ERROR, "unable to find object id for e3e file [%s] in function %s: module %s: line %i", e3d_filename, __func__, __FILE__, __LINE__);
    //exit(EXIT_FAILURE);

    return 0;
}

