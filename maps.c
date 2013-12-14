#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "numeric_functions.h"

void add_client_to_map(int connection, int map_id){

    int map_client_count=maps.map[map_id]->client_list_count;

    printf("add client %i to map %i\n", connection, map_id);

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
        perror("unable to find character in function remove_char_from_map");
        exit(EXIT_FAILURE);
    }

    for(j=i; i<maps.map[map_id]->client_list_count-1; i++){

        maps.map[map_id]->client_list[j]=maps.map[map_id]->client_list[j+1];
    }

    maps.map[map_id]->client_list_count--;
}

/*
void get_threed_object_pos(int object_id, int map_id, float *x_pos, float *y_pos){

    unsigned char byte[4]={0};

    //char obj_file[80]="";
    //memcpy(obj_file, maps.map[id]->threed_object_map+k, 79);
    //str_trim_right(obj_file);
    //printf("%i %s\n", j, obj_file);

    byte[0]=maps.map[map_id]->threed_object_map[(object_id*144)+80];
    byte[1]=maps.map[map_id]->threed_object_map[(object_id*144)+81];
    byte[2]=maps.map[map_id]->threed_object_map[(object_id*144)+82];
    byte[3]=maps.map[map_id]->threed_object_map[(object_id*144)+83];

    *x_pos=Uint32_to_float(byte);

    byte[0]=maps.map[map_id]->threed_object_map[(object_id*144)+84];
    byte[1]=maps.map[map_id]->threed_object_map[(object_id*144)+85];
    byte[2]=maps.map[map_id]->threed_object_map[(object_id*144)+86];
    byte[3]=maps.map[map_id]->threed_object_map[(object_id*144)+87];

    *y_pos=Uint32_to_float(byte);
}
*/
