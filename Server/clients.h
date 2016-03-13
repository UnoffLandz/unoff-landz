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

#ifndef CLIENTS_H_INCLUDED
#define CLIENTS_H_INCLUDED

#include <time.h>                   //support for time_t data type
#include <stdbool.h>                //support for boolean data type
#include <unistd.h>                 //support for ssize_t data type
#include <stdint.h>                 //support for uint16_t data type

#include "chat.h"                   //contains definition of MAX_CHAN_SLOTS
#include "character_inventory.h"    //contains definition of MAX_INVENTORY_SLOTS
#include "character_movement.h"     //contains definition of PATH_MAX
#include "packet.h"
#include "characters.h"

#define MAX_CHAR_NAME_LEN 40
#define MAX_CHAR_PASSWORD_LEN 40

#define MAX_ACTORS 100
#define MAX_SOCKETS 50

#define ACTOR_CONTACT_PROXIMITY 2    //max distance at which actor can touch or harvest

struct client_socket_type{

    int actor_node;// link to the actor node

    enum {// records whether a socket node is being used and its status

        SOCKET_UNUSED=0,
        CLIENT_CONNECTED=1,
        CLIENT_LOGGED_IN=2,
    }socket_node_status;

    bool kill_connection; //used to signal to the timeout callback that the actor socket
                          //should be closed

    unsigned char packet_buffer[MAX_PACKET_SIZE * 2]; //used to store the socket receive data and
    size_t packet_buffer_length;//       measure it's length

    time_t time_of_last_heartbeat; //tracks the time the client last communicated with
                                   //with the server and whether the connection should
                                   //be treated as having lagged out.

    char ip_address[16]; //records the ip address through which the client has connected
                         //with the server

    //client version data
    int client_version_first_digit;
    int client_version_second_digit;
    int client_version_major;
    int client_version_minor;
    int client_version_release;
    int client_version_patch;
    int client_version_port;
    unsigned char client_version_ip_address[4];
    //OL extra packet data
    int client_version_os_type;
    uint16_t client_version_ol_ver_flag;

};
extern struct client_socket_type client_socket[MAX_SOCKETS];


struct client_node_type{// TODO (themuntdregger#1#): convert struct name to reflect actors rather than clients

    enum{//describes if this node is available or being used.

        CLIENT_NODE_UNUSED=0,
        CLIENT_NODE_USED=1,
    }client_node_status;

    enum{//describes the actor controller

        ACTOR_NONE=0,
        PLAYER=1,
        NPC=2,
        //ACTOR_COMPUTER_NON_PK
        //ACTOR_HUMAN_PK
        //ACTOR_COMPUTER_PK
    }player_type;

    int socket;

    time_t char_created;
    int character_id; //database id for char
    int char_age;//aggregate in-game time (measured in game minutes) for this character

    time_t session_commenced;
    time_t time_of_last_minute;

    int path[PATH_MAX];
    int path_count;
    time_t time_of_last_move;

    bool harvest_flag;
    int harvest_amount;    //amount harvested each cycle
    int harvest_object_id; //object being harvested
    int harvest_interval;  //inter val between each harvest
    int harvest_inventory_slot; //slot to which harvest item is to be placed
    time_t time_of_last_harvest; //time item last harvested

    char char_name[MAX_CHAR_NAME_LEN];
    char password[MAX_CHAR_PASSWORD_LEN];

    enum {

        CHAR_ALIVE,
        CHAR_DEAD,
        CHAR_BANNED
    }char_status;

    int active_chan;
    int chan[MAX_CHAN_SLOTS];       // chan0, chan1, chan2

    int unused;

    int map_id;
    int map_tile;

    enum {

        DEBUG_OFF,
        DEBUG_EXPLORE,
        DEBUG_PATH,
        DEBUG_TRACK
    }debug_status;

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
    int neck_attachment_type;
    int actor_scale;

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
    int harvest_lvl;
    int alchemy_lvl;
    int overall_lvl;
    int attack_lvl;
    int defence_lvl;
    int magic_lvl;
    int potion_lvl;

    int material_pts;
    int max_material_pts;

    int ethereal_pts;
    int max_ethereal_pts;

    int food_lvl;

    int manufacture_exp;
    int harvest_exp;
    int alchemy_exp;
    int overall_exp;
    int attack_exp;
    int defence_exp;
    int magic_exp;
    int potion_exp;

    int book_id;
    int max_book_time;
    int elapsed_book_time;

    struct client_inventory_type inventory[MAX_EQUIP_SLOT+1];

    bool bag_open;
    int open_bag_id;

    int portrait_id; //npc portrait

    bool boat_booked;
    int boat_node;
    int boat_departure_time;
    int boat_arrival_time;
    bool on_boat;

    //storage of npc option data
    struct{

        int boat_node;
        int boat_departure_time;
        int price;
        int amount;
    } npc_option[12];

    time_t npc_choice_time;
    int npc_node;
    int action_node;
};

struct client_list_type {

    int client_count;
    struct client_node_type client[MAX_ACTORS];
};
extern struct client_list_type clients;


/** RESULT   : finds next free node in the actor struct

    RETURNS  : void

    PURPOSE  : enables decoupling of actor nodes and client socket numbers

    NOTES    :
**/
int get_next_free_actor_node();


/** RESULT   : closes a client connection

    RETURNS  : void

    PURPOSE  : used in socket_accept_callback and socket_read_callback

    NOTES    :
**/
void close_connection_slot(int actor_node);

#endif // CLIENTS_H_INCLUDED
