#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for gettimeofday function

#include "global.h"
#include "protocol.h"
#include "files.h"
#include "numeric_functions.h"

int get_inventory_item_str_pos(int connection, int find_item){

    int i;
    int char_id=clients.client[connection]->character_id;
    int inventory_count=characters.character[char_id]->inventory[0];
    int inventory_item=0;

    for(i=0; i<inventory_count; i++){

        inventory_item=Uint16_to_dec(characters.character[char_id]->inventory[1+(i*8)], characters.character[char_id]->inventory[2+(i*8)]);
        printf("%i %i\n", characters.character[char_id]->inventory[1+(i*8)], characters.character[char_id]->inventory[2+(i*8)]);

        if(find_item==inventory_item) return i;
    }

    return 0;//item not found
}

int get_inventory_item_pos(int connection, int str_pos){

    int char_id=clients.client[connection]->character_id;

    return characters.character[char_id]->inventory[7+(str_pos*8)];

}

int get_inventory_item_amount(int connection, int str_pos){

    int char_id=clients.client[connection]->character_id;

    return Uint32_to_dec(characters.character[char_id]->inventory[3+(str_pos*8)], characters.character[char_id]->inventory[4+(str_pos*8)], characters.character[char_id]->inventory[5+(str_pos*8)], characters.character[char_id]->inventory[6+(str_pos*8)]);
}

void new_inventory_item(int connection, int item, int amount){

    int i=0;
    int char_id=clients.client[connection]->character_id;
    int inventory_count=characters.character[char_id]->inventory[0];
    int inventory[256];

    for(i=0; i<inventory_count; i++){
        inventory[characters.character[char_id]->inventory[7]]=1;
    }

    for(i=0; i<256; i++){

        if(inventory[i]==0){

            inventory_count++;
            characters.character[char_id]->inventory[0]=inventory_count;

            characters.character[char_id]->inventory[1+(inventory_count*8)]=item % 256;
            characters.character[char_id]->inventory[2+(inventory_count*8)]=item / 256 % 256;

            characters.character[char_id]->inventory[3+(inventory_count*8)]=amount % 256;
            characters.character[char_id]->inventory[4+(inventory_count*8)]=amount / 256 % 256;
            characters.character[char_id]->inventory[5+(inventory_count*8)]=amount / 256 / 256 % 256;
            characters.character[char_id]->inventory[6+(inventory_count*8)]=amount / 256 / 256 / 256 % 256;

            characters.character[char_id]->inventory[7+(inventory_count*8)]=i;

            return;
        }
    }

    printf("no spare inventory slots\n");
    exit(EXIT_FAILURE);
}

void update_inventory_item_amount(int connection, int str_pos, int amount){

    int char_id=clients.client[connection]->character_id;

    characters.character[char_id]->inventory[3+(str_pos*8)]=amount % 256;
    characters.character[char_id]->inventory[4+(str_pos*8)]=amount / 256 % 256;
    characters.character[char_id]->inventory[5+(str_pos*8)]=amount / 256 / 256 % 256;
    characters.character[char_id]->inventory[6+(str_pos*8)]=amount / 256 / 256 / 256 % 256;
}

int test;

void process_harvesting(int connection, time_t current_time){

    unsigned char packet[1024];

    int char_id=clients.client[connection]->character_id;
    int item=clients.client[connection]->harvest_item;
    int image_id=harvestables[item].image_id;
    int interval=harvestables[item].interval;
    int amount=0;
    int str_pos=0;

    if(clients.client[connection]->harvest_flag==TRUE){

        //adjust timer to compensate for minute wrap-around>
        if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

        // check for time of next harvest
        if(current_time>clients.client[connection]->time_of_last_harvest+interval) {

            //update the time of harvest
            gettimeofday(&time_check, NULL);
            clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

            //update stats
            characters.character[char_id]->harvest_exp+=harvestables[item].exp;
            send_partial_stats(connection, HARVEST_EXP, characters.character[char_id]->harvest_exp);

            str_pos=get_inventory_item_str_pos(connection, image_id);

            if(str_pos>0){
                //existing slot
                amount=get_inventory_item_amount(connection, str_pos);
                amount++;
                update_inventory_item_amount(connection, str_pos, amount);
            }
            else {
                //new slot
                amount=1;
                new_inventory_item(connection, image_id, amount);
                characters.character[char_id]->inventory[0]++;
                str_pos=characters.character[char_id]->inventory[0];
            }

            //save updated exp to char record
            //save_character(characters.character[char_id]->char_name, char_id);

            packet[0]=GET_NEW_INVENTORY_ITEM;

            packet[1]=9;
            packet[2]=0;

            packet[3]=image_id % 256;
            packet[4]=image_id / 256;

            packet[5]=amount % 256;//quantity
            packet[6]=amount / 256 % 256;
            packet[7]=amount / 256 / 256 % 256;
            packet[8]=amount / 256 / 256 / 256 % 256;

            packet[9]=get_inventory_item_pos(connection, str_pos);

            packet[10]=0;//flags

            send(connection, packet, 11, 0);
        }
    }
}
