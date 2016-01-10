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
#include <stdio.h> //support data type FILE

#include "e3d.h"

#include <string.h> //support for strcmp
#include <stdlib.h> // support for exit

#include "logging.h"
#include "server_start_stop.h"

struct e3d_type e3d[MAX_E3D_TYPES];


void read_threed_object_list(char *elm_filename){

    /** public function - see header */

    read_elm_header(elm_filename);

    FILE *file;

    if((file=fopen(elm_filename, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //bounds check the threed object list
    if(elm_header.threed_object_count>MAX_MAP_OBJECTS){

        log_event(EVENT_ERROR, "map object count [%i] exceeds maximum [%i] in map [%s] function %s: module %s: line %i", elm_header.threed_object_count, MAX_MAP_OBJECTS, elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read data proceeding the threed object list
    if(fseek(file, elm_header.threed_object_offset, SEEK_SET)!=0){

        log_event(EVENT_ERROR, "unable to seek file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read the threed object list into the unsigned char array
    size_t threed_object_list_size= (size_t)(elm_header.threed_object_count * elm_header.threed_object_hash_len);

    if(fread(threed_object_list, (size_t) threed_object_list_size, 1, file)!=1) {

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    fclose(file);
}


int get_e3d_id(char *e3d_filename){

    /** public function - see header */

    for(int i=0; i<MAX_E3D_TYPES; i++){

        if(strcmp(e3d[i].e3d_filename, e3d_filename)==0){

            return i;
        }
    }

    log_event(EVENT_ERROR, "unable to find object id for e3d file [%s] in function %s: module %s: line %i", e3d_filename, __func__, __FILE__, __LINE__);

    return 0;
}

