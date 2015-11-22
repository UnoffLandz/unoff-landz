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

#define DEBUG_SERVER_PROTOCOL_FUNCTIONS 0

void send_new_minute(int connection, int minute){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t minute;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=NEW_MINUTE;


    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;
    packet.minute=(uint16_t)minute;

    send_packet(connection, &packet, packet_length);
}


void send_login_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=LOG_IN_OK;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_display_client_window(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=DISPLAY_CLIENT_WINDOW;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_login_not_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=LOG_IN_NOT_OK;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_you_dont_exist(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', (size_t)packet_length);

    //add data
    packet.protocol=YOU_DONT_EXIST;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_you_are(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t connection;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', (size_t)packet_length);

    //add data
    packet.protocol=YOU_ARE;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;
    packet.connection=(uint16_t)connection;

    send_packet(connection, &packet, packet_length);
}


void send_create_char_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', (size_t)packet_length);

    //add data
    packet.protocol=CREATE_CHAR_OK;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_create_char_not_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=CREATE_CHAR_NOT_OK;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_raw_text(int connection, int channel, char *text){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char channel;
        char text[1024];
    }packet;

    //clear the struct
    memset(&packet, '0', sizeof(packet));

    //The struct size includes a reserve of 1024 for the text message.
    //We therefore calculate the actual packet length by taking the
    //struct size less the 1024 reserved for the message and then
    //add on the actual message length
    size_t packet_length=sizeof(packet)- 1024 + strlen(text)+1;

    //add data
    packet.protocol=RAW_TEXT;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;
    packet.channel=(unsigned char)channel;
    strcpy(packet.text, text);

    send_packet(connection, &packet, packet_length);
}

void send_text(int connection, int channel_type, const char *fmt, ...){

    /** public function - see header */

    char text_in[SEND_TEXT_MAX]="";

    va_list args;
    va_start(args, fmt);

    if (vsprintf(text_in, fmt, args)>SEND_TEXT_MAX){

        log_event(EVENT_ERROR, "text string exceeds max in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    send_raw_text(connection, channel_type, text_in);

    va_end(args);
}


void send_here_your_inventory(int connection){

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
        }inventory[MAX_INVENTORY_SLOTS];

    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_INVENTORY;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    packet.slot_count=MAX_INVENTORY_SLOTS;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        packet.inventory[i].object_id=(uint16_t)clients.client[connection].inventory[i].object_id;
        packet.inventory[i].amount=(uint32_t)clients.client[connection].inventory[i].amount;
        packet.inventory[i].slot=(unsigned char)i;
        packet.inventory[i].flags=0;
    }

    send_packet(connection, &packet, packet_length);
}


void send_here_your_ground_items(int connection, int bag_id){

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
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_GROUND_ITEMS;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    packet.slot_count=MAX_BAG_SLOTS;

    for(int i=0; i<MAX_BAG_SLOTS; i++){

        packet.inventory[i].object_id=(uint16_t)bag[bag_id].inventory[i].object_id;
        packet.inventory[i].amount=(uint32_t)bag[bag_id].inventory[i].amount;
        packet.inventory[i].slot=(unsigned char)i;
    }

    send_packet(connection, &packet, packet_length);
}


void send_close_bag(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=CLOSE_BAG;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    send_packet(connection, &packet, packet_length);
}


void send_get_active_channels(int connection, unsigned char active_chan, int *chan_slot){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        unsigned char _active_chan;
        uint32_t _chan_slot[MAX_CHAN_SLOTS];
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=GET_ACTIVE_CHANNELS;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    packet._active_chan=active_chan;

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        packet._chan_slot[i]=(uint32_t)chan_slot[i];
    }

    send_packet(connection, &packet, packet_length);
}


