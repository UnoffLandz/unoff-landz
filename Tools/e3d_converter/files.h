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
#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#define MAX_DATA 1000000

#define MAX_VERTEX (64*1024)
#define MAX_INDEX (64*1024)
#define MAX_MATERIAL 256

#define E3D_EXPECTED_HEADER_OFFSET 28
#define E3D_EXPECTED_HEADER_LENGTH 40

#define E3D_MATERIAL_STR_LEN 129
#define E3D_MD5_HASH_LEN 16
#define FILENAME_LEN 256
#define FILELINE_LEN 256
#define SUFFIX_LEN 3

#define VERSION "OL 1.8" //program version

#define REPORT_FILENAME "report.txt"

enum {
    TRUE=1,
    FALSE=-1
};

struct p_options_type{

    int vertex_optimisation;
    int vertex_optimisation_report;
    int header_report;
    int vertex_report;
    int index_report;
    int materials_report;
    int dds_report;
    int report_only;
    int reverse_y_axis;
    int create_mtl;
    int silent;

    //int calculate_normals;
    int file_dump;
};

int Uint16_to_dec( unsigned char *byte);
float Uint32_to_float(unsigned char *byte);
int Uint32_to_dec(unsigned char *byte);

int file_exists(char *file_name);
char *truefalse(int i);
int get_file_size(char *file_name); //test

void get_filename_prefix(char *filename, char *prefix);
//void calculate_normals(struct e3d_type *e3d);
void get_md5(char *filename, int offset, unsigned char *md5_hash);

#endif // FILES_H_INCLUDED
