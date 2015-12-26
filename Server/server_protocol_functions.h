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

#ifndef SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED
#define SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED

#include <stdint.h>

#include "chat.h" //support for MAX_CHAN_SLOTS

#define SEND_TEXT_MAX 1024

struct {// struct used to transfer data to the _add_enhanced_actor_packet and
        //_remove_actor_packet functions
    int actor_node;
    char char_name[80];
    int guild_id;
    int map_id;
    int map_tile;
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
    int player_type;
    char banner[80];
} actor;


/** RESULT  : sends the log_in_ok packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char has been successfully logged in

    NOTES   :
*/
void send_login_ok(int socket);


/** RESULT  : sends the log_in_not_ok packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char has not been successfully logged in

    NOTES   :
*/
void send_login_not_ok(int socket);


/** RESULT  : sends the you_dont_exist packet to the client

    RETURNS : void

    PURPOSE : to let the client know that char does not exist

    NOTES   :
*/
void send_you_dont_exist(int socket);


/** RESULT  : sends the create_char_ok packet to the client

    RETURNS : void

    PURPOSE : sent following successful char creation

    NOTES   :
*/
void send_create_char_ok(int socket);


/** RESULT  : sends the create_char_not_ok packet to the client

    RETURNS : void

    PURPOSE : sent following unsuccessful char creation

    NOTES   :
*/
void send_create_char_not_ok(int socket);


/** RESULT  : sends the you_are packet to the client

    RETURNS : void

    PURPOSE : sends the client a code to identify connection following log in

    NOTES   :
*/
void send_you_are(int socket);


/** RESULT  : sends the raw_text packet to client

    RETURNS : void

    PURPOSE : sends text messages to a client via the specified channel

    NOTES   :
*/
void send_raw_text(int socket, int chan_type, char *text);


/** RESULT  : sends the here_your_inventory packet to client

    RETURNS : void

    PURPOSE : sends the character inventory to client at log-in

    NOTES   :
*/
void send_here_your_inventory(int socket);


/** RESULT  : sends the here_your_ground_items packet to client

    RETURNS : void

    PURPOSE : sends the bag inventory to client

    NOTES   :
*/
void send_here_your_ground_items(int socket, int bag_id);


/** RESULT  : sends the get_active_channels packet to client

    RETURNS : void

    PURPOSE : sends active chat channels to client at log-in

    NOTES   :
*/
void send_get_active_channels(int socket, unsigned char active_chan, int *chan_slot);


/** RESULT  : sends the here_your_stats packet to client

    RETURNS : void

    PURPOSE : sends the characters stats to client at log-in

    NOTES   :
*/
void send_here_your_stats(int socket);


/** RESULT  : sends the change_map packet to client

    RETURNS : void

    PURPOSE : informs the client of the map to be used with this character

    NOTES   :
*/
void send_change_map(int socket, char *elm_filename);


/** RESULT  : creates an add_new_enhanced_actor_packet

    RETURNS : void

    PURPOSE : wrapper for _add_new_enhanced_actor_packet that automatically transfers details
              of the char specified in socket

    NOTES   :
*/
void add_new_enhanced_actor_packet(int socket, unsigned char *packet, size_t *packet_length);


/** RESULT  : creates an add_new_enhanced_actor_packet

    RETURNS : void

    PURPOSE : enables a packet to be created once which can then be sent multiple times

    NOTES   : Because this packet is broadcasted to multiple clients we simply create the packet
              and leave the broadcast function to send
*/
void _add_new_enhanced_actor_packet(unsigned char *packet, size_t *packet_length);


/** RESULT  : creates an remove_actor_packet

    RETURNS : void

    PURPOSE : wrapper for _remove_actor_packet that automatically transfers details
              of the char specified in socket

    NOTES   :
*/
void remove_actor_packet(int actor_node, unsigned char *packet, size_t *packet_length);


/** RESULT  : creates the remove actor_packet to client

    RETURNS : void

    PURPOSE : enables a packet to be created once which can then be sent multiple times

    NOTES   : Because this packet is broadcasted to multiple clients we simply create the packet
              and leave the broadcast function to send
*/
void _remove_actor_packet(unsigned char *packet, size_t *packet_length);


/** RESULT  : creates the add_actor_packet

    RETURNS : void

    PURPOSE : enables a packet to be created once which can then be sent multiple times

    NOTES   : Because this packet is broadcasted to multiple clients we simply create the packet
              and leave the broadcast function to send
*/
void add_actor_packet(int socket, unsigned char move, unsigned char *packet, size_t *packet_length);


/** RESULT  : sends the new_minute packet

    RETURNS : void

    PURPOSE :

    NOTES   : used to set client game time
*/
void send_new_minute(int socket, int minute);


/** RESULT  : sends the get_new_inventory_item packet

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void send_get_new_inventory_item(int socket, int image_id, int amount, int slot);


/** RESULT  : creates the get_new_bag_packet

    RETURNS : void

    PURPOSE : enables a packet to be created once which can then be sent multiple times

    NOTES   : Because this packet is broadcasted to multiple clients we simply create the packet
              and leave the broadcast function to send
*/
void get_new_bag_packet(int socket, int bag_list_number, unsigned char *packet, size_t *packet_length);


/** RESULT  : sends the close_bag packet

    RETURNS : void

    PURPOSE : closes an open bag bag inventory grid

    NOTES   :
*/
void send_close_bag(int socket);


/** RESULT  : sends the destroy_bag packet

    RETURNS : void

    PURPOSE : poofs a bag

    NOTES   :
*/
void send_destroy_bag(int socket, int bag_id);


/** RESULT  : sends the get_bags_list_packet

    RETURNS : void

    PURPOSE : sends client a list of bags

    NOTES   :
*/
void send_get_bags_list(int socket);


/** RESULT  : sends inventory text

    RETURNS : void

    PURPOSE : sends inventory text to the client inventory grid

    NOTES   :
*/
void send_inventory_item_text(int socket, char *text);

// TODO (themuntdregger#1#): document npc functions
void send_npc_text(int socket, char *text);

void send_npc_options_list(int socket, int npc_actor_node, char *options);

void send_npc_info(int socket, char *npc_name, int npc_id);

/*****************************************************************************************************
***                                   C FUNCTIONS CALLED FROM C++ MODULES                          ***
******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** RESULT  : sends formatted text message to a client via  the specified channel

    RETURNS : void

    PURPOSE : wrapper for send_raw_text

    NOTES   :

*/
void send_text(int socket, int channel_type, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif // SERVER_PROTOCOL_FUNCTIONS_H_INCLUDED
