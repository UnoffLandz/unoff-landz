#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "files.h"
#include "protocol.h" //required for read_motd
#include "string_functions.h"
#include "numeric_functions.h"
#include "datetime_functions.h"
#include "maps.h"
#include "database.h"

void clear_file(char *file_name){

    //used to clear logs on boot up

    FILE *file;

    if((file=fopen(file_name, "a"))!=NULL){

        fclose(file);

        if((file=fopen(file_name, "w"))==NULL){

            log_event2(EVENT_ERROR, "unable to clear file [%s]", file_name);
            exit(EXIT_FAILURE);
        }
    }

    log_event2(EVENT_INITIALISATION, "clearing file [%s]", file_name);
}

const char *byte_to_binary(int x){

    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

void load_e3d(char *filename){

    FILE *file;
    unsigned char byte[50000];

    //open the e3d file
    if((file=fopen(filename, "r"))==NULL) {

        log_event2(EVENT_ERROR, "unable to open file [%s] in function load_e3d: module files.c", filename);
        exit(EXIT_FAILURE);
    }

/****************************************************************************************************/

    //read file identification bytes
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read identification bytes for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    if(byte[0]!='e' || byte[1]!='3' || byte[2]!='d' || byte[3]!='x'){

        printf("identification bytes [%c %c %c %c] should = 'e3dx' in file [%s] in function load_map:module files.c", byte[0], byte[1], byte[2], byte[3], filename);
        exit (EXIT_FAILURE);
    }

    printf("file identification bytes [%c %c %c %c]\n", byte[0], byte[1], byte[2], byte[3]);

/****************************************************************************************************/

    //read e3d format version
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read e3d format version for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    printf("e3d format version [%i %i %i %i]\n", byte[0], byte[1], byte[2], byte[3]);

/****************************************************************************************************/

    //read dummy (this looks like md5 string)
    if(fread(&byte, 16, 1, file)!=1){

        printf("unable to read dummy for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

/****************************************************************************************************/

    //read header size
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read header offset for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int header_size=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("header size [%i]\n", header_size);

/***************************************************************************************************/

    //read vertex count
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read vertex map offset for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int vertex_count=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("vertex count [%i]\n", vertex_count);

/****************************************************************************************************/

    //read vertex size
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read vertex count for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int vertex_size=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("vertex size [%i]\n", vertex_size);

/****************************************************************************************************/

    //read vertex offset
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read vertex size for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int vertex_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("vertex offset [%i]\n", vertex_offset);

/****************************************************************************************************/

     //read index count
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read index count for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int index_count=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("index count [%i]\n", index_count);

/****************************************************************************************************/

     //read index size
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read index size for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int index_size=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("index size [%i]\n", index_size);

/****************************************************************************************************/

    //read index offset
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read index offset for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int index_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("index offset [%i]\n", index_offset);

/****************************************************************************************************/

    //read material count
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read material count for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int material_count=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("material count [%i]\n", material_count);

/****************************************************************************************************/

   //read material size
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read material size for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int material_size=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("material size [%i]\n", material_size);

/****************************************************************************************************/

   //read material offset
    if(fread(&byte, 4, 1, file)!=1){

        printf("unable to read material offset for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    int material_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    printf("material offset [%i]\n", material_offset);

/****************************************************************************************************/

    //read vertex options
    if(fread(&byte, 1, 1, file)!=1){

        printf("unable to read vertex options for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    unsigned int vertex_options=byte[0]; //flag determining whether this is a ground object, has tangents or extra uv's

    printf("vertex options [%i] [%s]\n", byte[0], byte_to_binary(byte[0]));

    if((vertex_options & 1)==0) printf("has normals\n"); else printf("no normals\n");
    if(((vertex_options / 2) & 1)==0) printf("has tangent\n"); else printf("no tangents\n");
    if(((vertex_options / 4) & 1)==0) printf("has extra uv\n"); else printf("no extra uv\n");

/****************************************************************************************************/

    //read vertex format
    if(fread(&byte, 1, 1, file)!=1){

        printf("unable to read reserved char 1 for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    unsigned int vertex_format=byte[0]; // flag determining whether haf floats are used for position, uv and/or extra uv's and if normals and tangents are compressed */

    printf("vertex format [%i]\n", vertex_format);

/****************************************************************************************************/

    //read reserved char 2
    if(fread(&byte, 1, 1, file)!=1){

        printf("unable to read reserved char 2 for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    unsigned int reserved_2=byte[0];

    printf("reserved char 2 [%i]\n", reserved_2);

/****************************************************************************************************/

    //read reserved char 3
    if(fread(&byte, 1, 1, file)!=1){

        printf("unable to read reserved char 3 for file [%s] in function load_e3d: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    unsigned int reserved_3=byte[0];

    printf("reserved char 3 [%i]\n", reserved_3);

/****************************************************************************************************/

    //read material list
    if(fread(&byte, material_offset-vertex_offset, 1, file)!=1){

        printf("unable to read material list in function load_e3d: module files.c");
        exit (EXIT_FAILURE);
    }
    //printf("%i\n", material_offset-vertex_offset);
/****************************************************************************************************/

    int i=0;
    int material_options=0;
    char material_filename[128]="";
    float min_x, min_y, min_z, min_index;
    float max_x, max_y, max_z, max_index;
    float index, count;

    for(i=0; i<material_count; i++){

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        material_options=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("material options [%i]\n", material_options);

        if(fread(&byte, 128, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        memcpy(material_filename, byte, 127);
        printf("material filename [%s]\n", material_filename);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        min_x=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("min x [%f]\n", min_x);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        min_y=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("min y [%f]\n", min_y);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        min_z=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("min z [%f]\n", min_z);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        max_x=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("max x [%f]\n", max_x);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        max_y=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("max y [%f]\n", max_y);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        max_z=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("max z [%f]\n", max_z);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        min_index=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("min index [%f]\n", min_index);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        max_index=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("max index [%f]\n", max_index);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        index=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("index [%f]\n", index);

        if(fread(&byte, 4, 1, file)!=1){

            printf("unable to material [%i] in function load_e3d: module files.c", i);
            exit (EXIT_FAILURE);
        }

        count=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
        printf("count [%f]\n", count);

    }
}

int load_map(int id){

    FILE *file;

    unsigned char byte[4];
    char elm_filename[80]="";

    int h_tiles=0, v_tiles=0;
    int tile_map_offset=0;
    int height_map_offset=0;

    int tile_map_size=0;
    int height_map_size=0;

    int threed_object_offset=0;
    int threed_object_map_size=0;

    int twod_object_offset=0;
    int twod_object_map_size=0;
    //int twod_object_structure_len=0;
    //int twod_object_count=0;

    int lights_object_offset=0;

    //open the elm file and extract map data
    extract_file_name(maps.map[id]->elm_filename, elm_filename);

    if((file=fopen(elm_filename, "r"))==NULL) {

        log_event2(EVENT_ERROR, "unable to open file [%s] in function load_map: module files.c", elm_filename);
        exit(EXIT_FAILURE);
    }

    //read file identification bytes (should = elmf)
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read identification bytes for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    if(byte[0]!='e' || byte[1]!='l' || byte[2]!='m' || byte[3]!='f'){

        log_event2(EVENT_ERROR, "identification bytes [%c%c%c%c] should = 'elmf' in file [%s] in function load_map:module files.c", byte[0], byte[1], byte[2], byte[3], elm_filename );
        exit (EXIT_FAILURE);
    }

    //read horizontal tile count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read horizontal tile count for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    h_tiles=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);

    if(h_tiles<MIN_MAP_AXIS) {

        log_event2(EVENT_ERROR, "horizontal tile count [%i] is less than MIN_MAP_AXIS [%i] in file [%s] in function load_map: module files.c", h_tiles, MIN_MAP_AXIS, elm_filename);
        exit(EXIT_FAILURE);
    }
    //printf("horizontal tiles %i\n", h_tiles);

    /* because a char can move 6 steps per tile, we calculate the map axis as 6x the number
    of horizontal tiles. In the case of Isla Prima, the number of horizontal tiles is 32, so
    our map axis is calculated as 6x32=192 */
    maps.map[id]->map_axis=h_tiles*6;

    //read vertical tile count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read vertical tile count for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    v_tiles=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("vertical tiles %i\n", j);

    /* Because we assume that maps will always be a perfect square, if vertical tiles do not equal
    the number of horizontal tiles, we need to flag it as a serious problem */
    if(maps.map[id]->map_axis/6!=v_tiles){

        log_event2(EVENT_ERROR, "horizontal tile count [%i] unequal to vertical tile count [%i] in file [%s]", h_tiles, v_tiles, elm_filename);
        exit (EXIT_FAILURE);
    }

    //read tile map offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read tile map offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    tile_map_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("tile map offset %i\n", tile_map_offset);

    // We assume that tile map offset will always be 124 bytes otherwise our map data will be loaded incorrectly
    if(tile_map_offset!=ELM_FILE_HEADER_LEN){

        log_event2(EVENT_ERROR, "file header size [%i] unequal to ELM_FILE_HEADER_LEN [%i] for file [%s] in function load_map: module files.c", elm_filename, tile_map_offset, ELM_FILE_HEADER_LEN);
        exit (EXIT_FAILURE);
    }

    //read height map offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read height map offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    height_map_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("height map offset %i\n", height_map_offset);

    //read 3d object structure length
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read threed object structure length for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    maps.map[id]->threed_object_structure_len=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("3d object structure length %i\n", maps.map[id]->threed_object_structure_len);

    //read 3d object count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read threed object count in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    maps.map[id]->threed_object_count=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("3d object count %i\n",  maps.map[id]->threed_object_count);

    //read 3d object offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read threed object offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    threed_object_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("3d object offset %i\n", threed_object_offset);

    //read 2d object structure length
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read twod object structure length for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //twod_object_structure_len=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("2d object structure len %i\n", twod_object_structure_len);

    //read 2d object count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read twod object count for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //twod_object_count=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("2d object count %i\n", twod_object_count);

    //read 2d object offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read twod object offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    twod_object_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("2d object offset %i\n", twod_object_offset);

    //read lights structure length
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read lights structure length for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("lights structure length %i\n", j);

    //read lights count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read lights count for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("lights count %i\n", j);

    //read lights offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read lights offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    lights_object_offset=Uint32_to_dec(byte[0], byte[1], byte[2], byte[3]);
    //printf("lights offset %i\n", lights_object_offset);

    //read dungeon flag
    if(fread(byte, 1, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read dungeon flag for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //printf("dungeon flag %i\n", bytes[0]);

    //read version flag
    if(fread(byte, 1, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read file version byte for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    if(byte[0]!=1){

        log_event2(EVENT_ERROR, "version byte [%i] is unequal to expected version [%i] for file [%s] in function load_map: module files.c", byte[0], ELM_FILE_VERSION, elm_filename);
        exit(EXIT_FAILURE);
    }

    //printf("version flag %i\n", bytes[0]);

    //read reserved byte 3
    if(fread(byte, 1, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 3 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //printf("reserved byte 3 %i\n", bytes[0]);

    //read reserved byte 4
    if(fread(byte, 1, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 4 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //printf("reserved byte 4 %i\n", bytes[0]);

    //read ambient red
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read ambient red for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient red %f\n", (float)j);

    //read ambient green
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read ambient green for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient green %f\n", (float)j);

    //read ambient blue
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read ambient blue for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("ambient blue %f\n", (float)j);

    //read particles structure len
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read particles structure length for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles structure len %i\n", j);

    //read particles count
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read particles count for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles count %i\n", j);

    //read particles offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read particles offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("particles offset %i\n", j);

    //read clusters offset
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read clusters offset for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("clusters offset %i\n", j);

    //read reserved 9
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 9 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 9 %i\n", j);

    //read reserved 10
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 10 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 10 %i\n", j);

    //read reserved 11
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 11 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 11 %i\n", j);

    //read reserved 12
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 12 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 12 %i\n", j);

    //read reserved 13
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 13 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 13 %i\n", j);

    //read reserved 14
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 14 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 14 %i\n", j);

    //read reserved 15
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 15 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 15 %i\n", j);

    //read reserved 16
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 16 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 16 %i\n", j);

    //read reserved 17
    if(fread(byte, 4, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read reserved byte 17 for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    //j=Uint32_to_dec(bytes[0], bytes[1], bytes[2], bytes[3]);
    //printf("res 17 %i\n", j);

    /**read tile map

    the height map offset indicates the end of the tile map, hence we use it to calculate
    the size of the tile map by deducting the tile map offset (header size). We can then read
    that number of bytes to extract our tile map data. In the case of Isla Prima, the tile map
    consists of 32 x 32 =1024 bytes */

    tile_map_size=height_map_offset-tile_map_offset;

    if(tile_map_size>TILE_MAP_MAX) {

        log_event2(EVENT_ERROR, "tile map size [%i] exceeds maximum [i] for file [%s] in function load_map: module files.c", tile_map_size, TILE_MAP_MAX, elm_filename);
        exit(EXIT_FAILURE);
    }

    maps.map[id]->tile_map_size=tile_map_size;

     if(fread(maps.map[id]->tile_map, tile_map_size, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read tile map in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    /**read height map

    the 3d object offset indicates the end of the height map, hence we use it to calculate
    the size of height map by deducting the height map offset. We can then read that number of
    bytes to extract our height map data. In the case of Isla Prima, the height map consists
    of 192 x 192 = 36864 bytes */

    height_map_size=threed_object_offset-height_map_offset;

    if(height_map_size>HEIGHT_MAP_MAX) {

        log_event2(EVENT_ERROR, "height map size [%i] exceeds maximum [%i] for file [%s] in function load_map: module files.c", height_map_size, HEIGHT_MAP_MAX, elm_filename);
        exit(EXIT_FAILURE);
    }

    maps.map[id]->height_map_size=height_map_size;

    if(fread(maps.map[id]->height_map, height_map_size, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read height map in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

    /**read 3d object map

    the 2d object offset indicates the end of the 3d object map, hence we use it to calculate
    the size of 3d object map by deducting the 3d object map offset. We can then read that number of
    bytes to extract our 3d object map data. In the case of Isla Prima, the 3d object map is 167760 bytes*/

    threed_object_map_size=twod_object_offset-threed_object_offset;

    if(threed_object_map_size>THREED_OBJECT_MAP_MAX) {

        log_event2(EVENT_ERROR, "threed object map size [%i] exceeds maximum [%i] for file [%s] in function load_map: module files.c", threed_object_map_size, THREED_OBJECT_MAP_MAX, elm_filename);
        exit(EXIT_FAILURE);
    }

    maps.map[id]->threed_object_map_size=threed_object_map_size;

    if(fread(maps.map[id]->threed_object_map, threed_object_map_size, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read threed object map for file [%s] in function load_map: module files.c", elm_filename);
        exit (EXIT_FAILURE);
    }

/*
    //TEST 3D STRUCTURE
    int i,j,k=0;
    float x=0.0f, y=0.0f, z=0.0f;

    for(i=0; i<maps.map[id]->threed_object_count; i++){

        for(j=k; j<k+144; j++){
            printf("%c", maps.map[id]->threed_object_map[j]);
        }

        x=Uint32_to_float(maps.map[id]->threed_object_map+k+80);
        y=Uint32_to_float(maps.map[id]->threed_object_map+k+84);
        z=Uint32_to_float(maps.map[id]->threed_object_map+k+88);

        printf("  [%f] [%f] [%f]\n", x, y, z);

        k+=144;
    }
*/

    /**read 2d object map

    the lights object offset indicates the end of the 2d object map, hence we use it to calculate
    the size of 2d object map by deducting the 2d object map offset. We can then read that number of
    bytes to extract our 2d object map data. In the case of Isla Prima, the 2d object map is 130944 bytes*/

    twod_object_map_size=lights_object_offset-twod_object_offset;

    if(twod_object_map_size>TWOD_OBJECT_MAP_MAX) {

        log_event2(EVENT_ERROR, "twod object map size [%i] exceeds maximum [%i] for file [%s] in function load_map: module files.c", twod_object_map_size, TWOD_OBJECT_MAP_MAX, elm_filename);
        exit(EXIT_FAILURE);
    }

    maps.map[id]->twod_object_map_size=twod_object_map_size;

    if(fread(maps.map[id]->twod_object_map, twod_object_map_size, 1, file)!=1){

        log_event2(EVENT_ERROR, "unable to read twod object map in function load_map: module files.c", elm_filename);
        exit(EXIT_FAILURE);
    }

/*
    //TEST 2D structure

    int k=0;
    int j=0;
    unsigned char byte[4]={0};
    unsigned char x;

    for(j=0; j<(128*200);j+=128){

        byte[0]=maps.map[id]->twod_object_map[j+80];
        byte[1]=maps.map[id]->twod_object_map[j+81];
        byte[2]=maps.map[id]->twod_object_map[j+82];
        byte[3]=maps.map[id]->twod_object_map[j+83];
        printf("x_pos %f\n", Uint32_to_float(byte));

        byte[0]=maps.map[id]->twod_object_map[j+84];
        byte[1]=maps.map[id]->twod_object_map[j+85];
        byte[2]=maps.map[id]->twod_object_map[j+86];
        byte[3]=maps.map[id]->twod_object_map[j+87];
        printf("y_pos %f\n", Uint32_to_float(byte));

        for(k=j; k<j+80; k++){
            x=maps.map[id]->twod_object_map[k];
            printf("%c", x);
        }
        printf("#\n");
    }
  exit(1);
*/

    return FOUND;
}

int get_file_size(char *file_name){

    FILE *file;
    int count=0;

    if((file=fopen(file_name, "r"))==NULL) return NOT_FOUND;

    while(fgetc(file) != EOF) {
        count++;
    }

    fclose(file);

    return count;
}

void log_to_file(char *filename, char *text) {

    FILE *file;

    //print to console
    #ifdef DEBUG
    printf("%s\n", text);
    #endif

    //attempt to open the file for append
    if((file=fopen(filename, "a"))==NULL) {

        //if we don't have an existing list file, create a new one
        log_event2(EVENT_INITIALISATION, "Unable to find existing file [%s]. Creating new one", filename);

        if((file=fopen(filename, "a"))==NULL) {

            log_event2(EVENT_ERROR, "unable to create file [%s] in function log_to_file: module files.c", filename);
            exit(EXIT_FAILURE);
        }
    }

    //attempt to write to the file
    if(!fprintf(file, "%s\n", text)){

        log_event2(EVENT_ERROR, "unable to write to file [%s] in function log_to_file: module files.c", filename);
        exit(EXIT_FAILURE);
    }

    fflush(file);

    fclose(file);
}

void log_event(int event_type, char *text_in){

    char file_name[1024]="";
    char text_out[1024]="";

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    get_time_stamp_str(time(NULL), time_stamp_str);
    get_date_stamp_str(time(NULL), date_stamp_str);

    switch(event_type){

        case EVENT_NEW_CHAR:
            strcpy(file_name, CHARACTER_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] New Character Created - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_ERROR:
            strcpy(file_name, ERROR_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_SESSION:
            strcpy(file_name, SESSION_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] session - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_CHAT:
            strcpy(file_name, CHAT_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Event - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_MOVE_ERROR:
            strcpy(file_name, MOVE_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Move-error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        default:
            strcpy(file_name, ERROR_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Unknown Event - %s", date_stamp_str, time_stamp_str, text_in);
        break;

    }

    log_to_file(file_name, text_out);
}

void log_event2(int event_type, char *fmt, ...){

    char file_name[80]="";
    char text_in[1024]="";
    char text_out[1024]="";

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    va_list args;
    va_start(args, fmt);
    vsprintf(text_in, fmt, args);

    get_time_stamp_str(time(NULL), time_stamp_str);
    get_date_stamp_str(time(NULL), date_stamp_str);

    switch(event_type){

        case EVENT_NEW_CHAR:
            strcpy(file_name, CHARACTER_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Character - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_ERROR:
            strcpy(file_name, ERROR_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_SESSION:
            strcpy(file_name, SESSION_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] session - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_CHAT:
            strcpy(file_name, CHAT_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Event - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_MOVE_ERROR:
            strcpy(file_name, MOVE_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Move-error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_INITIALISATION:
            strcpy(file_name, INITIALISATION_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] %s", date_stamp_str, time_stamp_str, text_in);
        break;

        default:
            strcpy(file_name, ERROR_LOG_FILE_NAME);
            sprintf(text_out, "[%s][%s] Unknown Event - %s", date_stamp_str, time_stamp_str, text_in);
        break;

    }

    va_end(args);

    log_to_file(file_name, text_out);
}

void create_configuration_file(char *file_name, char *file_format_str){

    FILE *file;
    char dummy=' ';
    log_event2(EVENT_INITIALISATION, "Creating configuration file [%s]", file_name);

    //create the file
    if((file=fopen(file_name, "w"))==NULL) {

            log_event2(EVENT_ERROR, "unable to open file [%s] in function create_configuration_file: module files.c", file_name);
            exit(EXIT_FAILURE);
    }

    //write the format header information to the file
    fprintf(file, file_format_str, dummy); //the dummy prevents the compiler warning that file_format is not string literal
    fclose(file);

    //log the result and stop the server
    log_event2(EVENT_INITIALISATION, "You need to edit the file [%s] with your data", file_name);
    exit(EXIT_FAILURE);
}

void load_database_item_table_data(char *file_name){

    FILE *file;
    int image_id=0;
    char item_name[80]="";
    int harvestable=0;
    int emu=0;
    int interval=0;
    int exp=0;
    int food_value=0;
    int food_cooldown=0;
    int organic_nexus=0;
    int vegetal_nexus=0;
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, ITEM_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database item_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_item_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //load entries to database
    while (fscanf(file, "%i %s %i %i %i %i %i %i %i %i\n",
            &image_id,
            item_name,
            &harvestable,
            &emu,
            &interval,
            &exp,
            &food_value,
            &food_cooldown,
            &organic_nexus,
            &vegetal_nexus)!=-1){

        //remove underscores which are needed for fscanf to ignore spaces in item name
        str_remove_underscores(item_name);

        //add item to database
        add_item(image_id, item_name, harvestable, emu, interval, exp, food_value, food_cooldown,
                organic_nexus,
                vegetal_nexus);

        //zero variables
        image_id=0;
        strcpy(item_name, "");
        harvestable=0;
        emu=0;
        interval=0;
        exp=0;
        food_value=0;
        food_cooldown=0;
        organic_nexus=0;
        vegetal_nexus=0;
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_threed_object_table_data(char *file_name){

    FILE *file;
    int image_id=0;
    char e3d_file_name[80]="";
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, THREED_OBJECT_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database threed_object table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_item_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %s\n", &image_id, e3d_file_name)!=-1){

        add_threed_object(e3d_file_name, image_id);

        //zero variables
        image_id=0;
        strcpy(e3d_file_name, "");
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_map_table_data(char *file_name){

    FILE *file;
    int map_id;
    char map_name[80]="";
    char elm_file_name[80]="";
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, MAP_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database map_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_map_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %s %s\n", &map_id, map_name, elm_file_name)!=-1){

        add_map(map_id, map_name, elm_file_name);

        //zero variables
        map_id=0;
        strcpy(map_name, "");
        strcpy(elm_file_name, "");
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_channel_table_data(char *file_name){

    FILE *file;
    int channel_id=0;
    int channel_type=0;
    char password[80]="";
    char channel_name[80]="";
    char channel_description[80]="";
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, CHANNEL_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database channel_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_channel_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }

    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %i %s %s %s\n", &channel_id, &channel_type, password, channel_name, channel_description)!=-1){

        //remove underscores which are needed for fscanf to ignore spaces in channel name and description
        str_remove_underscores(channel_name);
        str_remove_underscores(channel_description);

        //add channel to database channel_table
        // second parameter is owner_id 0=system
        add_channel(channel_id, 0, channel_type, password, channel_name, channel_description);

        //zero variables
        channel_id=0;
        channel_type=0;
        strcpy(password, "");
        strcpy(channel_name, "");
        strcpy(channel_description, "");
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_race_table_data(char *file_name){

    FILE *file;
    int race_id=0;
    char race_name[20]="";
    char race_description[160]="";
    int initial_emu=0;
    float emu_multiplier=0.0f;
    int initial_visual_proximity=0;
    float visual_proximity_multiplier=0.0f;
    int initial_chat_proximity=0;
    float chat_proximity_multiplier=0.0f;
    float initial_night_vis=0;
    float night_vis_multiplier=0.0f;
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, RACE_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database race_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_race_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %s %s %i %f %i %f %i %f %f %f\n",
                       &race_id,
                       race_name,
                       race_description,

                       &initial_emu,
                       &emu_multiplier,

                       &initial_visual_proximity,
                       &visual_proximity_multiplier,

                       &initial_chat_proximity,
                       &chat_proximity_multiplier,

                       &initial_night_vis,
                       &night_vis_multiplier
                       )!=-1){


        //remove underscores which are needed for fscanf to ignore spaces in channel name and description
        str_remove_underscores(race_name);
        str_remove_underscores(race_description);

        //add race to database race_table
        add_race(race_id, race_name, race_description, initial_emu, emu_multiplier,
                     initial_visual_proximity,
                     visual_proximity_multiplier,
                     initial_chat_proximity,
                     chat_proximity_multiplier,
                     initial_night_vis,
                     night_vis_multiplier
                     );

        //zero variables
        race_id=0;
        strcpy(race_name, "");
        strcpy(race_description, "");
        initial_emu=0;
        emu_multiplier=0.0f;
        initial_visual_proximity=0;
        visual_proximity_multiplier=0.0f;
        initial_chat_proximity=0;
        chat_proximity_multiplier=0.0f;
        initial_night_vis=0.0f;
        night_vis_multiplier=0.0f;
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_guild_table_data(char *file_name){

    FILE *file;
    int guild_id=0;
    char guild_tag[4]="";
    char guild_name[20]="";
    char guild_description[160]="";
    int tag_colour=0;
    int logon_colour=0;
    int logoff_colour=0;
    int chan_text_colour=0;
    int chan_id=0;
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, GUILD_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database guild_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_guild_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %s %s %s %i %i %i %i %i\n",
                       &guild_id,

                       guild_tag,
                       guild_name,
                       guild_description,

                       &tag_colour,
                       &logon_colour,
                       &logoff_colour,
                       &chan_text_colour,

                       &chan_id
                      )!=-1){

        //remove underscores which are needed for fscanf to ignore spaces in channel name and description
        str_remove_underscores(guild_tag);
        str_remove_underscores(guild_name);
        str_remove_underscores(guild_description);

        //add guild to database race_table
        add_guild(guild_id, guild_tag, guild_name, guild_description, tag_colour, logon_colour, logoff_colour,
                  chan_text_colour,
                  chan_id);

        //zero variables
        guild_id=0;
        strcpy(guild_tag, "");
        strcpy(guild_name, "");
        strcpy(guild_description, "");

        tag_colour=0;
        logon_colour=0;
        logoff_colour=0;
        chan_text_colour=0;
        chan_id=0;
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}

void load_database_bag_type_table_data(char *file_name){

    FILE *file;
    int bag_type_token=0;
    char bag_type_description[160]="";
    int poof_time=0;
    int max_emu=0;
    char buf[1024]="";

    //check we have an existing file and, if not, then create one
    if((file=fopen(file_name, "r"))==NULL) create_configuration_file(file_name, BAG_TYPE_DATA_FILE_FORMAT);

    //load data from the text file
    log_event2(EVENT_INITIALISATION, "Loading data to database bag_type_table...");

    //skip notes
    do{

        if(fgets(buf, 1024, file)==NULL){

            log_event2(EVENT_ERROR, "Unable to read file [%s] in function load_database_guild_data_table: module files.c", file_name);
            exit(EXIT_FAILURE);
        }
    }
    while(buf[0]==ASCII_HASH);

    //scan the entries and load to database
    while (fscanf(file, "%i %s %i %i\n",
                       &bag_type_token,
                       bag_type_description,
                       &poof_time,
                       &max_emu
                      )!=-1){

        //remove underscores which are needed for fscanf to ignore spaces in channel name and description
        str_remove_underscores(bag_type_description);

        //add guild to database race_table
        add_bag_type(bag_type_token, bag_type_description, poof_time, max_emu);

        //zero variables
        bag_type_token=0;
        strcpy(bag_type_description, "");
        poof_time=0;
        max_emu=0;
    }

    fclose(file);

    log_event2(EVENT_INITIALISATION, "---");
}
