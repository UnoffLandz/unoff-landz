/***
    Copyright 2014 UnoffLandz

    This file is part of e3d_conv.

    e3d_conv is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    e3d_conv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with e3d_conv.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef OBJ_H_INCLUDED
#define OBJ_H_INCLUDED

void create_obj_file(char *filename,
                     struct p_options_type *p_options,
                     struct e3d_header_type *e3d_header,
                     struct vertex_hash_type2 *vertex_hash2,
                     struct index_hash_type2 *index_hash2,
                     struct materials_hash_type2 *materials_hash2);

void create_mtl_file(char *filename, struct e3d_header_type *e3d_header, struct materials_hash_type2 *materials_hash2);

#endif // OBJ_H_INCLUDED
