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

#ifndef CLIENTS_H_INCLUDED
#define CLIENTS_H_INCLUDED

#include <time.h>                   //support for time_t data type
#include <stdbool.h>                //support for boolean data type

#include "chat.h"                   //contains definition of MAX_CHAN_SLOTS
#include "character_inventory.h"    //contains definition of MAX_INVENTORY_SLOTS
#include "character_movement.h"     //contains definition of PATH_MAX

#define MAX_CLIENTS 100
#define BAG_LIST_MAX 10

struct client_node_type{

    enum{

        LOGGED_OUT=0,
        LOGGED_IN=1,
        CONNECTED=2
    }client_status;

    int packet_buffer[1024];
    int packet_buffer_length;

    time_t char_created;
    int character_id; //database id for char
    int char_age;//aggregate in-game time (measured in game minutes) for this character

    time_t session_commenced;
    time_t time_of_last_heartbeat;
    time_t time_of_last_minute;

    int path[PATH_MAX];
    int path_count;
    time_t time_of_last_move;

    bool harvest_flag;
    int harvest_amount;    //amount harvested each cycle
    int harvest_object_id; //object being harvested
    int harvest_interval;  //interval between each harvest
    int harvest_inventory_slot;
    time_t time_of_last_harvest;

    char ip_address[16];

    char char_name[80];
    char password[80];

    enum {

        CHAR_ALIVE,
        CHAR_DEAD,
        CHAR_BANNED
    }char_status;

    int active_chan;
    int chan[MAX_CHAN_SLOTS];       // chan0, chan1, chan2

    int gm_permission;
    int ig_permission;

    int map_id;
    int map_tile;

    int guild_id;
    time_t joined_guild;
    int guild_rank;

    int char_type;
    int skin_type;
    int hair_type;
    int shirt_type;
    int pants_type;
    int boots_type;
    int head_type;
    int shield_type;
    int weapon_type;
    int cape_type;
    int helmet_type;
    int frame;
    int max_health;
    int current_health;

    int physique_pp;
    int max_physique;
    int coordination_pp;
    int max_coordination;
    int reasoning_pp;
    int max_reasoning;
    int will_pp;
    int max_will;
    int instinct_pp;
    int max_instinct;
    int vitality_pp;
    int max_vitality;

    int human;
    int max_human;
    int animal;
    int max_animal;
    int vegetal;
    int max_vegetal;
    int inorganic;
    int max_inorganic;
    int artificial;
    int max_artificial;
    int magic;
    int max_magic;

    int manufacturing_lvl;
    int max_manufacturing_lvl;
    int harvest_lvl;
    int max_harvest_lvl;
    int alchemy_lvl;
    int max_alchemy_lvl;
    int overall_lvl;
    int max_overall_lvl;
    int attack_lvl;
    int max_attack_lvl;
    int defence_lvl;
    int max_defence_lvl;
    int magic_lvl;
    int max_magic_lvl;
    int potion_lvl;
    int max_potion_lvl;
    int material_pts;
    int max_material_pts;
    int ethereal_pts;
    int max_ethereal_pts;

    int food_lvl;

    int manufacture_exp;
    int max_manufacture_exp;
    int harvest_exp;
    int max_harvest_exp;
    int alchemy_exp;
    int max_alchemy_exp;
    int overall_exp;
    int max_overall_exp;
    int attack_exp;
    int max_attack_exp;
    int defence_exp;
    int max_defence_exp;
    int magic_exp;
    int max_magic_exp;
    int potion_exp;
    int max_potion_exp;

    int book_id;
    int max_book_time;
    int elapsed_book_time;

    struct client_inventory_type inventory[MAX_INVENTORY_SLOTS];

    bool bag_open;
    int open_bag_id;
};

struct client_list_type {

    int client_count;
    struct client_node_type client[MAX_CLIENTS];
};
extern struct client_list_type clients;


/** RESULT   : closes a client connection

    RETURNS  : void

    PURPOSE  : used in socket_accept_callback and socket_read_callback

    NOTES    :
**/
void close_connection_slot(int connection);

#endif // CLIENTS_H_INCLUDED
