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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TILE_MAP_MAX 50000
#define HEIGHT_MAP_MAX 150000


struct __attribute__((__packed__)){

    unsigned char magic_number[4];
    int h_tiles;
    int v_tiles;
    int tile_map_offset;
    int height_map_offset;

    int threed_object_hash_len;
    int threed_object_count;
    int threed_object_offset;

    int twod_object_hash_len;
    int twod_object_count;
    int twod_object_offset;

    int lights_object_hash_len;
    int lights_object_count;
    int lights_object_offset;

    unsigned char dungeon_flag;
    unsigned char version_flag;
    unsigned char reserved1;
    unsigned char reserved2;

    int ambient_red;
    int ambient_green;
    int ambient_blue;

    int particles_object_hash_len;
    int particles_object_count;
    int particles_object_offset;

    int clusters_offset;

    int reserved_9;
    int reserved_10;
    int reserved_11;
    int reserved_12;
    int reserved_13;
    int reserved_14;
    int reserved_15;
    int reserved_16;
    int reserved_17;

} elm_header;

void get_square(int start_h, int start_v, int *finish_h, int *finish_v, int *height, unsigned char *height_map_byte){

    int tile=start_h + (start_v * 192);

    *finish_h=0, *finish_v=192;
    *height=height_map_byte[tile];

    for(int i=start_h; i<192; i++){

        *finish_h=i;
        if(height_map_byte[i]!=*height) break;

        for(int j=i+start_v; j<192; j++){

            if(height_map_byte[i+(j*192)]!=*height){

                if(j<*finish_v) *finish_v=j;
            }
        }
    }
}


int main(int argc, char *argv[]){

    char elm_filename[80]="startmap.elm";

    //open map file
    FILE *file;

    if((file=fopen(elm_filename, "r"))==NULL) {

        printf("unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    unsigned char header_byte[124]= {0};
    if(fread(header_byte, 124, 1, file)!=1) {

        printf("unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    //copy bytes to header struct so we can extract data
    memcpy(&elm_header, header_byte, 124);

    //extract the tile map from the elm file
    unsigned char tile_map_byte[TILE_MAP_MAX]= {0};
    int tile_map_size=elm_header.height_map_offset - elm_header.tile_map_offset;

    if(fread(tile_map_byte, tile_map_size, 1, file)!=1) {

        printf("unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    //extract the height map from the elm file
    unsigned char height_map_byte[HEIGHT_MAP_MAX]= {0};
    int height_map_size=elm_header.threed_object_offset - elm_header.height_map_offset;

    if(fread(height_map_byte, height_map_size, 1, file)!=1) {

        printf("unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int max_v, max_h;

    int start_v=0, start_h=0, height=0;

    get_square(start_h, start_v, &max_h, &max_v, &height, height_map_byte);

    printf("start_h %i start_v %i max h %i max v %i height %i\n", start_h, start_v, max_h, max_v, height);

    start_v=max_v;
    start_h=max_h;
    if(start_h==191) start_h=0;

    get_square(start_h, start_v, &max_h, &max_v, &height, height_map_byte);

    printf("start_h %i start_v %i max h %i max v %i height %i\n", start_h, start_v, max_h, max_v, height);

    start_v=max_v;
    start_h=max_h;
    if(start_h==191) start_h=0;

    get_square(start_h, start_v, &max_h, &max_v, &height, height_map_byte);

    printf("start_h %i start_v %i max h %i max v %i height %i\n", start_h, start_v, max_h, max_v, height);

    return EXIT_SUCCESS;
}


