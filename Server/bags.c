#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bags.h"
#include "clients.h"
#include "date_time_functions.h"
#include "objects.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "broadcast_actor_functions.h"
#include "items.h"

#define DEBUG_BAGS 0

struct bag_type bag[MAX_BAGS];


int find_bag_slot(int bag_id, int object_id){

    /** public function - see header **/

    //find an existing bag slot or next empty slot or return -1 if no slots available
    int slot=-1;
    for(int i=0; i<MAX_BAG_SLOTS; i++){

        if(bag[bag_id].inventory[i].object_id==0 && slot==-1) slot=i;

        if(bag[bag_id].inventory[i].object_id==object_id){

            return i;
            break;
        }
    }

    return slot;
}


int create_bag(int map_id, int tile){

    /** public function - see header */

    for(int i=0; i<MAX_BAGS; i++){

        if(bag[i].bag_in_use==false) {

            bag[i].bag_in_use=true;
            bag[i].bag_type_id=0; /** 0=ordinary bag **/
            bag[i].map_id=map_id;
            bag[i].tile=tile;

            //calculate and store the bag creation time so we know when to poof
            gettimeofday(&time_check, NULL);
            bag[i].bag_refreshed=time_check.tv_sec;

            #if DEBUG_BAGS==1
            printf("Create new bag\n");
            #endif

            return i;
        }
    }

    #if DEBUG_BAGS==1
    printf("Max bags exceeded. Can't create new bag\n");
    #endif

    return -1;
}


int add_to_bag(int bag_id, int object_id, int amount, int slot){

    /** public function - see header */

    //add the item to the inventory
    bag[bag_id].inventory[slot].amount+=amount;
    bag[bag_id].inventory[slot].object_id=object_id;

    //update bag poof time
    gettimeofday(&time_check, NULL);
    bag[bag_id].bag_refreshed=time_check.tv_sec;

    return amount;
}


void remove_item_from_bag(int bag_id, int amount, int bag_slot){

    /** public function - see header */

    //check amount to be taken from slot doesn't exceed slot contents
    int slot_amount = bag[bag_id].inventory[bag_slot].amount;
    if(slot_amount<amount) amount=slot_amount;

    bag[bag_id].inventory[bag_slot].amount-=amount;

    //update bag poof time
    gettimeofday(&time_check, NULL);
    bag[bag_id].bag_refreshed=time_check.tv_sec;
}

bool bag_empty(int bag_id){

    /** public function - see header */

    for( int i=0; i<MAX_BAG_SLOTS; i++){

        if(bag[bag_id].inventory[i].amount>0) return false;
    }

    return true;
}


int get_bag_inventory_emu(int bag_id){

    /** public function - see header */

    int bag_emu=0;

    for(int i=0; i<MAX_BAG_SLOTS; i++){

        int item_id=bag[bag_id].inventory[i].object_id;
        int item_emu=item[item_id].emu ;
        bag_emu+=bag[bag_id].inventory[i].amount * item_emu;
    }

    return bag_emu;
}
