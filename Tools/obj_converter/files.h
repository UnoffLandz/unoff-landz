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

#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#define MAX_DATA 1000000

#define MAX_VERTEX (64*1024)
#define MAX_INDEX (64*1024)
#define MAX_MATERIAL 256

#define EXPECTED_HEADER_OFFSET 28
#define EXPECTED_HEADER_LENGTH 40

#define MATERIAL_STR_LEN 129
#define MD5_HASH_LEN 16
#define FILENAME_LEN 256
#define FILELINE_LEN 256
#define SUFFIX_LEN 3
#define OBJ_TAG_LEN 4

#define VERSION "OL 1.0" //program version

#include <SDL/SDL_types.h> //required for Uint16

enum {
    TRUE=1,
    FALSE=-1
};

enum {
    FOUND,
    NOT_FOUND
};

int Uint16_to_dec( unsigned char *byte);
float Uint32_to_float(unsigned char *byte);
int Uint32_to_dec(unsigned char *byte);
void str_trim_right(char *str_in);

int file_exists(char *file_name);
char *truefalse(int i);
void get_filename_prefix( char *filename, char *prefix);
void get_md5(char *filename, int offset, unsigned char *md5_hash);

#endif // FILES_H_INCLUDED