void send_here_your_stats(int connection){

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
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_STATS;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    packet.physique_pp=(uint16_t)clients.client[connection].physique_pp;
    packet.max_physique=(uint16_t)clients.client[connection].max_physique;
    packet.coordination_pp=(uint16_t)clients.client[connection].coordination_pp;
    packet.max_coordination=(uint16_t)clients.client[connection].max_coordination;
    packet.reasoning_pp=(uint16_t)clients.client[connection].reasoning_pp;
    packet.max_reasoning=(uint16_t)clients.client[connection].max_reasoning;
    packet.will_pp=(uint16_t)clients.client[connection].will_pp;
    packet.max_will=(uint16_t)clients.client[connection].max_will;
    packet.instinct_pp=(uint16_t)clients.client[connection].instinct_pp;
    packet.max_instinct=(uint16_t)clients.client[connection].max_instinct;
    packet.vitality_pp=(uint16_t)clients.client[connection].vitality_pp;
    packet.max_vitality=(uint16_t)clients.client[connection].max_vitality;

    packet.human=(uint16_t)clients.client[connection].human;
    packet.max_human=(uint16_t)clients.client[connection].max_human;
    packet.animal=(uint16_t)clients.client[connection].animal;
    packet.max_animal=(uint16_t)clients.client[connection].max_animal;
    packet.vegetal=(uint16_t)clients.client[connection].vegetal;
    packet.max_vegetal=(uint16_t)clients.client[connection].max_vegetal;
    packet.inorganic=(uint16_t)clients.client[connection].inorganic;
    packet.max_inorganic=(uint16_t)clients.client[connection].max_inorganic;
    packet.artificial=(uint16_t)clients.client[connection].artificial;
    packet.max_artificial=(uint16_t)clients.client[connection].max_artificial;
    packet.magic=(uint16_t)clients.client[connection].magic;
    packet.max_magic=(uint16_t)clients.client[connection].max_magic;

    packet.inventory_emu=(uint16_t)get_inventory_emu(connection);
    packet.max_inventory_emu=(uint16_t)get_max_inventory_emu(connection);
    packet.material_pts=(uint16_t)clients.client[connection].material_pts;
    packet.max_material_pts=(uint16_t)clients.client[connection].max_material_pts;
    packet.ethereal_pts=(uint16_t)clients.client[connection].ethereal_pts;
    packet.max_ethereal_pts=(uint16_t)clients.client[connection].max_ethereal_pts;
    packet.food_lvl=(uint16_t)clients.client[connection].food_lvl;
    packet.elapsed_book_time=(uint16_t)clients.client[connection].elapsed_book_time;
    packet.unused=0;

    packet.manufacture_exp=(uint32_t)clients.client[connection].manufacture_exp;
    packet.max_manufacture_exp=(uint32_t)clients.client[connection].max_manufacture_exp;
    packet.harvest_exp=(uint32_t)clients.client[connection].harvest_exp;
    packet.max_harvest_exp=(uint32_t)clients.client[connection].max_harvest_exp;
    packet.alchemy_exp=(uint32_t)clients.client[connection].alchemy_exp;
    packet.overall_exp=(uint32_t)clients.client[connection].overall_exp;
    packet.max_overall_exp=(uint32_t)clients.client[connection].max_overall_exp;
    packet.attack_exp=(uint32_t)clients.client[connection].attack_exp;
    packet.max_attack_exp=(uint32_t)clients.client[connection].max_attack_exp;
    packet.defence_exp=(uint32_t)clients.client[connection].defence_exp;
    packet.max_defence_exp=(uint32_t)clients.client[connection].max_defence_exp;
    packet.magic_exp=(uint32_t)clients.client[connection].magic_exp;
    packet.max_magic_exp=(uint32_t)clients.client[connection].max_magic_exp;
    packet.potion_exp=(uint32_t)clients.client[connection].potion_exp;
    packet.max_potion_exp=(uint32_t)clients.client[connection].max_potion_exp;
    packet.book_id=(uint16_t)clients.client[connection].book_id;
    packet.max_book_time=(uint16_t)clients.client[connection].max_book_time;

    send_packet(connection, &packet, packet_length);
}


void send_change_map(int connection, char *elm_filename){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char elm_filename[1024];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //The struct size includes a reserve of 1024 for the map name.
    //We therefore calculate the actual packet length by taking the
    //struct size less the 1024 reserved for the map name and then
    //add on the actual message length
    size_t packet_length=sizeof(packet) - 1024 + strlen(elm_filename)+1;

    //add data
    packet.protocol=CHANGE_MAP;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    strcpy(packet.elm_filename, elm_filename);

    send_packet(connection, &packet, packet_length);
}


