#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "numeric_functions.h"
#include "files.h"
#include "string_functions.h"
#include "maps.h"

void add_client_to_map(int connection, int map_id){

    int map_client_count=maps.map[map_id]->client_list_count;
    char text_out[80]="";

    maps.map[map_id]->client_list[map_client_count]=connection;

    if(maps.map[map_id]->client_list_count+1<MAX_CLIENTS) {

        maps.map[map_id]->client_list_count++;
    }
    else {

        //should never happen as we already check this bounds on the client struct array
        sprintf(text_out, "map client list range [%i] exceeded in function add_client_to_map_list: module maps.c", MAX_CLIENTS);
        log_event(EVENT_ERROR, text_out);
        exit(EXIT_FAILURE);
    }
}

void remove_client_from_map(int connection, int map_id){

    int i=0, j=0;
    int found=0;
    char text_out[80]="";

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        if(maps.map[map_id]->client_list[i]==connection) {
            found=1;
            break;
        }
    }

    if(found==0) {

        sprintf(text_out, "unable to find connection [%i] in function remove_client_from_map: module maps.c", connection);
        log_event(EVENT_ERROR, text_out);
        exit(EXIT_FAILURE);
    }

    for(j=i; i<maps.map[map_id]->client_list_count-1; i++){

        maps.map[map_id]->client_list[j]=maps.map[map_id]->client_list[j+1];
    }

    maps.map[map_id]->client_list_count--;
}

int get_map_object(int object_id, int map_id){

    int offset=object_id * maps.map[map_id]->threed_object_structure_len;
    int j=0;

    //zero struct elements
    map_object.image_id=0;
    map_object.x=0.0f;
    map_object.y=0.0f;
    map_object.z=0.0f;
    map_object.tile_pos=0;
    strcpy(map_object.e3d_filename,"");

    memcpy(map_object.e3d_filename, maps.map[map_id]->threed_object_map+offset, 80);
    extract_file_name(map_object.e3d_filename, map_object.e3d_filename);

    map_object.x=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+80) * 2.00f;
    map_object.y=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+84) * 2.00f;
    map_object.z=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+88) * 2.00f;

    //calculate tile pos as it will save us having to do this multiple times in the future
    map_object.tile_pos=(int)map_object.x + ((int)map_object.y * maps.map[map_id]->map_axis);

    #ifdef DEBUG
    printf(" file [%s] x [%f] y [%f] z [%f] tile [%i]\n", map_object.e3d_filename, map_object.x, map_object.y, map_object.z, map_object.tile_pos);
    #endif

    //find the corresponding inventory image id for this object
    do{

        if(strcmp(threed_object[j].file_name, map_object.e3d_filename)==0) {
            map_object.image_id=threed_object[j].inventory_image_id;
            return FOUND;
        }

        j++;

    }while(j<MAX_THREED_OBJECTS);

    return NOT_FOUND;
}

