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

    //unsigned char byte[4]={0};
    int offset=object_id * maps.map[map_id]->threed_object_structure_len;
    char obj_file[80]="";
    char text_out[1024]="";
    int i,j=0;

    //printf("object id %i  offset %i\n", object_id, offset );

    for(i=offset+80; i>offset; i--){
        if(maps.map[map_id]->threed_object_map[i]==ASCII_BACKSLASH) break;
    }

    //if no backslash is found the the 3d object entry must be incorrect
    if(i==offset){
        sprintf(text_out, "error in function get_threed_object: module maps.c");
        log_event(EVENT_ERROR, text_out);
        exit(1);
    }

    //extract the filename from the path
    memcpy(obj_file, maps.map[map_id]->threed_object_map+i+1, offset+80-i);
    str_trim_right(obj_file);

    printf("%s\n", obj_file);

    //find the corresponding inventory image id for this object
    do{
        if(strcmp(threed_object[j].file_name, obj_file)==0) return threed_object[j].inventory_image_id;

        j++;

    }while(i<MAX_THREED_OBJECTS);

    return NOT_FOUND;
}

