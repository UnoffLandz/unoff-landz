/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/
#include <stdio.h> // testing only

#include "clients.h"
#include "character_race.h"
#include "items.h"
#include "character_type.h"
#include "attributes.h"
#include "objects.h"
#include "server_protocol_functions.h"
#include "colour.h"
#include "server_messaging.h"
#include "db/database_functions.h"
#include "idle_buffer2.h"
#include "logging.h"
#include "server_start_stop.h"
#include "bags.h"
#include "broadcast_actor_functions.h"

struct client_inventory_type client_inventory;

int get_max_inventory_emu(int actor_node){

    /** public function - see header */

    int pick_points=clients.client[actor_node].physique_pp;
    int race_id=get_char_race_id(actor_node);

    return attribute[race_id].carry_capacity[pick_points];
}


int get_inventory_emu(int actor_node){

    /** public function - see header **/

    int total_emu=0;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        int object_id=clients.client[actor_node].inventory[i].object_id;
        total_emu +=(clients.client[actor_node].inventory[i].amount * item[object_id].emu);
     }

    return total_emu;
}


int find_inventory_slot(int actor_node, int object_id){

    /** public function - see header **/

    //find an existing inventory slot or next empty slot or return -1 if no slots available
    int slot=-1;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        //find the first empty slot
        if(clients.client[actor_node].inventory[i].amount==0 && slot==-1) slot=i;

        //find slot with the object
        if(clients.client[actor_node].inventory[i].object_id==object_id
        && clients.client[actor_node].inventory[i].amount>0) return i;
    }

    return slot;
}


int item_in_inventory(int actor_node, int object_id){

    /** public function - see header **/

    //find an existing inventory slot or return -1
    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        //fund slot with the object
        if(clients.client[actor_node].inventory[i].object_id==object_id
        && clients.client[actor_node].inventory[i].amount>0) return i;
    }

    return -1;
}


bool add_to_inventory(int actor_node, int object_id, int amount, int slot){

    /** public function - see header */

    //check inventory max emu not exceeded
    int inventory_emu=get_inventory_emu(actor_node);
    int max_inventory_emu=get_max_inventory_emu(actor_node);
    if(inventory_emu+(amount * object[object_id].emu)>max_inventory_emu) return false;

    //add the item to the inventory
    clients.client[actor_node].inventory[slot].amount+=amount;
    clients.client[actor_node].inventory[slot].object_id=object_id;

    //update the client inventory
    int socket=clients.client[actor_node].socket;
    send_get_new_inventory_item(socket, object_id, clients.client[actor_node].inventory[slot].amount, slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[actor_node].inventory[slot].amount, clients.client[actor_node].character_id, slot);

    return true;
}


int remove_from_inventory(int actor_node, int object_id, int amount, int slot){

     /** public function - see header */

     //ensure we don't remove more than is in slot
    if(amount>clients.client[actor_node].inventory[slot].amount) {

        amount=clients.client[actor_node].inventory[slot].amount;
    }

    //remove amount from slot
    clients.client[actor_node].inventory[slot].amount-=amount;

    //if slot is empty remove object clear slot
    if(clients.client[actor_node].inventory[slot].amount==0){

        clients.client[actor_node].inventory[slot].object_id=0;
    }

    //update the client inventory
    int socket=clients.client[actor_node].socket;
    send_get_new_inventory_item(socket, object_id, clients.client[actor_node].inventory[slot].amount, slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[actor_node].inventory[slot].amount, clients.client[actor_node].character_id, slot);

    return amount;
}


void move_inventory_item(int actor_node, int from_slot, int to_slot){

    /** public function - see header */

    int object_id=clients.client[actor_node].inventory[from_slot].object_id;
    int amount=clients.client[actor_node].inventory[from_slot].amount;
    int socket=clients.client[actor_node].socket;

    //zero the 'from slot'
    clients.client[actor_node].inventory[from_slot].object_id=0;
    clients.client[actor_node].inventory[from_slot].amount=0;
    send_get_new_inventory_item(socket, 0, 0, from_slot);  // TODO (themuntdregger#1#): replace with REMOVE_ITEM_FROM_INVENTORY protocol

    //place item in the 'to slot'
    clients.client[actor_node].inventory[to_slot].object_id=object_id;
    clients.client[actor_node].inventory[to_slot].amount=amount;
    send_get_new_inventory_item(socket, object_id, amount, to_slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[actor_node].inventory[from_slot].amount, clients.client[actor_node].character_id, from_slot);
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[actor_node].inventory[to_slot].amount, clients.client[actor_node].character_id, to_slot);
}


void drop_from_inventory_to_bag(int actor_node, int inventory_slot, int withdraw_amount){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    //determine the item to be dropped
    int item_id=clients.client[actor_node].inventory[inventory_slot].object_id;

    int bag_id;
    int bag_slot=-1;

    if(clients.client[actor_node].bag_open==true){//use existing bag

        //find the existing bag id
        bag_id=clients.client[actor_node].open_bag_id;

        //find a slot in which to place the item
        bag_slot=find_bag_slot(bag_id, item_id);

        //check if max bag slots exceeded
        if(bag_slot==-1){

            send_text(socket, CHAT_SERVER, "%cthere are no slots left in this bag", c_red3+127);
            return;
        }
    }
    else {// create a new bag

        // get new bag id
        bag_id=create_bag(clients.client[actor_node].map_id, clients.client[actor_node].map_tile);

        //check if max bags permitted by server has been exceeded
        if(bag_id==-1){

            send_text(socket, CHAT_SERVER, "%cThe server has reached the maximum number of bags. Wait for one to poof! ", c_red3+127);
            return;
        }

        //broadcast the bag drop
        broadcast_get_new_bag_packet(actor_node, bag_id);

        //place item in first slot of new bag
        bag_slot=0;
    }

    //update char to show that it is standing on an open bag
    clients.client[actor_node].bag_open=true;
    clients.client[actor_node].open_bag_id=bag_id;

    //remove item from char inventory
    int amount_withdrawn=remove_from_inventory(actor_node, item_id, withdraw_amount, inventory_slot);

    //add to bag
    int amount_added=add_to_bag(bag_id, item_id, amount_withdrawn, bag_slot);

    //catch if amount added to bag is less than amount withdrawn from inventory
    if(amount_added != amount_withdrawn){

        log_event(EVENT_ERROR, "char [%s] error dropping item from inventory", clients.client[actor_node].char_name);
        log_text(EVENT_ERROR, "item [%s]", object[item_id].object_name);
        log_text(EVENT_ERROR, "amount withdrawn from inventory [%i]", amount_withdrawn);
        log_text(EVENT_ERROR, "amount added to bag [%i]", amount_added);

        stop_server();
    }

    //send revised char inventory and bag inventory to client
    send_here_your_inventory(socket);
    send_here_your_ground_items(socket, bag_id);
}

void pick_up_from_bag_to_inventory(int actor_node, int bag_slot, int amount, int bag_id){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    //adds item to char inventory
    int object_id=bag[bag_id].inventory[bag_slot].object_id;
    int inventory_slot=find_inventory_slot(actor_node, object_id);

    if(add_to_inventory(actor_node, object_id, amount, inventory_slot)==false){

        send_text(socket, CHAT_SERVER, "%cyour maximum carry capacity has been exceeded", c_red1+127);
        return;
    }

    //removes item from bag
    remove_item_from_bag(bag_id, amount, bag_slot);
    send_here_your_ground_items(socket, bag_id);

    if(bag_empty(bag_id)==true){

       // destroy bag
       broadcast_destroy_bag_packet(bag_id);
    }
}
