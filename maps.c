#include <stdio.h>
#include <stdlib.h>

#include "global.h"

void add_client_to_map(int client_id, int map_id){

    maps.map[map_id]->client_list[maps.map[map_id]->client_list_count]=client_id;
    maps.map[map_id]->client_list_count++;
}

void remove_client_from_map(int client_id, int map_id){

    int i=0, j=0;
    int found=0;

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        if(maps.map[map_id]->client_list[i]==client_id) {
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
