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

#ifndef E3D_H_INCLUDED
#define E3D_H_INCLUDED

#include "maps.h"

#define E3D_FILE "e3d.lst"
#define MAX_E3D_TYPES 100

struct e3d_type{

    char e3d_filename[MAX_E3D_FILENAME];
    float x;
    float y;
    float z;
    int object_id;
};

struct e3d_list_type {

    bool data_loaded;
    struct e3d_type e3d[MAX_E3D_TYPES];
};
extern struct e3d_list_type e3ds;

struct __attribute__((__packed__)){

    char e3d_path_and_filename[MAX_E3D_FILENAME];
    float x_pos;
    float y_pos;
    float z_pos;
    float x_rot;
    float y_rot;
    float z_rot;
    unsigned char self_lit;
    unsigned char blended;
    unsigned char reserved[2];
    float r;
    float g;
    float b;
    unsigned char reserved2[24];
}threed_object_list[MAX_MAP_OBJECTS];


/** RESULT  : reads the 3d object list from an elm file into an array

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void read_threed_object_list(char *elm_filename);


/** RESULT  : finds the object id for an e3d file

    RETURNS : void

    PURPOSE : to find the object corresponding to an entry in an elm 3d object list

    NOTES   :
**/
int get_e3d_id(char *e3d_filename);


#endif // E3D_H_INCLUDED
