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

#ifndef MAP_OBJECTS_H_INCLUDED
#define MAP_OBJECTS_H_INCLUDED

#include <stdbool.h>

#include "e3d.h"

#define MAX_OBJECT_ID 2000
#define OBJECT_FILE "object.lst"

struct object_type{

    char e3d_file_name[MAX_E3D_FILENAME];
    char object_name[80];
    bool harvestable;
    bool edible;
    int harvest_interval;
    int emu;
    int equipable_item_type;
    int equipable_item_id;
};
extern struct object_type object[MAX_OBJECT_ID];


#endif // MAP_OBJECTS_H_INCLUDED
