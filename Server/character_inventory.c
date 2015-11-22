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

struct client_inventory_type client_inventory;

int get_max_inventory_emu(int connection){

    /** public function - see header */

    int pick_points=clients.client[connection].physique_pp;
    int race_id=get_char_race_id(connection);

    return attribute[race_id].carry_capacity[pick_points];
}


int get_inventory_emu(int connection){

    /** public function - see header **/

    int total_emu=0;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        int object_id=clients.client[connection].inventory[i].object_id;
        total_emu +=(clients.client[connection].inventory[i].amount * item[object_id].emu);
     }

    return total_emu;
}


int find_inventory_slot(int connection, int object_id){

    /** public function - see header **/

    //find an existing inventory slot or next empty slot or return -1 if no slots available
    int slot=-1;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection].inventory[i].object_id==0 && slot==-1) {

            slot=i;
        }

        if(clients.client[connection].inventory[i].object_id==object_id) {

            return i;
        }
    }

    return slot;
}


bool add_to_inventory(int connection, int object_id, int amount, int slot){

    /** public function - see header */

    //check inventory max emu not exceeded
    int inventory_emu=get_inventory_emu(connection);
    int max_inventory_emu=get_max_inventory_emu(connection);
    if(inventory_emu+(amount * object[object_id].emu)>max_inventory_emu) return false;

    //add the item to the inventory
    clients.client[connection].inventory[slot].amount+=amount;
    clients.client[connection].inventory[slot].object_id=object_id;

    //update the client inventory
    send_get_new_inventory_item( connection, object_id, clients.client[connection].inventory[slot].amount, slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].inventory[slot].amount, clients.client[connection].character_id, slot);

    return true;
}


int remove_from_inventory(int connection, int object_id, int amount, int slot){

    /** public function - see header */

    //ensure we don't remove more than is in slot
    if(amount>clients.client[connection].inventory[slot].amount) {

        amount=clients.client[connection].inventory[slot].amount;
    }

    //remove amount from slot
    clients.client[connection].inventory[slot].amount-=amount;

    //if slot is empty remove object clear slot
    if(clients.client[connection].inventory[slot].amount==0){

        clients.client[connection].inventory[slot].object_id=0;
    }

    //update the client inventory
    send_get_new_inventory_item( connection, object_id, clients.client[connection].inventory[slot].amount, slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].inventory[slot].amount, clients.client[connection].character_id, slot);

    return amount;
}


void move_inventory_item(int connection, int from_slot, int to_slot){

    /** public function - see header */

    int object_id=clients.client[connection].inventory[from_slot].object_id;
    int amount=clients.client[connection].inventory[from_slot].amount;

    //zero the 'from slot'
    clients.client[connection].inventory[from_slot].object_id=0;
    clients.client[connection].inventory[from_slot].amount=0;
    send_get_new_inventory_item(connection, 0, 0, from_slot);

    //place item in the 'to slot'
    clients.client[connection].inventory[to_slot].object_id=object_id;
    clients.client[connection].inventory[to_slot].amount=amount;
    send_get_new_inventory_item(connection, object_id, amount, to_slot);

    //update_database
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].inventory[from_slot].amount, clients.client[connection].character_id, from_slot);
    push_sql_command("UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].inventory[to_slot].amount, clients.client[connection].character_id, to_slot);

}
