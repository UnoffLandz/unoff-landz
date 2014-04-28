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

        packet[j+0]=clients.client[connection].client_inventory[i].image_id % 256; //image_id of item
        packet[j+1]=clients.client[connection].client_inventory[i].image_id / 256;

        packet[j+2]=clients.client[connection].client_inventory[i].amount % 256; //amount (when zero nothing is shown in inventory)
        packet[j+3]=clients.client[connection].client_inventory[i].amount / 256 % 256;
        packet[j+4]=clients.client[connection].client_inventory[i].amount / 256 / 256 % 256;
        packet[j+5]=clients.client[connection].client_inventory[i].amount / 256 / 256 / 256 % 256;

        packet[j+6]=i; //inventory pos (starts at 0)
        packet[j+7]=0; //flags
    }

    send(connection, packet, (MAX_INVENTORY_SLOTS*8)+4, 0);
}

int bag_count(int connection){

    //counts bags created by connection that are currently in operation

    int i=0;
    int count=0;

    for(i=0; i<MAX_BAGS; i++){

        if(bag_list[i].connection==connection && bag_list[i].status==USED) count++;
    }

    return count;
}


int is_inventory_slots_exceeded(int connection, int image_id){

    int i=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection].client_inventory[i].image_id==image_id) return FALSE;

        if(clients.client[connection].client_inventory[i].amount==0) return FALSE;
    }

    return TRUE;
}

int get_used_inventory_slot(int connection, int image_id, int *slot){

    int i;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection].client_inventory[i].image_id==image_id && clients.client[connection].client_inventory[i].amount>0) {

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

        if(clients.client[connection].client_inventory[i].amount==0) {

            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_char_carry_capacity(int connection){

    int race_id=clients.client[connection].race_type;
    int initial_carry_capacity=race[race_id].initial_carry_capacity;
    int carry_capacity_multiplier=race[race_id].carry_capacity_multiplier;

    return initial_carry_capacity + (carry_capacity_multiplier * clients.client[connection].physique);
}

int get_inventory_emu(int connection){

    int i=0;
    int total_emu=0;
    int image_id=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        image_id=clients.client[connection].client_inventory[i].image_id;
        total_emu +=(clients.client[connection].client_inventory[i].amount * item[image_id].emu);
    }

    return total_emu;
}

void send_get_new_bag(int connection, int bag_id){

    unsigned char packet[11];

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int x_pos=clients.client[connection].map_tile % map_axis;
    int y_pos=clients.client[connection].map_tile / map_axis;

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

        //printf("slot [%i] image [%i] amount [%i]\n", i, image_id, quantity);
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

    packet[9]=slot;

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

        //printf("bag id %i, slot %i amount %i\n", bag_id, i, bag_list[bag_id].inventory[i].amount );

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

        if(bag_list[bag_id].inventory[i].amount>0) {
             return FALSE;
        }
    }

    return TRUE;
}

int create_empty_bag(int map_id, int tile, int *bag_id, struct ev_loop *loop){

    //in order that both the system can use this function to create bags, it needs to avoid
    //returning messages to any particular connection.

    //get the bag id
    if(get_unused_bag(bag_id)==NOT_FOUND) {

        /*if function returns NOT_FOUND it means that the bag array has been exceeded and we cant therefore create
        the bag. Hence, we abort the function */
        return FALSE;
    }

    //set default bag parameters
    bag_list[*bag_id].bag_type_id=0; //set default bag type
    bag_list[*bag_id].connection=-1; //set default bag creator (system)
    bag_list[*bag_id].map_id=map_id;
    bag_list[*bag_id].tile_pos=tile;
    bag_list[*bag_id].status=USED;

    //broadcast the bag to all client connections in the vicinity
    broadcast_bag_drop(*bag_id, map_id);

    #ifdef DEBUG
        printf("Create new bag [%i] at position [%i] on map [%s]\n", *bag_id, tile, maps.map[map_id]->map_name);
    #endif

    //set poof timer
    ev_bag_timer[*bag_id].data=(int*) *bag_id;
    ev_timer_init(&ev_bag_timer[*bag_id], bag_poof_cb, 20, 0);
    ev_timer_start(loop, &ev_bag_timer[*bag_id]);

    return TRUE;
}

