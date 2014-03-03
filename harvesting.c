#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function

#include "global.h"
#include "protocol.h"
#include "files.h"
#include "numeric_functions.h"
#include "database.h"

int get_inventory_item_str_pos(int connection, int find_item){

    int i;
    int inventory_count=clients.client[connection]->inventory[0];
    int inventory_item=0;

    for(i=0; i<inventory_count; i++){

        inventory_item=Uint16_to_dec(clients.client[connection]->inventory[1+(i*8)], clients.client[connection]->inventory[2+(i*8)]);
        printf("%i %i\n", clients.client[connection]->inventory[1+(i*8)], clients.client[connection]->inventory[2+(i*8)]);

        if(find_item==inventory_item) return i;
    }

    return 0;//item not found
}

int get_inventory_item_pos(int connection, int str_pos){

    return clients.client[connection]->inventory[7+(str_pos*8)];

}

int get_inventory_item_amount(int connection, int str_pos){

    return Uint32_to_dec(clients.client[connection]->inventory[3+(str_pos*8)], clients.client[connection]->inventory[4+(str_pos*8)], clients.client[connection]->inventory[5+(str_pos*8)], clients.client[connection]->inventory[6+(str_pos*8)]);
}

void new_inventory_item(int connection, int item, int amount){

    int i=0;
    int inventory_count=clients.client[connection]->inventory[0];
    int inventory[256];

    for(i=0; i<inventory_count; i++){
        inventory[clients.client[connection]->inventory[7]]=1;
    }

    for(i=0; i<256; i++){

        if(inventory[i]==0){

            inventory_count++;
            clients.client[connection]->inventory[0]=inventory_count;

            clients.client[connection]->inventory[1+(inventory_count*8)]=item % 256;
            clients.client[connection]->inventory[2+(inventory_count*8)]=item / 256 % 256;

            clients.client[connection]->inventory[3+(inventory_count*8)]=amount % 256;
            clients.client[connection]->inventory[4+(inventory_count*8)]=amount / 256 % 256;
            clients.client[connection]->inventory[5+(inventory_count*8)]=amount / 256 / 256 % 256;
            clients.client[connection]->inventory[6+(inventory_count*8)]=amount / 256 / 256 / 256 % 256;

            clients.client[connection]->inventory[7+(inventory_count*8)]=i;

            return;
        }
    }

    printf("no spare inventory slots\n");
    exit(EXIT_FAILURE);
}

void update_inventory_item_amount(int connection, int str_pos, int amount){

    clients.client[connection]->inventory[3+(str_pos*8)]=amount % 256;
    clients.client[connection]->inventory[4+(str_pos*8)]=amount / 256 % 256;
    clients.client[connection]->inventory[5+(str_pos*8)]=amount / 256 / 256 % 256;
    clients.client[connection]->inventory[6+(str_pos*8)]=amount / 256 / 256 / 256 % 256;
}

void process_harvesting(int connection, time_t current_time){

    unsigned char packet[1024];
    int amount=0;
    int str_pos=0;

    if(clients.client[connection]->harvest_flag==TRUE){

        //adjust timer to compensate for minute wrap-around>
        if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

        // check for time of next harvest
        if(current_time>clients.client[connection]->time_of_last_harvest+clients.client[connection]->harvest_item_interval) {

            //update the time of harvest
            gettimeofday(&time_check, NULL);
            clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

            //update stats and send to client
            clients.client[connection]->harvest_exp+=clients.client[connection]->harvest_item_exp;
            send_partial_stats(connection, HARVEST_EXP, clients.client[connection]->harvest_exp);

            //save updated stats to database
            update_db_char_stats(connection);

            str_pos=get_inventory_item_str_pos(connection, clients.client[connection]->image_id);

            if(str_pos>0){
                //existing slot
                amount=get_inventory_item_amount(connection, str_pos);
                amount++;
                update_inventory_item_amount(connection, str_pos, amount);
            }
            else {
                //new slot
                amount=1;
                new_inventory_item(connection, clients.client[connection]->image_id, amount);
                clients.client[connection]->inventory[0]++;
                str_pos=clients.client[connection]->inventory[0];
            }

            packet[0]=GET_NEW_INVENTORY_ITEM;

            packet[1]=9;
            packet[2]=0;

            packet[3]=clients.client[connection]->image_id % 256;
            packet[4]=clients.client[connection]->image_id / 256;

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
