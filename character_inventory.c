#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function
#include <string.h>

#include "character_inventory.h"
#include "global.h"

int get_used_inventory_slot(int connection, int image_id){

    int i;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection]->client_inventory[i].image_id==image_id) return i;
    }

    return NOT_FOUND;
}

int get_unused_inventory_slot(int connection){

    int i;

    //search for slot with no image id
    for(i=0; i<MAX_INVENTORY_SLOTS; i++){
        if(clients.client[connection]->client_inventory[i].amount==0) return i;
    }

    return NOT_FOUND;
}

int get_inventory_emu(int connection){

    int i=0;
    int total_emu=0;
    int image_id=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        image_id=clients.client[connection]->client_inventory[i].image_id;
        total_emu +=(clients.client[connection]->client_inventory[i].amount * item[image_id].emu);
    }

    return total_emu;

}