void add_new_enhanced_actor_packet(int connection, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

  struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t connection;
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
    memset(&_packet1, '0', sizeof(_packet1));
    memset(&_packet2, '0', sizeof(_packet2));

        //create a char array carrying the banner (char name and guild name separated by a space and
    //127+colour character)
    int guild_tag_colour=127+guilds.guild[clients.client[connection].guild_id].guild_tag_colour;

    size_t banner_length=(size_t)sprintf(_packet1.banner, "%s %c%s",
        clients.client[connection].char_name,
        guild_tag_colour,
        guilds.guild[clients.client[connection].guild_id].guild_tag);

    //calculate the total packet length
    *packet_length=sizeof(_packet1) - 80 + banner_length+1 + sizeof(_packet2);

    //add data to packet 1
    _packet1.protocol=ADD_NEW_ENHANCED_ACTOR;
    _packet1.connection=(uint16_t)connection;
    int data_length=(int)*packet_length-2;
    _packet1.data_length=(uint16_t)data_length;

    //_packet1.data_length=*packet_length-2;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int x=clients.client[connection].map_tile % map_axis;
    int y=clients.client[connection].map_tile / map_axis;
    _packet1.x_pos=(uint16_t)x;
    _packet1.y_pos=(uint16_t)y;

    _packet1.z_pos=0; //z position (set to 0 pending further development)
    _packet1.rot=45; //rotation angle (set to 45 pending further development)

    _packet1.char_type=(unsigned char)clients.client[connection].char_type;
    _packet1.unused=0; //unused (set to 0)

    _packet1.skin_type=(unsigned char)clients.client[connection].skin_type;
    _packet1.hair_type=(unsigned char)clients.client[connection].hair_type;
    _packet1.shirt_type=(unsigned char)clients.client[connection].shirt_type;
    _packet1.pants_type=(unsigned char)clients.client[connection].pants_type;
    _packet1.boots_type=(unsigned char)clients.client[connection].boots_type;
    _packet1.head_type=(unsigned char)clients.client[connection].head_type;
    _packet1.shield_type=(unsigned char)clients.client[connection].shield_type;
    _packet1.weapon_type=(unsigned char)clients.client[connection].weapon_type;
    _packet1.cape_type=(unsigned char)clients.client[connection].cape_type;
    _packet1.helmet_type=(unsigned char)clients.client[connection].helmet_type;
    _packet1.frame_type=(unsigned char)clients.client[connection].frame;
    _packet1.max_health=(unsigned char)clients.client[connection].max_health;
    _packet1.current_health=(unsigned char)clients.client[connection].current_health;
    _packet1.player_type=1; //special (PLAYER=1 NPC=??)

    //add data to packet 2
    _packet2.unknown=0;
    _packet2.char_size=64; //char size (min=2 normal=64 max=127)
    _packet2.riding=255; //riding (nothing=255  brown horse=200)
    _packet2.neck_attachment=64; //neck attachment (none=64)

    //create the complete packet
    memcpy(packet, &_packet1, sizeof(_packet1)-80 + banner_length+1);
    memcpy(packet + *packet_length-4, &_packet2, sizeof(_packet2));
}


void remove_actor_packet(int connection, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t connection;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet 1
    _packet.protocol=REMOVE_ACTOR;
    int data_length=(int)*packet_length-2;
    _packet.data_length=(uint16_t)data_length;

    //_packet.data_length=*packet_length-2;
    _packet.connection=(uint16_t)connection;

    memcpy(packet, &_packet, sizeof(_packet));
}


void add_actor_packet(int connection, unsigned char move, unsigned char *packet, size_t *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t connection;
        unsigned char move;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet 1
    _packet.protocol=ADD_ACTOR;
    int data_length=(int)*packet_length-2;
    _packet.data_length=(uint16_t)data_length;

    //_packet.data_length=*packet_length-2;
    _packet.connection=(uint16_t)connection;
    _packet.move=move;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_get_new_inventory_item(int connection, int object_id, int amount, int slot){

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
    memset(&packet, '0', sizeof(packet));

    //add data to packet
    packet.protocol=GET_NEW_INVENTORY_ITEM;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;

    packet.object_id=(uint16_t)object_id;
    packet.amount=(uint32_t)amount;
    packet.slot=(unsigned char)slot;
    packet.flags=0;

    send_packet(connection, &packet, packet_length);
}


void get_new_bag_packet(int connection, int bag_list_number, unsigned char *packet, size_t *packet_length){

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
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet
    _packet.protocol=GET_NEW_BAG;
    int data_length=(int)*packet_length-2;
    _packet.data_length=(uint16_t)data_length;

    _packet.x_pos=(uint16_t)get_x_pos(clients.client[connection].map_tile, clients.client[connection].map_id);
    _packet.y_pos=(uint16_t)get_y_pos(clients.client[connection].map_tile, clients.client[connection].map_id);
    _packet.bag_list_number=(unsigned char)bag_list_number;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_destroy_bag(int connection, int bag_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        uint16_t bag_id;
    }packet;

    size_t packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=DESTROY_BAG;
    int data_length=(int)packet_length-2;
    packet.data_length=(uint16_t)data_length;
    packet.bag_id=(uint16_t)bag_id;

    send_packet(connection, &packet, packet_length);
}


void send_get_bags_list(int connection){

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
    memset(&packet, '0', sizeof(packet));

    //add data
    packet.protocol=GET_BAGS_LIST;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int j=0;

    for(uint8_t i=0; i<MAX_BAGS; i++){

        if(bag[i].map_id==clients.client[connection].map_id){

            packet.bag_list[j].x=(uint16_t)(bag[i].tile % map_axis);
            packet.bag_list[j].y=(uint16_t)(bag[i].tile / map_axis);

            packet.bag_list[j].bag_id=(unsigned char)j;

            j++;
        }
    }

    packet.bag_count=(unsigned char)j;

    if(j>0){

        size_t packet_length=(size_t)(4+(j*5));

        int data_length=(int)packet_length-2;
        packet.data_length=(uint16_t)data_length;

        send_packet(connection, &packet, packet_length);
    }
}