int is_item_a_bag_token(int image_id, int *bag_token_type){

    int i;

    for(i=0; i<MAX_BAG_TYPES; i++){

        if(image_id==bag_type[i].image_id) {

            *bag_token_type=i;
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

    //add amount to bag
    bag_list[bag_id].inventory[*bag_slot].amount+=amount;

    //reset the bag poof time
    ev_timer_init(&ev_bag_timer[bag_id], bag_poof_cb, bag_type[bag_list[bag_id].bag_type_id].poof_time, 0);
    ev_timer_start(loop, &ev_bag_timer[bag_id]);

    return TRUE;
}

int remove_item_from_bag(int bag_id, int image_id, int amount, int *bag_slot, struct ev_loop *loop){

    /*we separate this function from pick_up_from_bag so we can use it as a general function for adding stuff to
    the char inventory */

    int poof_time=0;
    int bag_type_id=0;

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

    //remove the item from the bag
    bag_list[bag_id].inventory[*bag_slot].amount-=amount;

    //get the poof time for the bag type
    bag_type_id=bag_list[bag_id].bag_type_id;
    poof_time=bag_type[bag_type_id].poof_time;

    //reset the bag poof time
    ev_timer_init(&ev_bag_timer[bag_id], bag_poof_cb, poof_time, 0);
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
        clients.client[connection].client_inventory[*inventory_slot].image_id=image_id;

        #ifdef DEBUG
        printf("Add item [%i] to inventory for connection [%i]. Using new slot [%i]\n", image_id, connection, *inventory_slot);
        #endif
    }else {

        #ifdef DEBUG
        printf("Add item [%i] to inventory for connection [%i]. Using existing slot [%i]\n", image_id, connection, *inventory_slot);
        #endif
    }

    //send updated inventory_emu to client
    clients.client[connection].inventory_emu += amount * item[image_id].emu;
    send_partial_stats(connection, INVENTORY_EMU,  clients.client[connection].inventory_emu);

    //Add items to the char inventory
    clients.client[connection].client_inventory[*inventory_slot].amount+=amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection].client_inventory[*inventory_slot].amount, *inventory_slot);

    //update the database
    update_db_char_slot(connection, *inventory_slot);

    return TRUE;
}

int remove_item_from_inventory(int connection, int image_id, int amount, int slot){

    /*we separate this function from pick_up_from_bag so we can use it as a general function for removing stuff from
    the char inventory */

    //update char inventory and send to client
    clients.client[connection].client_inventory[slot].amount -=amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection].client_inventory[slot].amount, slot);

    //send updated emu to client
    clients.client[connection].inventory_emu -=amount * item[image_id].emu;
    send_partial_stats(connection, INVENTORY_EMU,  clients.client[connection].inventory_emu);

    //update char inventory on database
    update_db_char_slot(connection, slot);

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

void bag_split(int connection, int bag_id){

    int i=0;
    int amount=0;
    int image_id=0;
    int inventory_slot=0;
    char text_out[80]="";
    int map_id=bag_list[bag_id].map_id;

    for(i=0; i<MAX_BAG_SLOTS; i++){

        amount=bag_list[bag_id].inventory[i].amount;

        if(amount>0){

            //get the image id and calculate the retrieved amount
            amount=rand() % amount;
            image_id=bag_list[bag_id].inventory[i].image_id;

            //zero the bag slot
            bag_list[bag_id].inventory[i].amount=0;
            bag_list[bag_id].inventory[i].image_id=0;

            //add item to the inventory
            if(add_item_to_inventory(connection, image_id, amount, &inventory_slot)==FALSE){

                //abort function if we can't add that item to the inventory
                sprintf(text_out, "%cSorry. we seem to have an error and can't add that item to the inventory", c_red1+127);
                send_raw_text_packet(connection, CHAT_SERVER, text_out);
                return;
            }

            //update the client char inventory grid
            send_get_new_inventory_item(connection, image_id, amount, inventory_slot);
        }
    }

    broadcast_bag_poof(bag_id, map_id);

    bag_list[bag_id].tile_pos=0;
    bag_list[bag_id].map_id=0;
    bag_list[bag_id].bag_type_id=0;
    bag_list[bag_id].status=UNUSED;
}

