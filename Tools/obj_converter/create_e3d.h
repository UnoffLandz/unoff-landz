/***
    Copyright 2014 UnoffLandz

    This file is part of obj_conv.

    obj_conv is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    obj_conv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with obj_conv.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef CREATE_E3D_H_INCLUDED
#define CREATE_E3D_H_INCLUDED

#define HEADER_OFFSET 28
#define HEADER_LENGTH 40
#define VERTEX_HASH_LENGTH 14
#define INDEX_HASH_LENGTH 2
#define MATERIAL_HASH_LENGTH 172

struct obj_vertex_type{
    float x;
    float y;
    float z;
};

struct obj_texture_type{
    float x;
    float y;
};

struct obj_normal_type{
    float x;
    float y;
    float z;
};

struct obj_material_type{

    char material_name[FILELINE_LEN];
    float min_x;
    float min_y;
    float min_z;
    float max_x;
    float max_y;
    float max_z;
    int min_idx;
    int max_idx;
};

struct obj_index_type{

    int v;
    int t;
    int n;
    int mtl;
};

struct obj_type {

    int vertex_count;
    int texture_count;
    int normals_count;
    int material_count;
    int index_count;

    int data_len;

    char material_lib[FILENAME_MAX];

    struct obj_vertex_type vertex[MAX_VERTEX];
    struct obj_texture_type texture[MAX_VERTEX];
    struct obj_normal_type normal[MAX_VERTEX];
    struct obj_material_type material[MAX_MATERIAL];
    struct obj_index_type index[MAX_INDEX];
};

void read_obj_data(char *filename, struct obj_type *obj);

void create_e3d_file(char *filename, struct obj_type *obj);

#endif // CREATE_E3D_H_INCLUDED
