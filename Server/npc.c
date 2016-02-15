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

#include <stdbool.h> // support for bool data type
#include <stdio.h> // support for sprintf function
#include <string.h> //support for memset function

#include "npc.h"
#include "clients.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "objects.h"
#include "boats.h"
#include "game_data.h"
#include "maps.h"
#include "date_time_functions.h"

struct npc_trigger_type npc_trigger[MAX_NPC_TRIGGERS];
struct npc_action_type npc_action[MAX_NPC_ACTIONS];


void npc_give_sale_options(int actor_node, int npc_actor_node, int action_node){

    /** public function - see header **/

    char text_out[160]="";
    int socket=clients.client[actor_node].socket;

    //clear the section of the client struct that will contain the price options
    memset(&clients.client[actor_node].npc_option, 0, sizeof(clients.client[actor_node].npc_option));

    //clear the npc options list
    send_npc_options_list(socket, npc_actor_node, "");

    int object_id_required=npc_action[action_node].object_id_required;
    int amount_required=npc_action[action_node].object_amount_required;
    int amount_given=npc_action[action_node].object_amount_given;

    //check that char has the required object
    int slot=item_in_inventory(actor_node, object_id_required, CARRY_SLOTS);

    if(slot==-1){

        sprintf(text_out, "Sorry %s.\n\nI'm buying %s but you don't appear to have any.", clients.client[actor_node].char_name, object[object_id_required].object_name );
        send_npc_info(socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);
        send_npc_text(socket, text_out);
        return;
    }

    char option_str[1024]="";
    sprintf(option_str, "%s%d;", option_str, (int)(amount_required*1));
    sprintf(option_str, "%s%d;", option_str, (int)(amount_required*5));
    sprintf(option_str, "%s%d;", option_str, (int)(amount_required*10));
    sprintf(option_str, "%s%d;", option_str, (int)(amount_required*15));
    sprintf(option_str, "%s%d;", option_str, (int)(amount_required*20));

    //keep a note of sale list options so we can refer back when responding to NPC
    clients.client[actor_node].npc_option[0].amount=amount_required*1;
    clients.client[actor_node].npc_option[1].amount=amount_required*5;
    clients.client[actor_node].npc_option[2].amount=amount_required*10;
    clients.client[actor_node].npc_option[3].amount=amount_required*15;
    clients.client[actor_node].npc_option[4].amount=amount_required*20;

    clients.client[actor_node].npc_option[0].price=amount_given*1;
    clients.client[actor_node].npc_option[1].price=amount_given*5;
    clients.client[actor_node].npc_option[2].price=amount_given*10;
    clients.client[actor_node].npc_option[3].price=amount_given*15;
    clients.client[actor_node].npc_option[4].price=amount_given*20;

    //retain details of the action node so we can refer back when responding to NPC
    clients.client[actor_node].action_node=action_node;

    //send npc communication
    int object_id_given=npc_action[action_node].object_id_given;

    sprintf(text_out, "Hello %s.\n\nI am buying %i %s for 1 %s. How many would you like to sell to me?", clients.client[actor_node].char_name,  npc_action[action_node].object_amount_required, object[object_id_required].object_name, object[object_id_given].object_name);
    send_npc_text(socket, text_out);
    send_npc_info(socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);
    send_npc_options_list(socket, npc_actor_node, option_str);

    //update time_check struct (used to put a time limit on the char's response)
    gettimeofday(&time_check, NULL);
    clients.client[actor_node].npc_choice_time=time_check.tv_sec;
}


void npc_sell_object(int actor_node, int npc_actor_node, int choice){

    /** public function - see header **/

    char text_out[160]="";
    int action_node=clients.client[actor_node].action_node;
    int socket=clients.client[actor_node].socket;

    //clear the options list
    send_npc_options_list(socket, npc_actor_node, "");

    //check the timeout
    gettimeofday(&time_check, NULL);

    if(clients.client[actor_node].npc_choice_time + NPC_OPTION_TIMEOUT < time_check.tv_sec){

        send_npc_info(clients.client[actor_node].socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);
        sprintf(text_out, "%cSorry %s. You took too long to decide. Try again", 127+c_red3, clients.client[actor_node].char_name);
        send_npc_text(clients.client[actor_node].socket, text_out);
        return;
    }

    //get cost of object to be purchased
    int referring_action_node=clients.client[actor_node].action_node;

    int object_amount_required=clients.client[actor_node].npc_option[choice].amount;
    int object_id_required=npc_action[referring_action_node].object_id_required;
    int item_required_slot=item_in_inventory(actor_node, object_id_required, CARRY_SLOTS);

    //check actor has payment for the object to be purchased
    if(item_required_slot==-1 || clients.client[actor_node].inventory[item_required_slot].amount < object_amount_required){

        sprintf(text_out, "Sorry %s.\n\nYou don't have enough %s to buy that amount.", clients.client[actor_node].char_name, object[object_id_required].object_name);
        send_npc_info(clients.client[actor_node].socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);
        send_npc_text(socket, text_out);
        return;
    }

    //check actor has room in inventory for the object to be purchased
    int object_amount_given=clients.client[actor_node].npc_option[choice].price;
    int object_id_given=npc_action[referring_action_node].object_id_given;
    int item_given_slot=find_inventory_slot(actor_node, object_id_given, CARRY_SLOTS);

    //check actor has room for payment item
    if(item_given_slot==-1){

        sprintf(text_out, "Sorry %s.\n\nYou don't have room for....", clients.client[actor_node].char_name);
        send_npc_info(clients.client[actor_node].socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);
        send_npc_text(socket, text_out);
        return;
    }

    //add purchased object to and remove payment from the actors inventory
    add_to_inventory(actor_node, object_id_given, object_amount_given, item_given_slot);
    remove_from_inventory(actor_node, object_id_required, object_amount_required, item_required_slot);

    //repeat sale options
    npc_give_sale_options(actor_node, npc_actor_node, action_node);
}