void pick_up_from_bag(int connection, int bag_slot, struct ev_loop *loop){

    int bag_id=0, image_id=0, amount=0;
    int tile=clients.client[connection].map_tile;
    char text_out[80]="";
    int inventory_emu=get_inventory_emu(connection);
    int max_emu =get_char_carry_capacity(connection) - inventory_emu;
    int map_id=clients.client[connection].map_id;
    int inventory_slot=0;
    int bag_type_id=0;
    int bag_type_max_emu=0;
    int bag_emu=0;

    //find the bag id in the bag array
    if(bag_exists(map_id, tile, &bag_id)==FALSE){

        sprintf(text_out, "%cSorry. No bag here for you to pick up", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    image_id=bag_list[bag_id].inventory[bag_slot].image_id;
    amount=bag_list[bag_id].inventory[bag_slot].amount;
    bag_type_id=bag_list[bag_id].bag_type_id;
    bag_type_max_emu=bag_type[bag_type_id].max_emu;

    //reduce the amount to be loaded if this is exceeds the char carry capacity
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

    //check for bag split
    bag_type_max_emu=bag_type[bag_list[bag_id].bag_type_id].max_emu;
    bag_emu=get_bag_inventory_emu(bag_id);

    if(bag_emu>bag_type_max_emu){

        //calculate chance of a bag split
        if(rand() % bag_type_max_emu < bag_emu-bag_type_max_emu){

            //ouch. a bag split has occured. Now all items in the bag are loaded back to the inventory
            //less a random amount for each slot
            bag_split(connection, bag_id);
            return;
        }
    }

    //remove item from the bag
    if(remove_item_from_bag(bag_id, image_id, amount, &bag_slot, loop)==FALSE){

        //abort function if we can't pick up that item from the bag
        sprintf(text_out, "%cSorry. we seem to have an error and can't remove that item from the bag", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);
        return;
    }

    //check if bag is empty
    if(bag_is_empty(bag_id)==TRUE) {

        //if empty then poof the bag
        broadcast_bag_poof(bag_id, map_id);
    }
    else {

        //if bag not empty then send the revised bag inventory to the client
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
    send_get_new_inventory_item(connection, image_id, clients.client[connection].client_inventory[inventory_slot].amount, inventory_slot);
}

void drop_from_inventory(int connection, int inventory_slot, int drop_amount, struct ev_loop *loop){

    int bag_id=0, bag_slot=0;
    int map_id=clients.client[connection].map_id;
    int tile=clients.client[connection].map_tile;
    int image_id=clients.client[connection].client_inventory[inventory_slot].image_id;
    char text_out[80]="";
    int bag_token=FALSE, bag_token_type=0;
    int inventory_amount=clients.client[connection].client_inventory[inventory_slot].amount;

    //anti bag spam prevents char having any more than 10 live bags
    if(bag_count(connection)+1==MAX_BAG_SPAM) {

        sprintf(text_out, "%cYou've created too many bags. Wait until some of them poof", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return;
    }

    /*determine if drop is a bag token and, the type of token, as we'll need this later on to prevent dropping of
    empty bags and, so we'll know when to upgrade a bags */
    bag_token=is_item_a_bag_token(image_id, &bag_token_type);
    printf("bag token [%i] [%i] [%i]\n", image_id, bag_token, bag_token_type);

    //make sure the amount in the packet is not greater than what is actually in the slot,
    if(inventory_amount < drop_amount){

        #ifdef DEBUG
        printf("reducing drop amount of [%i] to actual amount in char inventory [%i]\n", inventory_amount, drop_amount);
        #endif

        drop_amount=inventory_amount;
    }

    //check if there's already a bag at this location
    if(bag_exists(map_id, tile, &bag_id)==NOT_FOUND){

        //create a new bag

        if(bag_token==TRUE){

            /*abort function if the item being used to create the bag is a bag token otherwise, we'll end up creating
            an empty bag */
            sprintf(text_out, "%cSorry. You need to create the bag before you can use a bag token", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);
            return;
        }

        if(create_empty_bag(map_id, tile, &bag_id, loop)==FALSE){

            //abort function if we can't create the bag
            sprintf(text_out, "%cSorry. Can't create that bag at the moment. Wait a few secs and try again", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);
            return;
        }

        /*bag creation defaults to indicate bag was created by the system, hence lets update that value to show
        that its actually been created by a char */
        bag_list[bag_id].connection=connection;

        //set bag to default type
        bag_list[bag_id].bag_type_id=0;
    }

    //check if a valid bag token has been dropped
    if(bag_token==TRUE && bag_token_type>bag_list[bag_id].bag_type_id) {

        //upgrade the bag to the new bag token
        sprintf(text_out, "%cCongratulations. You just upgraded your bag to a %s", c_green1+127, bag_type[bag_token_type].bag_type_description);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        //remove the bag token from the inventory
        if(remove_item_from_inventory(connection, image_id, drop_amount, inventory_slot)==FALSE){

            //abort function if we can't remove the drop from the inventory
            sprintf(text_out, "%cSorry. we seem to have an error and can't drop that item", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);
            return;
        }

        //upgrade the bag
        bag_list[bag_id].bag_type_id=bag_token_type;

        //abort the function so as the bag token isn't added to the bag inventory
        return;
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

    //update the client bag inventory grid to indicate the new drop
    send_get_new_ground_item(connection, image_id,  bag_list[bag_id].inventory[bag_slot].amount, bag_slot);
}
