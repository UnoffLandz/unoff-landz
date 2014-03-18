#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function

#include "global.h"
#include "protocol.h"
#include "files.h"
#include "numeric_functions.h"
#include "database.h"
#include "harvesting.h"
#include "chat.h"
#include "broadcast.h"

int find_inventory_item(int connection, int find_item_image_id){

    int i, j;
    int slot_item_image_id;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        j=1+(i*8);

        slot_item_image_id=Uint16_to_dec(clients.client[connection]->inventory[j], clients.client[connection]->inventory[j+1]);

        if(slot_item_image_id==find_item_image_id) {

            inventory.slot=i;
            inventory.image_id=slot_item_image_id;
            inventory.amount=Uint32_to_dec(clients.client[connection]->inventory[j+3], clients.client[connection]->inventory[j+4], clients.client[connection]->inventory[j+5], clients.client[connection]->inventory[j+6]);

            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_inventory_slot_amount(int connection, int inventory_slot){

    int j=1+(inventory_slot*8);

    return Uint32_to_dec(clients.client[connection]->inventory[j+3], clients.client[connection]->inventory[j+4], clients.client[connection]->inventory[j+5], clients.client[connection]->inventory[j+6]);
}

void put_inventory_slot_amount(int connection, int inventory_slot, int amount){

    int j=1+(inventory_slot*8);

    clients.client[connection]->inventory[j+3]=amount % 256;
    clients.client[connection]->inventory[j+4]=amount / 256 % 256;
    clients.client[connection]->inventory[j+5]=amount / 256 / 256 % 256;
    clients.client[connection]->inventory[j+6]=amount / 256 / 256 / 256 % 256;
}

void new_inventory_item(int connection, int item_image_id){

    int i, j;
    char text_out[1024]="";
    int slot_amount;

    //search for slot with 0 amount
    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        j=1+(i*8);

        slot_amount=get_inventory_slot_amount(connection, i);

        if(slot_amount==0){

            printf("new slot %i\n", i);

            clients.client[connection]->inventory[j]=item_image_id % 256;
            clients.client[connection]->inventory[j+1]=item_image_id / 256;

            inventory.slot=i;
            inventory.image_id=item_image_id;
            inventory.amount=0;

            return;
        }
    }

    sprintf(text_out, "%cSorry, but you don't have enough inventory slots", c_red1+127);
    send_raw_text_packet(connection, CHAT_SERVER, text_out);

    return;
}

void process_harvesting(int connection, time_t current_time){

    int slot_amount;
    int inventory_image_id=clients.client[connection]->inventory_image_id;

      // exit if there's nothing to be harvested
    if(clients.client[connection]->harvest_flag==FALSE) return;

    //adjust timer to compensate for minute wrap-around>
    if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

    // exit if the harvest interval hasn't expired
    if(current_time<clients.client[connection]->time_of_last_harvest+item[inventory_image_id].interval) return;

    //update the time of harvest
    gettimeofday(&time_check, NULL);
    clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

    //update stats and send to client
    clients.client[connection]->harvest_exp+=item[inventory_image_id].exp;
    send_partial_stats(connection, HARVEST_EXP,  clients.client[connection]->harvest_exp);
    update_db_char_stats(connection);

    //calculate the new slot amount
    slot_amount=get_inventory_slot_amount(connection, clients.client[connection]->inventory_slot);
    slot_amount+=item[inventory_image_id].cycle_amount;

    //update the inventory string and send to client
    put_inventory_slot_amount(connection, clients.client[connection]->inventory_slot, slot_amount);
    update_db_char_inventory(connection);
    send_get_new_inventory_item(connection, inventory_image_id, slot_amount, clients.client[connection]->inventory_slot);
}
