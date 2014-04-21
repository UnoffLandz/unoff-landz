#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function
#include <string.h>
#include <ev.h>

#include "character_inventory.h"
#include "global.h"
#include "files.h"
#include "broadcast.h"
#include "database.h"
#include "protocol.h"

static void bag_poof_cb (struct ev_loop *loop, struct ev_timer *ev_bag_timer, int revents) {

    (void)(revents);//removes unused parameter warning
    (void)(loop);//removes unused parameter warning

    int bag_id=(int) ev_bag_timer->data;
    int map_id=bag_list[bag_id].map_id;

    broadcast_bag_poof(bag_id, map_id);
}

void send_s_close_bag(int connection){

    //tell client to close bag

    unsigned char packet[4];

    packet[0]=S_CLOSE_BAG;

    packet[1]=1;
    packet[2]=0;

    send(connection, packet, 3, 0);
}

void send_get_new_inventory_item(int connection, int item_image_id, int amount, int slot){

    unsigned char packet[12];

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

int is_inventory_slots_exceeded(int connection, int image_id){

    int i=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection]->client_inventory[i].image_id==image_id) return FALSE;

        if(clients.client[connection]->client_inventory[i].amount==0) return FALSE;
    }

    return TRUE;
}

int get_used_inventory_slot(int connection, int image_id, int *slot){

    int i;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection]->client_inventory[i].image_id==image_id && clients.client[connection]->client_inventory[i].amount>0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_unused_bag(int *bag_id){

    int i;

    for(i=1; i<MAX_BAGS; i++){

        if(bag_list[i].status==UNUSED){

            *bag_id=i;
            return FOUND;
        }
    }

    log_event2(EVENT_ERROR, "Bag array max [%i] exceeded", MAX_BAGS);

    return NOT_FOUND;
}

