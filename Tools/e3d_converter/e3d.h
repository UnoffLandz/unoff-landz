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

#ifndef E3D_H_INCLUDED
#define E3D_H_INCLUDED

struct e3d_header_type{

    char magic_number[4];
    unsigned char version[4];
    unsigned char md5_hash[16];
    int header_offset;

    int vertex_count;
    int vertex_size;
    int vertex_offset;

    int index_count;
    int index_size;
    int index_offset;

    int material_count;
    int material_size;
    int material_offset;

    unsigned char options_flag;
    unsigned char format_flag;
    unsigned char reserved1;
    unsigned char reserved2;
};

struct vertex_hash_type2{

    float uvx;
    float uvy;

    float vx;
    float vy;
    float vz;

    float nx;
    float ny;
    float nz;

    float t;
};

struct index_hash_type2{
    int idx;
};

struct materials_hash_type2{
    int options;
    char name[127];
    float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
    int minidx;
    int maxidx;
    int start;
    int count;
};

void read_e3d_header(char *filename, struct e3d_header_type *e3d_header);
void read_e3d_vertex_hash(char *filename, int offset, int vertex_count, struct vertex_hash_type2 *vertex_hash2);
void read_e3d_index_hash(char* filename, int offset, int index_count, struct index_hash_type2 *index_hash2);
void read_e3d_materials_hash(char* filename, int offset, int materials_count, struct materials_hash_type2 *materials_hash2);
void optimise_vertices(int vertex_count, int index_count, int *optimisation, struct vertex_hash_type2 *vertex_hash2, struct index_hash_type2 *index_hash2);

void report_e3d_header_data(char *filename, int report, char *report_filename, struct e3d_header_type *e3d_header);
void report_vertex_data(int report, char *report_filename, int vertex_count, struct vertex_hash_type2 *vertex_hash2);
void report_index_data(int report, char *report_filename, int index_count, struct index_hash_type2 *index_hash2);
void report_materials_data(int report, char *report_filename, int material_count, struct materials_hash_type2 *materials_hash2);
void report_vertex_optimisation(int report, char *report_filename, int optimisation, int vertex_count);

#endif // E3D_H_INCLUDED
