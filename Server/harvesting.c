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

#include <stdio.h> //support for sprintf

#include "clients.h"
#include "colour.h"
#include "server_messaging.h"
#include "logging.h"
#include "map_object.h"
#include "harvesting.h"
#include "objects.h"
#include "server_protocol_functions.h"
#include "e3d.h"

#define DEBUG_HARVESTING 0

void process_char_harvest(int actor_node, time_t current_time){

    /** public function - see header **/

    int socket=clients.client[actor_node].socket;

    if(clients.client[actor_node].harvest_flag==true){

        //get the harvest interval for item
        int object_id=clients.client[actor_node].harvest_object_id;
        int harvest_interval=object[object_id].harvest_interval;

        if(clients.client[actor_node].time_of_last_harvest + harvest_interval < current_time){

            //add item to the char inventory
            int slot=clients.client[actor_node].harvest_inventory_slot;
            int amount=clients.client[actor_node].harvest_amount;

            if(add_to_inventory(actor_node, object_id, amount, slot)==false){

                send_text(socket, CHAT_SERVER, "%cYou exceeded your maximum inventory EMU!", c_red3+127);

                stop_harvesting(actor_node);
                return;
            }

            //update time of last harvest
            clients.client[actor_node].time_of_last_harvest=current_time;
        }
    }
}

void stop_harvesting(int actor_node){

    /** public function - see header */

    int socket=clients.client[actor_node].socket;

    clients.client[actor_node].harvest_flag=false;

    //The the OL/EL client relies on the phrase 'You stopped harvesting' to stop the harvesting
    //effect. Hence, we can't implement a better statement such as 'You stopped harvesting [item]'
    send_text(socket, CHAT_SERVER, "%cYou stopped harvesting.", c_green3+127);

    int object_id=clients.client[actor_node].harvest_object_id;
    log_text(EVENT_SESSION, " char [%s] stopped harvesting [%s]", clients.client[actor_node].char_name, object[object_id].object_name);
}


void start_harvesting(int actor_node, int threed_object_list_pos){

    /** public function - see header */
    int socket=clients.client[actor_node].socket;

    //get the id for the object being harvested
    int map_id=clients.client[actor_node].map_id;
    int object_id=get_object_id(map_id, threed_object_list_pos);

    //determine if the object is close enough for the char to harvest
    int object_tile=get_object_tile(map_id, threed_object_list_pos);
    int object_proximity=get_proximity(clients.client[actor_node].map_tile, object_tile, maps.map[map_id].map_axis);

    if(object_proximity>MIN_HARVEST_PROXIMITY){

        send_text(socket, CHAT_SERVER, "%cYou are too far away to harvest that item!", c_red3+127);
        return;
    }

    //check if item is harvestable
    if(object[object_id].harvestable==false){

        send_text(socket, CHAT_SERVER, "%cItem isn't harvestable!", c_red3+122);
        return;
    }

    //check that we have an existing or free slot
    int slot=find_inventory_slot(actor_node, object_id);

    if(slot==-1){

        send_text(socket, CHAT_SERVER, "%cYou have no free slots in your inventory!. ", c_red3+127);
        return;
    }

    send_text(socket, CHAT_SERVER, "%cYou started to harvest %s!. ", c_green3+127, object[object_id].object_name);

    //set the chars harvest flag to show that it is now harvesting and set the item
    clients.client[actor_node].harvest_flag=true;

    //record what item the char is harvesting
    clients.client[actor_node].harvest_object_id=object_id;

    //set the char inventory slot
    clients.client[actor_node].harvest_inventory_slot=slot;

    //set the amount to be harvested each cycle
    clients.client[actor_node].harvest_amount=1; //default amount

    //set the harvest interval
    clients.client[actor_node].harvest_interval=object[object_id].harvest_interval;

    log_text(EVENT_SESSION, "char [%s] started harvesting [%s]", clients.client[actor_node].char_name, object[object_id].object_name);
}

