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

#ifndef MAP_OBJECTS_H_INCLUDED
#define MAP_OBJECTS_H_INCLUDED

#define MAX_MAP_OBJECTS 30

struct map_object_type{

    int object_id;
    char e3d_file_name[80];
    char object_name[80];
    enum {NON_HARVESTABLE, HARVESTABLE}harvestable;
    enum {EDIBLE, INEDIBLE}edible;
};
extern struct map_object_type map_object[MAX_MAP_OBJECTS];

#endif // MAP_OBJECTS_H_INCLUDED
