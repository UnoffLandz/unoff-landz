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
            bag[i].bag_created=time_check.tv_sec;

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

    //TO DO: reset remaining bag poof time

    return amount;
}



/*
void clear_bag(int bag_id){

    int i=0;

    for(i=0;i<MAX_BAG_SLOTS; i++){
        bag_list[bag_id].inventory[i].image_id=0;
        bag_list[bag_id].inventory[i].amount=0;
        bag_list[bag_id].inventory[i].flags=0;
    }

    bag_list[bag_id].tile_pos=0;
    bag_list[bag_id].map_id=0;
    bag_list[bag_id].bag_type_id=0; //0=default bag type
    bag_list[bag_id].mode=BAG_UNUSED;
}

void bag_timer_poof_cb (struct ev_loop *loop, struct ev_timer *ev_bag_timer, int revents) {

    (void)(revents);//removes unused parameter warning
    (void)(loop);//removes unused parameter warning

    int bag_id=(int) ev_bag_timer->data;
    int map_id=bag_list[bag_id].map_id;

    bag_list[bag_id].mode=BAG_UNUSED;

    if(bag_list[bag_id].visible==TRUE){

        //poof the bag and clear its inventory
        broadcast_bag_poof(bag_id, map_id);
        clear_bag(bag_id);
    }
}


int bag_count(int connection){

    //counts bags created by connection that are currently in operation

    int i=0;
    int count=0;

    for(i=0; i<MAX_BAGS; i++){

        if(bag_list[i].connection==connection && bag_list[i].mode!=BAG_UNUSED) count++;
    }

    return count;
}



int bag_exists(int map_id, int tile_pos){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        if(bag_list[i].tile_pos==tile_pos && bag_list[i].map_id==map_id && bag_list[i].mode!=BAG_UNUSED) return i;
    }

    return -1;
}
*/

bool is_bag_empty(int bag_id){


    for(int i=0; i<MAX_BAG_SLOTS; i++){

        if(bag[bag_id].inventory[i].amount>0) return false;
    }

    return true;
}


void remove_item_from_bag(int bag_id, int amount, int bag_slot){

    //removes items from bag without resetting bag timers or sending updated inventory to client

    //check amount to be taken from slot doesn't exceed slot contents
    int slot_amount = bag[bag_id].inventory[bag_slot].amount;
    if(slot_amount<amount) amount=slot_amount;

    bag[bag_id].inventory[bag_slot].amount-=amount;

}

/*
int get_bag_inventory_emu(int bag_id){

    int i=0;
    int amount=0;
    int item_emu=0;
    int image_id=0;
    int bag_emu=0;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        amount=bag_list[bag_id].inventory[i].amount;
        image_id=0;
        item_emu=0;

        if(amount>0){
            image_id=bag_list[bag_id].inventory[i].image_id;
            item_emu=item[image_id].emu;
            bag_emu+=(amount * item_emu);
        }
    }

    return bag_emu;
}
*/
