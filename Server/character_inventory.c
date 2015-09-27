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

        int object_id=clients.client[connection].client_inventory[i].object_id;
        total_emu +=(clients.client[connection].client_inventory[i].amount * item[object_id].emu);
     }

    return total_emu;
}


void add_item_to_inventory(int connection, int object_id, int amount){

    /** public function - see header */

    int slot=0;

    bool existing_item=false;
    bool new_slot=false;

    // check if we already have item in inventory
    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        if(clients.client[connection].client_inventory[i].object_id==object_id){

            existing_item=true;
            slot=i;
            break;
        }
    }

    // if we don't have item in inventory, find the next spare slot
    if(existing_item==false){

        for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

            if(clients.client[connection].client_inventory[i].object_id==0){

                new_slot=true;
                slot=i;
                break;
            }
        }

        //if we can't find space slot, inform client and abort
        if(new_slot==false){

            send_text(connection, CHAT_SERVER, "%cYou have no room in your inventory for any new items.", c_red3+127);
            return;
        }
    }

    //add the item to the inventory
    clients.client[connection].client_inventory[slot].amount+=amount;
    clients.client[connection].client_inventory[slot].object_id=object_id;

    //update the client inventory
    send_get_new_inventory_item( connection, object_id, clients.client[connection].client_inventory[slot].amount, slot);

    //update_database
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].client_inventory[slot].amount, clients.client[connection].character_id, slot);
    push_sql_command(sql);
}


void move_inventory_item(int connection, int from_slot, int to_slot){

    /** public function - see header */

    int object_id=clients.client[connection].client_inventory[from_slot].object_id;
    int amount=clients.client[connection].client_inventory[from_slot].amount;

    //zero the 'from slot'
    clients.client[connection].client_inventory[from_slot].object_id=0;
    clients.client[connection].client_inventory[from_slot].amount=0;
    send_get_new_inventory_item(connection, 0, 0, from_slot);

    /* we ought to need the following, but the OL/EL client already checks for it
    //check the 'to' slot to make sure it's empty
    if(clients.client[connection].client_inventory[from_slot].amount>0){

        char text_out[80]="";
        sprintf(text_out, "%cThat inventory slot is occupied.", c_red3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);
        return;
    }
    */

    //place item in the 'to slot'
    clients.client[connection].client_inventory[to_slot].object_id=object_id;
    clients.client[connection].client_inventory[to_slot].amount=amount;
    send_get_new_inventory_item(connection, object_id, amount, to_slot);

    //update_database
    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].client_inventory[from_slot].amount, clients.client[connection].character_id, from_slot);
    push_sql_command(sql);

    snprintf(sql, MAX_SQL_LEN, "UPDATE INVENTORY_TABLE SET IMAGE_ID=%i, AMOUNT=%i WHERE CHAR_ID=%i AND SLOT=%i", object_id, clients.client[connection].client_inventory[to_slot].amount, clients.client[connection].character_id, to_slot);
    push_sql_command(sql);
}

void remove_item_from_inventory(){

}
