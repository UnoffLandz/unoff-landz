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

#include <string.h>     //support for memcpy and strlen
#include <stdio.h>      //support for sprintf
#include <stdint.h>     //support uint16_t data type
#include <stdarg.h>     //support for args

#include "server_protocol.h"
#include "character_inventory.h"
#include "clients.h"
#include "maps.h"
#include "logging.h"
#include "packet.h"
#include "colour.h"
#include "string_functions.h"
#include "guilds.h"
#include "bags.h"
#include "server_start_stop.h"
#include "server_protocol_functions.h"
#include "npc.h"

void send_new_minute(int socket, int minute){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t minute;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=NEW_MINUTE;
    packet.data_length=(uint16_t)(packet_length-2);
    packet.minute=(uint16_t)minute;

    send_packet(socket, &packet, packet_length);
}


void send_login_ok(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=LOG_IN_OK;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_login_not_ok(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=LOG_IN_NOT_OK;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_you_dont_exist(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, (size_t)packet_length);

    //add data
    packet.protocol=YOU_DONT_EXIST;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_you_are(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t socket;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, (size_t)packet_length);

    //add data
    packet.protocol=YOU_ARE;
    packet.data_length=(uint16_t)(packet_length-2);
    packet.socket=(uint16_t)client_socket[socket].actor_node;

    send_packet(socket, &packet, packet_length);
}


void send_create_char_ok(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, (size_t)packet_length);

    //add data
    packet.protocol=CREATE_CHAR_OK;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_create_char_not_ok(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=CREATE_CHAR_NOT_OK;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_raw_text(int socket, int channel, char *text){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char channel;
        char text[SEND_TEXT_MAX];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //calculate the size of the packet length
    size_t packet_length=4 + strlen(text)+1;

    //add data
    packet.protocol=RAW_TEXT;
    packet.data_length=(uint16_t)(packet_length-2); //subtract 2 from the packet length to calculate the data length
    packet.channel=(unsigned char)channel;
    strcpy(packet.text, text);

    send_packet(socket, &packet, packet_length);
}


void send_inventory_item_text(int socket, char *text){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char text[SEND_TEXT_MAX];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //calculate the size of the packet length
    size_t packet_length=3 + strlen(text)+1;

    //add data
    packet.protocol=INVENTORY_ITEM_TEXT;
    packet.data_length=(uint16_t)(packet_length-2); //subtract 2 from the packet length to calculate the data length
    strcpy(packet.text, text);

    send_packet(socket, &packet, packet_length);
}


void send_text(int socket, int channel_type, const char *fmt, ...){

    /** public function - see header */

    char text_in[SEND_TEXT_MAX]="";

    va_list args;
    va_start(args, fmt);

    if (vsprintf(text_in, fmt, args)>SEND_TEXT_MAX){

        log_event(EVENT_ERROR, "text string exceeds max in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    send_raw_text(socket, channel_type, text_in);

    va_end(args);
}


void send_here_your_inventory(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char slot_count;

        struct __attribute__((__packed__)){

            uint16_t object_id;
            uint32_t amount;
            unsigned char slot;
            unsigned char flags;
        }inventory[MAX_EQUIP_SLOT+1];

    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=HERE_YOUR_INVENTORY;
    packet.data_length=(uint16_t)(packet_length-2);

    packet.slot_count=MAX_EQUIP_SLOT+1;

    int actor_node=client_socket[socket].actor_node;

    for(int i=0; i<MAX_EQUIP_SLOT+1; i++){

        packet.inventory[i].object_id=(uint16_t)clients.client[actor_node].inventory[i].object_id;
        packet.inventory[i].amount=(uint32_t)clients.client[actor_node].inventory[i].amount;
        packet.inventory[i].slot=(unsigned char)i;
        packet.inventory[i].flags=0;
    }

    send_packet(socket, &packet, packet_length);
}


void send_here_your_ground_items(int socket, int bag_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char slot_count;

        struct __attribute__((__packed__)){

            uint16_t object_id;
            uint32_t amount;
            unsigned char slot;
        }inventory[MAX_BAG_SLOTS];

    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=HERE_YOUR_GROUND_ITEMS;
    packet.data_length=(uint16_t)(packet_length-2);

    packet.slot_count=MAX_BAG_SLOTS;

    for(int i=0; i<MAX_BAG_SLOTS; i++){

        packet.inventory[i].object_id=(uint16_t)bag[bag_id].inventory[i].object_id;
        packet.inventory[i].amount=(uint32_t)bag[bag_id].inventory[i].amount;
        packet.inventory[i].slot=(unsigned char)i;
    }

    send_packet(socket, &packet, packet_length);
}


void send_close_bag(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=CLOSE_BAG;
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void send_get_active_channels(int socket){

    /** public function - see header */

/*  Examples of effect of values used in GET_ACTIVE_CHANNELS

    Active  Slot    Slot    Slot
    Chan    Zero    One     Two     Channels
    -----------------------------------------------------------
    2       1       2       3       Newbie-Help Gen-Help +Market
    1       1       2       3       Newbie-Help +Gen-Help Market
    0       1       2       3       +Newbie-Help Gen-Help Market
    1       0       1       2       +Newbie-Help Gen-Help
*/

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char _active_chan; // values start a 0 (chan slot 1)
        uint32_t _chan_slot[MAX_CHAN_SLOTS]; //values = channel number (0 = no chan)
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=GET_ACTIVE_CHANNELS;
    packet.data_length=(uint16_t)(packet_length-2);

    int actor_node=client_socket[socket].actor_node;

    packet._active_chan=(unsigned char)clients.client[actor_node].active_chan;

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        packet._chan_slot[i]=(uint32_t)clients.client[actor_node].chan[i];
    }

    send_packet(socket, &packet, packet_length);
}

void send_partial_stat(int socket, int attribute_type, int attribute_level){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;

        uint8_t attribute_type;
        uint32_t attribute_level;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=SEND_PARTIAL_STAT;
    packet.data_length=(uint16_t)(packet_length-2);

    packet.attribute_type=(uint8_t)attribute_type;
    packet.attribute_level=(uint32_t)attribute_level;

    send_packet(socket, &packet, packet_length);
}


void actor_wear_item_packet(int char_id, unsigned char *packet, size_t *packet_length, int equipable_item_type, int equipable_item_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;

        uint16_t char_id;
        uint8_t equipable_item_type;
        uint8_t equipable_item_id;
    }_packet;

    *packet_length=sizeof(_packet);

    //clear the struct
    memset(&_packet, 0, *packet_length);

    //add data
    _packet.protocol=ACTOR_WEAR_ITEM;
    _packet.data_length=(uint16_t)(*packet_length-2);

    _packet.char_id=(uint16_t)char_id;
    _packet.equipable_item_type=(uint8_t)equipable_item_type;
    _packet.equipable_item_id=(uint8_t)equipable_item_id;

    memcpy(packet, &_packet, sizeof(_packet));
}


void actor_unwear_item_packet(int char_id, unsigned char *packet, size_t *packet_length, int equipable_item_type, int equipable_item_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;

        uint16_t char_id;
        uint8_t equipable_item_type;
        uint8_t equipable_item_id;
    }_packet;

    *packet_length=sizeof(_packet);

    //clear the struct
    memset(&_packet, 0, *packet_length);

    //add data
    _packet.protocol=ACTOR_UNWEAR_ITEM;
    _packet.data_length=(uint16_t)(*packet_length-2);

    _packet.char_id=(uint16_t)char_id;
    _packet.equipable_item_type=(uint8_t)equipable_item_type;
    _packet.equipable_item_id=(uint8_t)equipable_item_id;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_here_your_stats(int socket){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;

        uint16_t physique_pp;
        uint16_t max_physique;
        uint16_t coordination_pp;
        uint16_t max_coordination;
        uint16_t reasoning_pp;
        uint16_t max_reasoning;
        uint16_t will_pp;
        uint16_t max_will;
        uint16_t instinct_pp;
        uint16_t max_instinct;
        uint16_t vitality_pp;
        uint16_t max_vitality;

        uint16_t human;
        uint16_t max_human;
        uint16_t animal;
        uint16_t max_animal;
        uint16_t vegetal;
        uint16_t max_vegetal;
        uint16_t inorganic;
        uint16_t max_inorganic;
        uint16_t artificial;
        uint16_t max_artificial;
        uint16_t magic;
        uint16_t max_magic;

        uint16_t manufacturing_lvl;
        uint16_t max_manufacturing_lvl;
        uint16_t harvest_lvl;
        uint16_t max_harvest_lvl;
        uint16_t alchemy_lvl;
        uint16_t max_alchemy_lvl;
        uint16_t overall_lvl;
        uint16_t max_overall_lvl;
        uint16_t attack_lvl;
        uint16_t max_attack_lvl;
        uint16_t defence_lvl;
        uint16_t max_defence_lvl;
        uint16_t magic_lvl;
        uint16_t max_magic_lvl;
        uint16_t potion_lvl;
        uint16_t max_potion_lvl;

        uint16_t inventory_emu;
        uint16_t max_inventory_emu;

        uint16_t material_pts;
        uint16_t max_material_pts;
        uint16_t ethereal_pts;
        uint16_t max_ethereal_pts;
        uint16_t food_lvl;
        uint16_t elapsed_book_time;
        uint16_t unused;

        uint32_t manufacture_exp;
        uint32_t max_manufacture_exp;
        uint32_t harvest_exp;
        uint32_t max_harvest_exp;
        uint32_t alchemy_exp;
        uint32_t overall_exp;
        uint32_t max_overall_exp;
        uint32_t attack_exp;
        uint32_t max_attack_exp;
        uint32_t defence_exp;
        uint32_t max_defence_exp;
        uint32_t magic_exp;
        uint32_t max_magic_exp;
        uint32_t potion_exp;
        uint32_t max_potion_exp;
        uint16_t book_id;
        uint16_t max_book_time;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    int actor_node=client_socket[socket].actor_node;

    //add data
    packet.protocol=HERE_YOUR_STATS;
    packet.data_length=(uint16_t)(packet_length-2);

    packet.physique_pp=(uint16_t)clients.client[actor_node].physique_pp;
    packet.max_physique=(uint16_t)clients.client[actor_node].max_physique;
    packet.coordination_pp=(uint16_t)clients.client[actor_node].coordination_pp;
    packet.max_coordination=(uint16_t)clients.client[actor_node].max_coordination;
    packet.reasoning_pp=(uint16_t)clients.client[actor_node].reasoning_pp;
    packet.max_reasoning=(uint16_t)clients.client[actor_node].max_reasoning;
    packet.will_pp=(uint16_t)clients.client[actor_node].will_pp;
    packet.max_will=(uint16_t)clients.client[actor_node].max_will;
    packet.instinct_pp=(uint16_t)clients.client[actor_node].instinct_pp;
    packet.max_instinct=(uint16_t)clients.client[actor_node].max_instinct;
    packet.vitality_pp=(uint16_t)clients.client[actor_node].vitality_pp;
    packet.max_vitality=(uint16_t)clients.client[actor_node].max_vitality;

    packet.human=(uint16_t)clients.client[actor_node].human;
    packet.max_human=(uint16_t)clients.client[actor_node].max_human;
    packet.animal=(uint16_t)clients.client[actor_node].animal;
    packet.max_animal=(uint16_t)clients.client[actor_node].max_animal;
    packet.vegetal=(uint16_t)clients.client[actor_node].vegetal;
    packet.max_vegetal=(uint16_t)clients.client[actor_node].max_vegetal;
    packet.inorganic=(uint16_t)clients.client[actor_node].inorganic;
    packet.max_inorganic=(uint16_t)clients.client[actor_node].max_inorganic;
    packet.artificial=(uint16_t)clients.client[actor_node].artificial;
    packet.max_artificial=(uint16_t)clients.client[actor_node].max_artificial;
    packet.magic=(uint16_t)clients.client[actor_node].magic;
    packet.max_magic=(uint16_t)clients.client[actor_node].max_magic;
    packet.inventory_emu=(uint16_t)get_inventory_emu(actor_node);
    packet.max_inventory_emu=(uint16_t)get_max_inventory_emu(actor_node);

    packet.material_pts=(uint16_t)clients.client[actor_node].material_pts;
    packet.max_material_pts=(uint16_t)clients.client[actor_node].max_material_pts;
    packet.ethereal_pts=(uint16_t)clients.client[actor_node].ethereal_pts;
    packet.max_ethereal_pts=(uint16_t)clients.client[actor_node].max_ethereal_pts;
    packet.food_lvl=(uint16_t)clients.client[actor_node].food_lvl;
    packet.elapsed_book_time=(uint16_t)clients.client[actor_node].elapsed_book_time;
    packet.unused=0;

    packet.manufacture_exp=(uint32_t)clients.client[actor_node].manufacture_exp;
    packet.max_manufacture_exp=(uint32_t)clients.client[actor_node].max_manufacture_exp;
    packet.harvest_exp=(uint32_t)clients.client[actor_node].harvest_exp;
    packet.max_harvest_exp=(uint32_t)clients.client[actor_node].max_harvest_exp;
    packet.alchemy_exp=(uint32_t)clients.client[actor_node].alchemy_exp;
    packet.overall_exp=(uint32_t)clients.client[actor_node].overall_exp;
    packet.max_overall_exp=(uint32_t)clients.client[actor_node].max_overall_exp;
    packet.attack_exp=(uint32_t)clients.client[actor_node].attack_exp;
    packet.max_attack_exp=(uint32_t)clients.client[actor_node].max_attack_exp;
    packet.defence_exp=(uint32_t)clients.client[actor_node].defence_exp;
    packet.max_defence_exp=(uint32_t)clients.client[actor_node].max_defence_exp;
    packet.magic_exp=(uint32_t)clients.client[actor_node].magic_exp;
    packet.max_magic_exp=(uint32_t)clients.client[actor_node].max_magic_exp;
    packet.potion_exp=(uint32_t)clients.client[actor_node].potion_exp;
    packet.max_potion_exp=(uint32_t)clients.client[actor_node].max_potion_exp;
    packet.book_id=(uint16_t)clients.client[actor_node].book_id;
    packet.max_book_time=(uint16_t)clients.client[actor_node].max_book_time;

    send_packet(socket, &packet, packet_length);
}


void send_change_map(int socket, char *elm_filename){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char elm_filename[160];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //The struct size includes a reserve of 160 for the map name.
    //We therefore calculate the actual packet length by taking the
    //struct size less the 160 reserved for the map name and then
    //add on the actual message length
    size_t packet_length=sizeof(packet) - 160 + strlen(elm_filename)+1;

    //add data
    packet.protocol=CHANGE_MAP;
    packet.data_length=(uint16_t)(packet_length-2);
    strcpy(packet.elm_filename, elm_filename);

    send_packet(socket, &packet, packet_length);
}


void send_npc_text(int socket, char *text){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char text[SEND_TEXT_MAX];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //calculate the size of the packet length
    size_t packet_length=4 + strlen(text)+1;

    //add data
    packet.protocol=NPC_TEXT;
    packet.data_length=(uint16_t)(packet_length-2); //subtract 2 from the packet length to calculate the data length
    strcpy(packet.text, text);

    send_packet(socket, &packet, packet_length);
}


void send_npc_info(int socket, char *npc_name, int npc_portrait_id){

/** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char npc_name[20];
        unsigned char npc_portrait_id;
    }packet;

   //clear the struct
    memset(&packet, 0, sizeof(packet));

    //calculate the size of the packet length
    size_t packet_length=sizeof(packet);

    //add data
    packet.protocol=SEND_NPC_INFO;
    packet.data_length=(uint16_t)(packet_length-2); //subtract 2 from the packet length to calculate the data length
    strcpy(packet.npc_name, npc_name);
    packet.npc_portrait_id=(unsigned char)npc_portrait_id;

    send_packet(socket, &packet, packet_length);
}


void send_npc_options_list(int socket, int npc_actor_node, char *options){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;

        struct __attribute__((__packed__)){

            uint16_t len;
            char option_text[80];
            uint16_t response;
            uint16_t actor_id;
        }npc_option[MAX_NPC_OPTIONS];

    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=NPC_OPTIONS_LIST;

    //load the npc options
    int j=0;
    size_t k=0;

    for(size_t i=0; i<strlen(options); i++){

        if(options[i]==';' || i==strlen(options)-1){

            strncpy(packet.npc_option[j].option_text, options+k, i-k);
            packet.npc_option[j].actor_id=(uint16_t)npc_actor_node;
            packet.npc_option[j].response=(uint16_t)j;
            packet.npc_option[j].len=(uint16_t)(80);

            k=i+(size_t)1;
            j++;
        }
    }

    //shorten the packet length to cut off excess options
    packet_length=(size_t)(3+(j*86));
    packet.data_length=(uint16_t)(packet_length-2);

    send_packet(socket, &packet, packet_length);
}


void add_new_enhanced_actor_packet(int actor_node, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    actor.actor_node=actor_node;
    strcpy(actor.char_name, clients.client[actor_node].char_name);
    actor.guild_id=clients.client[actor_node].guild_id;

    actor.map_tile=clients.client[actor_node].map_tile;
    actor.map_id=clients.client[actor_node].map_id;

    actor.char_type=clients.client[actor_node].char_type;
    actor.skin_type=clients.client[actor_node].skin_type;
    actor.hair_type=clients.client[actor_node].hair_type;
    actor.shirt_type=clients.client[actor_node].shirt_type;
    actor.pants_type=clients.client[actor_node].pants_type;
    actor.boots_type=clients.client[actor_node].boots_type;
    actor.head_type=clients.client[actor_node].head_type;
    actor.shield_type=clients.client[actor_node].shield_type;
    actor.weapon_type=clients.client[actor_node].weapon_type;
    actor.cape_type=clients.client[actor_node].cape_type;
    actor.helmet_type=clients.client[actor_node].helmet_type;
    actor.frame=clients.client[actor_node].frame;
    actor.max_health=clients.client[actor_node].max_health;
    actor.current_health=clients.client[actor_node].current_health;
    actor.player_type=clients.client[actor_node].player_type;

    _add_new_enhanced_actor_packet(packet, packet_length);
}


void _add_new_enhanced_actor_packet(unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t actor_node;
        uint16_t x_pos;
        uint16_t y_pos;
        uint16_t z_pos;
        uint16_t rot;
        unsigned char char_type;
        unsigned char unused;
        unsigned char skin_type;
        unsigned char hair_type;
        unsigned char shirt_type;
        unsigned char pants_type;
        unsigned char boots_type;
        unsigned char head_type;
        unsigned char shield_type;
        unsigned char weapon_type;
        unsigned char cape_type;
        unsigned char helmet_type;
        unsigned char frame_type;
        uint16_t max_health;
        uint16_t current_health;
        unsigned char player_type;
        char banner[80];
    }_packet1;

    //packet structure following the banner
    struct __attribute__((__packed__)){

        unsigned char unknown;
        unsigned char char_size;
        unsigned char riding;
        unsigned char neck_attachment;
    }_packet2;

    //clear the structs
    memset(&_packet1, 0, sizeof(_packet1));
    memset(&_packet2, 0, sizeof(_packet2));

    int actor_node=actor.actor_node;

    //create a char array carrying the banner (char name and guild name separated by a space and
    //127+colour character)
    int guild_tag_colour=127+guilds.guild[actor.guild_id].guild_tag_colour;

    size_t banner_length=(size_t)sprintf(_packet1.banner, "%s %c%s",
        actor.char_name,
        guild_tag_colour,
        guilds.guild[actor.guild_id].guild_tag);

    //calculate the total packet length
    *packet_length=sizeof(_packet1) - 80 + banner_length+1 + sizeof(_packet2);

    //add data to packet 1
    _packet1.protocol=ADD_NEW_ENHANCED_ACTOR;
    _packet1.actor_node=(uint16_t)actor_node;
    _packet1.data_length=(uint16_t)(*packet_length-2);

    int map_id=actor.map_id;
    int map_axis=maps.map[map_id].map_axis;

    _packet1.x_pos=(uint16_t)(actor.map_tile % map_axis);
    _packet1.y_pos=(uint16_t)(actor.map_tile / map_axis);

    _packet1.z_pos=0; //z position (set to 0 pending further development)
    _packet1.rot=45; //rotation angle (set to 45 pending further development)

    _packet1.char_type=(unsigned char)actor.char_type;
    _packet1.unused=0; //unused (set to 0)

    _packet1.skin_type=(unsigned char)actor.skin_type;
    _packet1.hair_type=(unsigned char)actor.hair_type;
    _packet1.shirt_type=(unsigned char)actor.shirt_type;
    _packet1.pants_type=(unsigned char)actor.pants_type;
    _packet1.boots_type=(unsigned char)actor.boots_type;
    _packet1.head_type=(unsigned char)actor.head_type;
    _packet1.shield_type=(unsigned char)actor.shield_type;
    _packet1.weapon_type=(unsigned char)actor.weapon_type;
    _packet1.cape_type=(unsigned char)actor.cape_type;
    _packet1.helmet_type=(unsigned char)actor.helmet_type;
    _packet1.frame_type=(unsigned char)actor.frame;
    _packet1.max_health=(unsigned char)actor.max_health;
    _packet1.current_health=(unsigned char)actor.current_health;
    _packet1.player_type=(unsigned char)actor.player_type;

    //add data to packet 2
    _packet2.unknown=0;
    _packet2.char_size=64; //char size (min=2 normal=64 max=127)// TODO (themuntdregger#1#): add actor scale to actor struct and database table
    _packet2.riding=255; //riding (nothing=255  brown horse=200)
    _packet2.neck_attachment=64; //neck attachment (none=64) // TODO (themuntdregger#1#): This setting on the enhanced actor packet might remove the blank medallions on chars

    //create the complete packet
    memcpy(packet, &_packet1, sizeof(_packet1)-80 + banner_length+1);
    memcpy(packet + *packet_length-4, &_packet2, sizeof(_packet2));
}


void remove_actor_packet(int actor_node, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    actor.actor_node=actor_node;

    _remove_actor_packet(packet, packet_length);
}


void _remove_actor_packet(unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t actor_node;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, 0, sizeof(_packet));

    //add data to packet 1
    _packet.protocol=REMOVE_ACTOR;
    _packet.data_length=(uint16_t)(*packet_length-2);
    _packet.actor_node=(uint16_t)actor.actor_node;

    memcpy(packet, &_packet, sizeof(_packet));
}


void add_actor_packet(int socket, unsigned char move, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t actor_node;
        unsigned char move;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, 0, sizeof(_packet));

    //add data to packet 1
    _packet.protocol=ADD_ACTOR;
    _packet.data_length=(uint16_t)(*packet_length-2);
    _packet.actor_node=(uint16_t)client_socket[socket].actor_node;
    _packet.move=move;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_get_new_inventory_item(int socket, int object_id, int amount, int slot){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t object_id;
        uint32_t amount;
        unsigned char slot;
        unsigned char flags;
    }packet;

    //calculate the packet length
    size_t packet_length=sizeof(packet);

    //clear the structs
    memset(&packet, 0, sizeof(packet));

    //add data to packet
    packet.protocol=GET_NEW_INVENTORY_ITEM;
    packet.data_length=(uint16_t)(packet_length-2);
    packet.object_id=(uint16_t)object_id;
    packet.amount=(uint32_t)amount;
    packet.slot=(unsigned char)slot;
    packet.flags=0;

    send_packet(socket, &packet, packet_length);
}


void get_new_bag_packet(int socket, int bag_list_number, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t x_pos;
        uint16_t y_pos;
        unsigned char bag_list_number;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, 0, sizeof(_packet));


    //add data to packet
    _packet.protocol=GET_NEW_BAG;
    _packet.data_length=(uint16_t)(*packet_length-2);

    int actor_node=client_socket[socket].actor_node;
    _packet.x_pos=(uint16_t)get_x_pos(clients.client[actor_node].map_tile, clients.client[actor_node].map_id);
    _packet.y_pos=(uint16_t)get_y_pos(clients.client[actor_node].map_tile, clients.client[actor_node].map_id);

    _packet.bag_list_number=(unsigned char)bag_list_number;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_destroy_bag(int socket, int bag_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t bag_id;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, 0, packet_length);

    //add data
    packet.protocol=DESTROY_BAG;
    packet.data_length=(uint16_t)(packet_length-2);
    packet.bag_id=(uint16_t)bag_id;

    send_packet(socket, &packet, packet_length);
}


void send_get_bags_list(int socket){

  struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char bag_count;

        struct __attribute__((__packed__)){

            uint16_t x;
            uint16_t y;
            unsigned char bag_id;
        }bag_list[MAX_BAGS];

    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //add data
    packet.protocol=GET_BAGS_LIST;

    int actor_node=client_socket[socket].actor_node;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int j=0;

    for(int i=0; i<MAX_BAGS; i++){

        if(bag[i].map_id==clients.client[actor_node].map_id && bag[i].bag_in_use==true){

            packet.bag_list[j].x=(uint16_t)(bag[i].tile % map_axis);
            packet.bag_list[j].y=(uint16_t)(bag[i].tile / map_axis);

            packet.bag_list[j].bag_id=(unsigned char)j;

            j++;
        }
    }

    packet.bag_count=(unsigned char)j;

    if(j>0){

        size_t packet_length=(size_t)(4+(j*5));

        packet.data_length=(uint16_t)(packet_length-2);

        send_packet(socket, &packet, packet_length);
    }
}
