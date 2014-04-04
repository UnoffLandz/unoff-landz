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

int get_char_carry_capacity(int connection){

    int race_id=clients.client[connection]->char_type;
    int initial_carry_capacity=race[race_id].initial_carry_capacity;
    int carry_capacity_multiplier=race[race_id].carry_capacity_multiplier;

    return initial_carry_capacity + (carry_capacity_multiplier * clients.client[connection]->physique);
}

int get_char_visual_proximity(int connection){

    int race_id=clients.client[connection]->char_type;
    int initial_visual_proximity=race[race_id].initial_visual_proximity;
    int visual_proximity_multiplier=race[race_id].visual_proximity_multiplier;

    return initial_visual_proximity + (visual_proximity_multiplier * clients.client[connection]->vitality);
}

int get_char_chat_proximity(int connection){

    int race_id=clients.client[connection]->char_type;
    int initial_chat_proximity=race[race_id].initial_chat_proximity;
    int chat_proximity_multiplier=race[race_id].chat_proximity_multiplier;

    return initial_chat_proximity + (chat_proximity_multiplier * clients.client[connection]->will);
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