int get_unused_inventory_slot(int connection, int *slot){

    int i;

    //search for slot with no contents
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
        if(bag_list[i].tile_pos==tile_pos && bag_list[i].map_id==map_id && bag_list[i].status==USED) {

            *bag_id=i;
            return FOUND;
        }
    }
    return NOT_FOUND;
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

        if(bag_list[bag_id].inventory[i].image_id==image_id && bag_list[bag_id].inventory[i].amount>0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_unused_bag_slot(int bag_id, int *slot){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        printf("bag id %i, slot %i amount %i\n", bag_id, i, bag_list[bag_id].inventory[i].amount );

        if(bag_list[bag_id].inventory[i].amount==0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int bag_is_empty(int bag_id){

    int i;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        printf("bag %i slot %i amount %i\n", bag_id, i, bag_list[bag_id].inventory[i].amount);

        if(bag_list[bag_id].inventory[i].amount>0) {
            printf("not empty\n");
            return FALSE;
        }
    }

    return TRUE;
}

int create_empty_bag(int map_id, int tile, int *bag_id, struct ev_loop *loop){

    //create a bag without using bag token

    if(get_unused_bag(bag_id)==NOT_FOUND) return FALSE;

    bag_list[*bag_id].connection=-1; //-1=system
    bag_list[*bag_id].map_id=map_id;
    bag_list[*bag_id].tile_pos=tile;
    bag_list[*bag_id].status=USED;

    broadcast_bag_drop(*bag_id, map_id);

    #ifdef DEBUG
        printf("Create new bag [%i] at position [%i] on map [%s]\n", *bag_id, tile, maps.map[map_id]->map_name);
    #endif

    ev_bag_timer[*bag_id].data=(int*) *bag_id;
    ev_timer_init(&ev_bag_timer[*bag_id], bag_poof_cb, 20, 0);
    ev_timer_start(loop, &ev_bag_timer[*bag_id]);

    return TRUE;
}

int create_empty_bag_from_token(int connection, int map_id, int tile, int *bag_id, struct ev_loop *loop){

    int slot=0;
    int token_image_id=0;

    //check char inventory for bag tokens starting from the lightest to the heaviest
    for(token_image_id=612; token_image_id<615; token_image_id++){

        if(get_used_inventory_slot(connection, token_image_id, &slot)==FOUND) {

            //if we've found a bag token then remove it from the char inventory
            if(remove_item_from_inventory(connection, token_image_id, 1, slot)==FALSE){

                log_event2(EVENT_ERROR, "couldn't remove bag token from inventory in function create_empty_bag_from_token: module character_inventory.c", token_image_id, clients.client[connection]->char_name);
                return NOT_FOUND;
            }

            //now create the empty bag
            if(create_empty_bag(map_id, tile, bag_id, loop)==FALSE){

                log_event2(EVENT_ERROR, "couldn't create empty bag in function create_empty_bag_from_token: module character_inventory.c", token_image_id, clients.client[connection]->char_name);
                return NOT_FOUND;
            }

            //update the bag list to show that this bag was created by a char and not the system
            bag_list[*bag_id].connection=connection;
            bag_list[*bag_id].bag_type_id=token_image_id;

            return TRUE;
        }
    }

    return FALSE;
}

int add_item_to_bag(int bag_id, int image_id, int amount, int *bag_slot, struct ev_loop *loop){

    /*we separate this function from pick_up_from_bag so we can use it as a general function for adding stuff to
    the char inventory */

    ev_timer_stop(loop, &ev_bag_timer[bag_id]); //stop the poof timer

    //find a bag slot which already has this item in
    if(get_used_bag_slot(bag_id, image_id, bag_slot)==NOT_FOUND) {

        //if no slot has this item then place it in a new slot
        if(get_unused_bag_slot(bag_id, bag_slot)==NOT_FOUND) return FALSE;

        bag_list[bag_id].inventory[*bag_slot].image_id=image_id;
    }

    bag_list[bag_id].inventory[*bag_slot].amount+=amount;

    //reset the bag poof time
    ev_timer_init(&ev_bag_timer[bag_id], bag_poof_cb, 20, 0);
    ev_timer_start(loop, &ev_bag_timer[bag_id]);

    return TRUE;
}

int remove_item_from_bag(int bag_id, int image_id, int amount, int *bag_slot, struct ev_loop *loop){

    /*we separate this function from pick_up_from_bag so we can use it as a general function for adding stuff to
    the char inventory */

    ev_timer_stop(loop, &ev_bag_timer[bag_id]); //stop the poof timer

    //find a bag slot which already has this item in
    if(get_used_bag_slot(bag_id, image_id, bag_slot)==NOT_FOUND) {

        //if no slot has this item then abort
        return FALSE;
    }

    //check that the slot actually contains enough for the drop
    if(bag_list[bag_id].inventory[*bag_slot].amount < amount) {

        amount=bag_list[bag_id].inventory[*bag_slot].amount;
    }

    bag_list[bag_id].inventory[*bag_slot].amount-=amount;

    //reset the bag poof time
    ev_timer_init(&ev_bag_timer[bag_id], bag_poof_cb, 20, 0);
    ev_timer_start(loop, &ev_bag_timer[bag_id]);

    return TRUE;
}

int add_item_to_inventory(int connection, int image_id, int amount, int *inventory_slot){

    /*we separate this function from pick_up_from_bag so we can use it as a general function for adding stuff to
    the char inventory */

    //use the existing slot if we have this item in the inventory
    if(get_used_inventory_slot(connection, image_id, inventory_slot)!=FOUND){

        //otherwise find next free inventory slot
        if(get_unused_inventory_slot(connection, inventory_slot)==NOT_FOUND) return FALSE;

        //add image id to the inventory slot
        clients.client[connection]->client_inventory[*inventory_slot].image_id=image_id;

        #ifdef DEBUG
        printf("Add item [%i] to inventory for connection [%i]. Using new slot [%i]\n", image_id, connection, *inventory_slot);
        #endif
    }else {

        #ifdef DEBUG
        printf("Add item [%i] to inventory for connection [%i]. Using existing slot [%i]\n", image_id, connection, *inventory_slot);
        #endif
    }

    //send updated inventory_emu to client
    clients.client[connection]->inventory_emu += amount * item[image_id].emu;
    send_partial_stats(connection, INVENTORY_EMU,  clients.client[connection]->inventory_emu);

    //Add items to the char inventory
    clients.client[connection]->client_inventory[*inventory_slot].amount+=amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection]->client_inventory[*inventory_slot].amount, *inventory_slot);

    //update the database
    update_db_char_slot(connection, *inventory_slot);

    return TRUE;
}

int remove_item_from_inventory(int connection, int image_id, int amount, int slot){

        /*we separate this function from pick_up_from_bag so we can use it as a general function for removing stuff from
        the char inventory */

        //if slot == -1 then we need to search for a slot that contains image id
        if(slot==-1) {

            //find the slot with the item in it
            if(get_used_inventory_slot(connection, image_id, &slot)==NOT_FOUND){

                //if no slot contains the required item, we have an error situation
                log_event2(EVENT_ERROR, "tried to remove item image [%i] from inventory for character [%s] that does not contain that item", image_id, clients.client[connection]->char_name);
                return FALSE;
            }
        }

        //make sure that whats in the slot is greater than zero
        if(amount<1) {

            //if slot contains zero or a minus amount then we have an error situation
            log_event2(EVENT_ERROR, "tried to drop from a slot with zero or minus image [%i] from inventory for character [%s] that does not contain that item", image_id, clients.client[connection]->char_name);
            return FALSE;
        }

        //update char inventory and send to client
        clients.client[connection]->client_inventory[slot].amount -=amount;
        send_get_new_inventory_item(connection, image_id, clients.client[connection]->client_inventory[slot].amount, slot);

        //send updated emu to client
        clients.client[connection]->inventory_emu -=amount * item[image_id].emu;
        send_partial_stats(connection, INVENTORY_EMU,  clients.client[connection]->inventory_emu);

        //update char inventory on database
        update_db_char_slot(connection, slot);

        return TRUE;
}

