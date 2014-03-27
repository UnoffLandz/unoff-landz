#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "numeric_functions.h"
#include "files.h"
#include "string_functions.h"
#include "maps.h"

void add_client_to_map_list(int connection, int map_id){

    int map_client_count=maps.map[map_id]->client_list_count;

    maps.map[map_id]->client_list[map_client_count]=connection;
    maps.map[map_id]->client_list_count++;
}

void remove_client_from_map_list(int connection, int map_id){

    int i=0, j=0;
    int found=0;

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        if(maps.map[map_id]->client_list[i]==connection) {
            found=1;
            break;
        }
    }

    if(found==0) {
        perror("unable to find character in function remove_client_from_map");
        exit(EXIT_FAILURE);
    }

    for(j=i; i<maps.map[map_id]->client_list_count-1; i++){

        maps.map[map_id]->client_list[j]=maps.map[map_id]->client_list[j+1];
    }

    maps.map[map_id]->client_list_count--;
}

int get_map_object(int object_id, int map_id){

    int offset=object_id * maps.map[map_id]->threed_object_structure_len;
    char obj_file[80]="";
    int j=0;

    //zero struct elements
    map_object.image_id=0;
    map_object.x=0.0f;
    map_object.y=0.0f;
    map_object.z=0.0f;
    strcpy(map_object.e3d_filename,"");

    memcpy(obj_file, maps.map[map_id]->threed_object_map+offset, 80);
    extract_file_name(obj_file, obj_file);

    map_object.x=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+80) * 2.00f;
    map_object.y=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+84) * 2.00f;
    map_object.z=Uint32_to_float(maps.map[map_id]->threed_object_map+offset+88) * 2.00f;

    printf("[%s] [%f] [%f] [%f]\n", map_object.e3d_filename, map_object.x, map_object.y, map_object.z);

    //find the corresponding inventory image id for this object
    do{

        if(strcmp(threed_object[j].file_name, obj_file)==0) {
            map_object.image_id=threed_object[j].inventory_image_id;
            return FOUND;
        }

        j++;

    }while(j<MAX_THREED_OBJECTS);

    return NOT_FOUND;
}

