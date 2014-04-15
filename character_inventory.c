#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function
#include <string.h>

#include "character_inventory.h"
#include "global.h"

void send_get_new_inventory_item(int connection, int item_image_id, int amount, int slot){

    unsigned char packet[11];

    packet[0]=GET_NEW_INVENTORY_ITEM;

    packet[1]=9;
    packet[2]=0;

    packet[3]=item_image_id % 256;
    packet[4]=item_image_id / 256;

    packet[5]=amount % 256;
    packet[6]=amount / 256 % 256;
    packet[7]=amount / 256 / 256 % 256;
    packet[8]=amount / 256 / 256 / 256 % 256;

    packet[9]=slot;

    packet[10]=0;//flags

    send(connection, packet, 11, 0);
}

void send_here_your_inventory(int connection){

    int i=0;
    unsigned char packet[(MAX_INVENTORY_SLOTS*8)+4];

    int data_length=2+(MAX_INVENTORY_SLOTS*8);
    int j;

    packet[0]=HERE_YOUR_INVENTORY;
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;

    packet[3]=MAX_INVENTORY_SLOTS;


    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        j=4+(i*8);

        packet[j+0]=clients.client[connection]->client_inventory[i].image_id % 256; //image_id of item
        packet[j+1]=clients.client[connection]->client_inventory[i].image_id / 256;

        packet[j+2]=clients.client[connection]->client_inventory[i].amount % 256; //amount (when zero nothing is shown in inventory)
        packet[j+3]=clients.client[connection]->client_inventory[i].amount / 256 % 256;
        packet[j+4]=clients.client[connection]->client_inventory[i].amount / 256 / 256 % 256;
        packet[j+5]=clients.client[connection]->client_inventory[i].amount / 256 / 256 / 256 % 256;

        packet[j+6]=i; //inventory pos (starts at 0)
        packet[j+7]=0; //flags
    }

    send(connection, packet, (MAX_INVENTORY_SLOTS*8)+4, 0);
}

int get_used_inventory_slot(int connection, int image_id, int *slot){

    int i;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection]->client_inventory[i].image_id==image_id) {
            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_unused_bag(int *bag_id){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        if(bag_list[i].status==EMPTY){

            *bag_id=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_unused_inventory_slot(int connection, int *slot){

    int i;

    //search for slot with no image id
    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection]->client_inventory[i].amount==0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_char_carry_capacity(int connection){

    int race_id=clients.client[connection]->char_type;
    int initial_carry_capacity=race[race_id].initial_carry_capacity;
    int carry_capacity_multiplier=race[race_id].carry_capacity_multiplier;

    return initial_carry_capacity + (carry_capacity_multiplier * clients.client[connection]->physique);
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

void send_get_new_bag(int connection, int bag_id){

    unsigned char packet[11];

    int map_id=clients.client[connection]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int x_pos=clients.client[connection]->map_tile % map_axis;
    int y_pos=clients.client[connection]->map_tile / map_axis;

    packet[0]=GET_NEW_BAG;

    packet[1]=6;
    packet[2]=0;

    packet[3]=x_pos % 256;
    packet[4]=x_pos / 256;

    packet[5]=y_pos % 256;
    packet[6]=y_pos / 256;

    packet[7]=bag_id; //bag list number

    send(connection, packet, 8, 0);
}

void send_destroy_bag(int connection, int bag_id){

    unsigned char packet[11];

    packet[0]=DESTROY_BAG;

    packet[1]=3;
    packet[2]=0;

    packet[3]=bag_id % 256;
    packet[4]=bag_id / 256;

    send(connection, packet, 5, 0);
}

int bag_exists(int map_id, int tile_pos, int *bag_id){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        //if an existing bag exists, use this to place the drop items in
        if(bag_list[i].tile_pos==tile_pos && bag_list[i].map_id==map_id && bag_list[i].status==FULL) {

            *bag_id=i;
            return TRUE;
        }
    }

    return FALSE;
}

void send_here_your_ground_items(int connection, int bag_id){

    //displays the bag inventory

    unsigned char packet[(MAX_BAG_SLOTS*7)+5];

    int i=0, j=0;
    int data_length=0;
    int image_id=0, quantity=0;

    packet[0]=HERE_YOUR_GROUND_ITEMS;

    data_length=(MAX_BAG_SLOTS*7)+2;

    packet[1]=data_length % 256;
    packet[2]=data_length / 256;

    packet[3]=MAX_BAG_SLOTS;

    for(i=0;i<MAX_BAG_SLOTS; i++){

        j=(i*7);

        image_id=bag_list[bag_id].inventory[i].image_id;
        quantity=bag_list[bag_id].inventory[i].amount;


        packet[4+j]=image_id % 256; //image_id
        packet[5+j]=image_id / 256;

        packet[6+j]=quantity % 256; //quantity;
        packet[7+j]=quantity / 256 % 256;
        packet[8+j]=quantity / 256 / 256 % 256;
        packet[9+j]=quantity / 256 / 256 / 256 % 256;

        packet[10+j]=i; //slot

        printf("slot [%i] image [%i] amount [%i]\n", i, image_id, quantity);
    }

    send(connection, packet, (MAX_BAG_SLOTS*7)+4, 0);

}

void send_get_new_ground_item(int connection, int item_image_id, int amount, int slot){

    unsigned char packet[11];

    packet[0]=GET_NEW_GROUND_ITEM;

    packet[1]=8;
    packet[2]=0;

    packet[3]=item_image_id % 256;
    packet[4]=item_image_id / 256;

    packet[5]=amount % 256;
    packet[6]=amount / 256 % 256;
    packet[7]=amount / 256 / 256 % 256;
    packet[8]=amount / 256 / 256 / 256 % 256;

    packet[9]=slot;//slot

    send(connection, packet, 10, 0);
}

int get_used_bag_slot(int bag_id, int image_id, int *slot){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        if(bag_list[bag_id].inventory[i].image_id==image_id) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_unused_bag_slot(int bag_id, int *slot){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        if(bag_list[bag_id].inventory[i].amount==0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}