void npc_give_boat_options(int actor_node, int npc_actor_node, int action_node){

    /** public function - see header **/

    //clear the section of the client struct that will contain the boat schedule
    memset(&clients.client[actor_node].npc_option, 0, sizeof(clients.client[actor_node].npc_option));

    char boat_schedule_str[1024]="";

    //search for boats leaving from the actors current map
    for(int i=0; i<MAX_BOATS; i++){

        if(boat[i].departure_map_id==clients.client[npc_actor_node].map_id){

            int k=0;

            //get the next three sailing times for each boat
            int boat_node=npc_action[action_node].boat_node;
            int object_id_required=boat[boat_node].boat_payment_object_id;

            for(int j=0; j<3; j++){

                clients.client[actor_node].npc_option[k].boat_node=i;

                //calculate the departure time in minutes
                int start_time=game_data.game_minutes / (boat[i].travel_time*2) * (boat[i].travel_time*2);
                clients.client[actor_node].npc_option[k].boat_departure_time=start_time + ((j+1)* boat[i].travel_time *2);
                clients.client[actor_node].npc_option[k].price=boat[i].boat_payment_price *((3-j) * 2);

                //create boat schedule string
                sprintf(boat_schedule_str, "%s%s departing at %i:%02i - price %i %s;",
                    boat_schedule_str,
                    maps.map[boat[i].destination_map_id].map_name,
                    clients.client[actor_node].npc_option[k].boat_departure_time / 60,
                    clients.client[actor_node].npc_option[k].boat_departure_time % 60,
                    clients.client[actor_node].npc_option[k].price,
                    object[object_id_required].object_name);

                k++;
            }
        }
    }

    //send boat schedule to the client
    int socket=clients.client[actor_node].socket;

    send_npc_options_list(socket, npc_actor_node, boat_schedule_str);
    send_npc_info(socket, clients.client[npc_actor_node].char_name, clients.client[npc_actor_node].portrait_id);

    char text_out[160]="";
    int map_id=clients.client[npc_actor_node].map_id;
    sprintf(text_out, "Hello %s.\n\nI can offer you passage on the following boats leaving %s", clients.client[actor_node].char_name, maps.map[map_id].map_name);
    send_npc_text(socket, text_out);

    //update time_check struct (used to put a time limit on the char's response)
    gettimeofday(&time_check, NULL);
    clients.client[actor_node].npc_choice_time=time_check.tv_sec;
}

void npc_sell_boat_ticket(int actor_node, int npc_actor_node, int ticket_node){

  /** public function - see header */

    char text_out[160]="";
    int socket=clients.client[actor_node].socket;
    int boat_node=clients.client[actor_node].npc_option[ticket_node].boat_node;

    //clear the npc options list
    send_npc_options_list(socket, npc_actor_node, "");

    //get ticket price details
    int object_amount_required=clients.client[actor_node].npc_option[ticket_node].price;
    int object_id_required=boat[boat_node].boat_payment_object_id;

    //check that char has the required payment item
    int slot=find_inventory_slot(actor_node, object_id_required, CARRY_SLOTS);

    if(slot==-1 || clients.client[actor_node].inventory[slot].amount < object_amount_required ){

        sprintf(text_out, "Sorry %s.\n\nYou don't have enough %s to buy that ticket.", clients.client[actor_node].char_name, object[object_id_required].object_name);
        send_npc_text(socket, text_out);
        return;
    }

    //remove payment from the chars inventory
    remove_from_inventory(actor_node, object_id_required, object_amount_required, slot);

    //set departure time
    clients.client[actor_node].boat_booked=true;
    clients.client[actor_node].boat_node=clients.client[actor_node].npc_option[ticket_node].boat_node;
    clients.client[actor_node].boat_departure_time=clients.client[actor_node].npc_option[ticket_node].boat_departure_time;

    //communicate to char
    int map_id=boat[boat_node].destination_map_id;

    sprintf(text_out, "Thanks %s.\n\nYou've just bought your ticket on the %02i:%02i boat to %s. Be sure not to miss it as I don't give refunds.",
    clients.client[actor_node].char_name,
    clients.client[actor_node].boat_departure_time / 60,
    clients.client[actor_node].boat_departure_time % 60,
    maps.map[map_id].map_name);

    send_npc_text(socket, text_out);
}
