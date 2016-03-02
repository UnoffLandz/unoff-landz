/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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

#include <stdbool.h> //support boolean datatype
#include <string.h> //support memset function

#include "bags.h"
#include "clients.h"
#include "date_time_functions.h"
#include "objects.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"

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

        int object_id=bag[bag_id].inventory[i].object_id;
        bag_emu+=bag[bag_id].inventory[i].amount * object[object_id].emu;
    }

    return bag_emu;
}


void broadcast_get_new_bag_packet(int actor_node, int bag_id) {

    /** public function - see header */

    //pre-create the packet so we don't have to repeat this on each occasion when it needs to
    //sent to other actors
    unsigned char packet[MAX_PACKET_SIZE]={0};
    size_t packet_length=0;
    int socket=clients.client[actor_node].socket;

    get_new_bag_packet(socket, bag_id, packet, &packet_length);

    //cycle through all the clients
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //restrict to chars on the same map as broadcasting char
            if(bag[bag_id].map_id==clients.client[i].map_id){

                send_packet(clients.client[i].socket, packet, packet_length);
            }
        }
    }
}


void broadcast_destroy_bag_packet(int bag_id) {

    /** public function - see header */

    //cycle through all the clients
    for(int i=0; i<MAX_ACTORS; i++){

        //restrict to actors who are players
        if(clients.client[i].client_node_status==CLIENT_NODE_USED
        && clients.client[i].player_type==PLAYER){

            //restrict to chars on the same map as bag
            if(bag[bag_id].map_id==clients.client[i].map_id){

                //if client has bag grid open then close it
                if(clients.client[i].open_bag_id==bag_id){

                    clients.client[i].open_bag_id=0;
                    clients.client[i].bag_open=false;

                    send_close_bag(clients.client[i].socket);
                }

                send_destroy_bag(clients.client[i].socket, bag_id);
            }
        }
    }

    //clear bag contents
    memset(&bag[bag_id], 0, sizeof(bag[bag_id]));
}