void pick_up_from_bag(int connection, int bag_slot, struct ev_loop *loop){

    int bag_id=0, image_id=0, amount=0;
    int tile=clients.client[connection]->map_tile;
    char text_out[80]="";
    int max_emu =get_char_carry_capacity(connection) - get_inventory_emu(connection);
    int map_id=clients.client[connection]->map_id;
    int inventory_slot=0;

    //find the bag id in the bag array
    if(bag_exists(map_id, tile, &bag_id)==FALSE){

        sprintf(text_out, "%cSorry. No bag here for you to pick up", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    image_id=bag_list[bag_id].inventory[bag_slot].image_id;
    amount=bag_list[bag_id].inventory[bag_slot].amount;

    //reduce the amount to be loaded if this is greater than the remaining char carry capacity
    if(max_emu/item[image_id].emu < amount) amount=max_emu/item[image_id].emu;

    //if amount has been reduced to zero then the inventory must be overloaded
    if(amount==0){

        sprintf(text_out, "%cYou are overloaded", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //check that adding this item to the inventory won't exceed the available slots
    if(is_inventory_slots_exceeded(connection, image_id)==TRUE){

        //abort if inventory slots exceeded
        sprintf(text_out, "%cThere are no more available slots in your inventory to accept this item", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //remove item from the bag
    if(remove_item_from_bag(bag_id, image_id, amount, &bag_slot, loop)==FALSE){

        //abort function if we can't pick up that item from the bag
        sprintf(text_out, "%cSorry. we seem to have an error and can't remove that item from the bag", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //update the client bag inventory grid
    if(bag_is_empty(bag_id)==TRUE) {

        //if empty then poof the bag
        broadcast_bag_poof(bag_id, map_id);
    }
    else {

        //if bag has contents then send the revised bag inventory to the client
        send_get_new_ground_item(connection, image_id,  bag_list[bag_id].inventory[bag_slot].amount, bag_slot);
    }

    //add item to the inventory
    if(add_item_to_inventory(connection, image_id, amount, &inventory_slot)==FALSE){

        //abort function if we can't add that item to the inventory
        sprintf(text_out, "%cSorry. we seem to have an error and can't add that item to the inventory", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //update the client char inventory grid
    send_get_new_inventory_item(connection, image_id, clients.client[connection]->client_inventory[inventory_slot].amount, inventory_slot);
}

void drop_from_inventory(int connection, int inventory_slot, int drop_amount, struct ev_loop *loop){

    int bag_id=0, bag_slot=0;
    int map_id=clients.client[connection]->map_id;
    int tile=clients.client[connection]->map_tile;
    int image_id=clients.client[connection]->client_inventory[inventory_slot].image_id;
    char text_out[80]="";

    //make sure the amount in the packet is not greater than what is actually in the slot,
    if(clients.client[connection]->client_inventory[inventory_slot].amount < drop_amount){

        drop_amount=clients.client[connection]->client_inventory[inventory_slot].amount;
    }

    //check if there's already a bag at this location
    if(bag_exists(map_id, tile, &bag_id)==NOT_FOUND){

        if(create_empty_bag_from_token(connection, map_id, tile, &bag_id, loop)==FALSE) {

            //abort function if we can't create the bag
            sprintf(text_out, "%cSorry. Can't create that bag at the moment. Wait a few secs maybe ?", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);
            return;
        }
    }

    //add the drop to the bag
    if(add_item_to_bag(bag_id, image_id, drop_amount, &bag_slot, loop)==FALSE) {

        //abort function if we can't create the bag
        sprintf(text_out, "%cSorry. You've run out of bag slots", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //remove the drop from the inventory
    if(remove_item_from_inventory(connection, image_id, drop_amount, inventory_slot)==FALSE){

        //abort function if we can't remove the drop from the inventory
        sprintf(text_out, "%cSorry. we seem to have an error and can't drop that item", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //send the drop to the client
    send_get_new_ground_item(connection, image_id,  bag_list[bag_id].inventory[bag_slot].amount, bag_slot);
}
