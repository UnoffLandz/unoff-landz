#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function
#include <string.h>

#include "global.h"
#include "protocol.h"
#include "files.h"
#include "numeric_functions.h"
#include "database.h"
#include "harvesting.h"
#include "chat.h"
#include "broadcast.h"
#include "character_inventory.h"


void process_harvesting(int connection, time_t current_time){

    char text_out[1024]="";
    int image_id=clients.client[connection]->inventory_image_id;
    int slot=clients.client[connection]->inventory_slot;

    // exit if there's nothing to be harvested
    if(clients.client[connection]->harvest_flag==FALSE) return;

    //adjust timer to compensate for minute wrap-around>
    if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

    // exit if the harvest interval hasn't expired
    if(current_time<clients.client[connection]->time_of_last_harvest+item[image_id].interval) return;

    //update the time of harvest
    gettimeofday(&time_check, NULL);
    clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

    // if inventory is overloaded stop harvesting
    if(get_inventory_emu(connection)+ (item[image_id].emu * item[image_id].cycle_amount) > MAX_EMU){

        sprintf(text_out, "%cYou are overloaded", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        clients.client[connection]->harvest_flag=FALSE;
        return;
    }

    //update stats and send to client
    clients.client[connection]->harvest_exp+=item[image_id].exp;
    send_partial_stats(connection, HARVEST_EXP,  clients.client[connection]->harvest_exp);
    update_db_char_stats(connection);

    //Add the harvested amount to the inventory and send to client
    clients.client[connection]->client_inventory[slot].amount+= item[image_id].cycle_amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection]->client_inventory[slot].amount, slot);
    update_db_char_slot(connection, slot);
}
