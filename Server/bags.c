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

int create_bag(int connection, int map_id, int tile){

    /** public function - see header */

    //find empty entry in bag list
    bool found_empty_bag=false;

    int i=0;
    for(i=0; i<MAX_BAGS; i++){

        if(bag[i].bag_in_use==false) {

            found_empty_bag=true;
            bag[i].bag_in_use=true;
            break;
        }
    }

    if(found_empty_bag==false){

        return -1;
    }

    //populate the bag entry
    bag[i].bag_type_id=0; /** 0=ordinary bag **/
    bag[i].map_id=map_id;
    bag[i].tile=tile;
    bag[i].bag_in_use=true;

    //calculate and store the bag creation time so we know when to poof
    gettimeofday(&time_check, NULL);
    bag[i].bag_created=time_check.tv_sec;

    //broadcast the bag drop
    broadcast_get_new_bag_packet(connection, i);

    #if DEBUG_BAGS==1
    printf("Create new bag\n");
    #endif

    //return the bag_id
    return i;
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

int get_unused_bag(){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        if(bag_list[i].mode==BAG_UNUSED) return i;

    }

    log_event2(EVENT_ERROR, "Bag array max [%i] exceeded", MAX_BAGS);

    return -1;
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


void send_destroy_bag(int connection, int bag_id){

    unsigned char packet[11];

    packet[0]=DESTROY_BAG;

    packet[1]=3;
    packet[2]=0;

    packet[3]=bag_id % 256;
    packet[4]=bag_id / 256;

    send(connection, packet, 5, 0);
}

int bag_exists(int map_id, int tile_pos){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        if(bag_list[i].tile_pos==tile_pos && bag_list[i].map_id==map_id && bag_list[i].mode!=BAG_UNUSED) return i;
    }

    return -1;
}


int get_unused_bag_slot(int bag_id){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        if(bag_list[bag_id].inventory[i].amount==0) return i;
    }

    return -1;
}

int bag_is_empty(int bag_id){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        if(bag_list[bag_id].inventory[i].amount>0) return FALSE;
    }

    return TRUE;
}


int add_item_to_bag(int bag_id, int image_id, int amount, int *bag_slot){

    //adds items to bag without resetting bag timers or sending updated inventory to client

    //find bag slot that already contains item (returns -1 if none)
    *bag_slot=get_used_bag_slot(bag_id, image_id);

    //if the bag doesn't have an existing slot for the item, create a new one
    if(*bag_slot==-1) {

        //find a free slot for the item
        *bag_slot=get_unused_bag_slot(bag_id);

        //if there are no free slots then abort function
        if(*bag_slot==-1) return FALSE;

        //place the item in the free slot
        bag_list[bag_id].inventory[*bag_slot].image_id=image_id;
    }

    //add amount to bag
    bag_list[bag_id].inventory[*bag_slot].amount+=amount;

    return TRUE;
}

int remove_item_from_bag(int bag_id, int image_id, int amount, int *bag_slot){

    //removes items from bag without resetting bag timers or sending updated inventory to client

    //find an existing slot for this item in the bag inventory (returns -1 if there are no existing slots with this item)
    *bag_slot=get_used_bag_slot(bag_id, image_id);

    //abort function if there is an attempt to remove a non existent item from the bag
    if(*bag_slot==-1) return FALSE;

    //ensure that the amount being removed from the bag doesn't exceed whats actually in the bag inventory
    if(bag_list[bag_id].inventory[*bag_slot].amount < amount) amount=bag_list[bag_id].inventory[*bag_slot].amount;

    //remove the item from the bag
    bag_list[bag_id].inventory[*bag_slot].amount-=amount;

    return TRUE;
}


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
