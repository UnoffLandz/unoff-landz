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
#include <sys/socket.h> //support for send function
#include <stdio.h>      //support for sprintf
#include <SDL/SDL_types.h> //support for Uint16 data type

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

void send_packet(int connection, void *packet, int packet_length){

    /** public function - see header */

    log_packet(connection, packet, SEND);

    send(connection, packet, packet_length, 0);
}


void send_new_minute(int connection, int minute){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 minute;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=NEW_MINUTE;
    packet.data_length=packet_length-2;
    packet.minute=minute;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_login_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=LOG_IN_OK;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_display_client_window(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=DISPLAY_CLIENT_WINDOW;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_login_not_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=LOG_IN_NOT_OK;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_you_dont_exist(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=YOU_DONT_EXIST;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_you_are(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 connection;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=YOU_ARE;
    packet.data_length=packet_length-2;
    packet.connection=connection;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_create_char_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=CREATE_CHAR_OK;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_create_char_not_ok(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=CREATE_CHAR_NOT_OK;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_raw_text(int connection, int channel, char *text){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        unsigned char channel;
        char text[1024];
    }packet;

    //clear the struct
    memset(&packet, '0', sizeof(packet));

    //The struct size includes a reserve of 1024 for the text message.
    //We therefore calculate the actual packet length by taking the
    //struct size less the 1024 reserved for the message and then
    //add on the actual message length
    int packet_length=sizeof(packet)- 1024 + strlen(text)+1;

    //add data
    packet.protocol=RAW_TEXT;
    packet.data_length=packet_length-2;
    packet.channel=channel;
    strcpy(packet.text, text);

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
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
        Uint16 data_length;
        unsigned char slot_count;

        struct __attribute__((__packed__)){

            Uint16 object_id;
            int amount;
            unsigned char slot;
            unsigned char flags;
        }inventory[MAX_BAG_SLOTS];

    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_INVENTORY;
    packet.data_length=packet_length-2;
    packet.slot_count=MAX_INVENTORY_SLOTS;

    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        packet.inventory[i].object_id=clients.client[connection].client_inventory[i].object_id;
        packet.inventory[i].amount=clients.client[connection].client_inventory[i].amount;
        packet.inventory[i].slot=i;
        packet.inventory[i].flags=0;
    }

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_here_your_ground_items(int connection, int bag_id){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        unsigned char slot_count;

        struct __attribute__((__packed__)){

            Uint16 object_id;
            int amount;
            unsigned char slot;
        }inventory[MAX_BAG_SLOTS];

    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_GROUND_ITEMS;
    packet.data_length=packet_length-2;
    packet.slot_count=MAX_BAG_SLOTS;

    for(int i=0; i<MAX_BAG_SLOTS; i++){

        packet.inventory[i].object_id=bag[bag_id].inventory[i].object_id;
        packet.inventory[i].amount=bag[bag_id].inventory[i].amount;
        packet.inventory[i].slot=i;
    }

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_close_bag(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=CLOSE_BAG;
    packet.data_length=packet_length-2;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_get_active_channels(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        unsigned char active_channel;
        int chan_slot[MAX_CHAN_SLOTS];

    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=GET_ACTIVE_CHANNELS;
    packet.data_length=packet_length-2;
    packet.active_channel=clients.client[connection].active_chan;

    for(int i=0; i<MAX_CHAN_SLOTS; i++){

        packet.chan_slot[i]=clients.client[connection].chan[i];
    }

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_here_your_stats(int connection){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;

        Uint16 physique_pp;
        Uint16 max_physique;
        Uint16 coordination_pp;
        Uint16 max_coordination;
        Uint16 reasoning_pp;
        Uint16 max_reasoning;
        Uint16 will_pp;
        Uint16 max_will;
        Uint16 instinct_pp;
        Uint16 max_instinct;
        Uint16 vitality_pp;
        Uint16 max_vitality;

        Uint16 human;
        Uint16 max_human;
        Uint16 animal;
        Uint16 max_animal;
        Uint16 vegetal;
        Uint16 max_vegetal;
        Uint16 inorganic;
        Uint16 max_inorganic;
        Uint16 artificial;
        Uint16 max_artificial;
        Uint16 magic;
        Uint16 max_magic;

        Uint16 manufacturing_lvl;
        Uint16 max_manufacturing_lvl;
        Uint16 harvest_lvl;
        Uint16 max_harvest_lvl;
        Uint16 alchemy_lvl;
        Uint16 max_alchemy_lvl;
        Uint16 overall_lvl;
        Uint16 max_overall_lvl;
        Uint16 attack_lvl;
        Uint16 max_attack_lvl;
        Uint16 defence_lvl;
        Uint16 max_defence_lvl;
        Uint16 magic_lvl;
        Uint16 max_magic_lvl;
        Uint16 potion_lvl;
        Uint16 max_potion_lvl;

        Uint16 inventory_emu;
        Uint16 max_inventory_emu;
        Uint16 material_pts;
        Uint16 max_material_pts;
        Uint16 ethereal_pts;
        Uint16 max_ethereal_pts;
        Uint16 food_lvl;
        Uint16 elapsed_book_time;
        Uint16 unused;

        int manufacture_exp;
        int max_manufacture_exp;
        int harvest_exp;
        int max_harvest_exp;
        int alchemy_exp;
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
        Uint16 book_id;
        Uint16 max_book_time;
    }packet;

    int packet_length=sizeof(packet);

    //clear the struct
    memset(&packet, '0', packet_length);

    //add data
    packet.protocol=HERE_YOUR_STATS;
    packet.data_length=packet_length-2;

    packet.physique_pp=clients.client[connection].physique_pp;
    packet.max_physique=clients.client[connection].max_physique;
    packet.coordination_pp=clients.client[connection].coordination_pp;
    packet.max_coordination=clients.client[connection].max_coordination;
    packet.reasoning_pp=clients.client[connection].reasoning_pp;
    packet.max_reasoning=clients.client[connection].max_reasoning;
    packet.will_pp=clients.client[connection].will_pp;
    packet.max_will=clients.client[connection].max_will;
    packet.instinct_pp=clients.client[connection].instinct_pp;
    packet.max_instinct=clients.client[connection].max_instinct;
    packet.vitality_pp=clients.client[connection].vitality_pp;
    packet.max_vitality=clients.client[connection].max_vitality;

    packet.human=clients.client[connection].human;
    packet.max_human=clients.client[connection].max_human;
    packet.animal=clients.client[connection].animal;
    packet.max_animal=clients.client[connection].max_animal;
    packet.vegetal=clients.client[connection].vegetal;
    packet.max_vegetal=clients.client[connection].max_vegetal;
    packet.inorganic=clients.client[connection].inorganic;
    packet.max_inorganic=clients.client[connection].max_inorganic;
    packet.artificial=clients.client[connection].artificial;
    packet.max_artificial=clients.client[connection].max_artificial;
    packet.magic=clients.client[connection].magic;
    packet.max_magic=clients.client[connection].max_magic;

    packet.inventory_emu=get_inventory_emu(connection);
    packet.max_inventory_emu=get_max_inventory_emu(connection);
    packet.material_pts=clients.client[connection].material_pts;
    packet.max_material_pts=clients.client[connection].max_material_pts;
    packet.ethereal_pts=clients.client[connection].ethereal_pts;
    packet.max_ethereal_pts=clients.client[connection].max_ethereal_pts;
    packet.food_lvl=clients.client[connection].food_lvl;
    packet.elapsed_book_time=clients.client[connection].elapsed_book_time;
    packet.unused=0;

    packet.manufacture_exp=clients.client[connection].manufacture_exp;
    packet.max_manufacture_exp=clients.client[connection].max_manufacture_exp;
    packet.harvest_exp=clients.client[connection].harvest_exp;
    packet.max_harvest_exp=clients.client[connection].max_harvest_exp;
    packet.alchemy_exp=clients.client[connection].alchemy_exp;
    packet.overall_exp=clients.client[connection].overall_exp;
    packet.max_overall_exp=clients.client[connection].max_overall_exp;
    packet.attack_exp=clients.client[connection].attack_exp;
    packet.max_attack_exp=clients.client[connection].max_attack_exp;
    packet.defence_exp=clients.client[connection].defence_exp;
    packet.max_defence_exp=clients.client[connection].max_defence_exp;
    packet.magic_exp=clients.client[connection].magic_exp;
    packet.max_magic_exp=clients.client[connection].max_magic_exp;
    packet.potion_exp=clients.client[connection].potion_exp;
    packet.max_potion_exp=clients.client[connection].max_potion_exp;
    packet.book_id=clients.client[connection].book_id;
    packet.max_book_time=clients.client[connection].max_book_time;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void send_change_map(int connection, char *elm_filename){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        char elm_filename[1024];
    }packet;

    //clear the struct
    memset(&packet, 0, sizeof(packet));

    //The struct size includes a reserve of 1024 for the map name.
    //We therefore calculate the actual packet length by taking the
    //struct size less the 1024 reserved for the map name and then
    //add on the actual message length
    int packet_length=sizeof(packet) - 1024 + strlen(elm_filename)+1;

    //add data
    packet.protocol=CHANGE_MAP;
    packet.data_length=packet_length-2;
    strcpy(packet.elm_filename, elm_filename);

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */

  struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 connection;
        Uint16 x_pos;
        Uint16 y_pos;
        Uint16 z_pos;
        Uint16 rot;
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
        Uint16 max_health;
        Uint16 current_health;
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
    int guild_tag_colour=guilds.guild[clients.client[connection].guild_id].guild_tag_colour;

    int banner_length=sprintf(_packet1.banner, "%s %c%s",
        clients.client[connection].char_name,
        127+guild_tag_colour,
        guilds.guild[clients.client[connection].guild_id].guild_tag);

    //calculate the total packet length
    *packet_length=sizeof(_packet1) - 80 + banner_length+1 + sizeof(_packet2);

    //add data to packet 1
    _packet1.protocol=ADD_NEW_ENHANCED_ACTOR;
    _packet1.connection=connection;
    _packet1.data_length=*packet_length-2;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    _packet1.x_pos=clients.client[connection].map_tile % map_axis;
    _packet1.y_pos=clients.client[connection].map_tile / map_axis;
    _packet1.z_pos=0; //z position (set to 0 pending further development)
    _packet1.rot=45; //rotation angle (set to 45 pending further development)

    _packet1.char_type=clients.client[connection].char_type;
    _packet1.unused=0; //unused (set to 0)

    _packet1.skin_type=clients.client[connection].skin_type;
    _packet1.hair_type=clients.client[connection].hair_type;
    _packet1.shirt_type=clients.client[connection].shirt_type;
    _packet1.pants_type=clients.client[connection].pants_type;
    _packet1.boots_type=clients.client[connection].boots_type;
    _packet1.head_type=clients.client[connection].head_type;
    _packet1.shield_type=clients.client[connection].shield_type;
    _packet1.weapon_type=clients.client[connection].weapon_type;
    _packet1.cape_type=clients.client[connection].cape_type;
    _packet1.helmet_type=clients.client[connection].helmet_type;
    _packet1.frame_type=clients.client[connection].frame;
    _packet1.max_health=clients.client[connection].max_health;
    _packet1.current_health=clients.client[connection].current_health;
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


void remove_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 connection;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet 1
    _packet.protocol=REMOVE_ACTOR;
    _packet.data_length=*packet_length-2;
    _packet.connection=connection;

    memcpy(packet, &_packet, sizeof(_packet));
}


void add_actor_packet(int connection, unsigned char move, unsigned char *packet, int *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 connection;
        unsigned char move;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet 1
    _packet.protocol=ADD_ACTOR;
    _packet.data_length=*packet_length-2;
    _packet.connection=connection;
    _packet.move=move;

    memcpy(packet, &_packet, sizeof(_packet));
}


void send_get_new_inventory_item(int connection, int object_id, int amount, int slot){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 object_id;
        int amount;
        unsigned char slot;
        unsigned char flags;
    }packet;

    //calculate the packet length
    int packet_length=sizeof(packet);

    //clear the structs
    memset(&packet, '0', sizeof(packet));

    //add data to packet
    packet.protocol=GET_NEW_INVENTORY_ITEM;
    packet.data_length=packet_length-2;
    packet.object_id=object_id;
    packet.amount=amount;
    packet.slot=slot;
    packet.flags=0;

    send_packet(connection, &packet, packet_length);
    //send(connection, &packet, packet_length, 0);
}


void get_new_bag_packet(int connection, int bag_list_number, unsigned char *packet, int *packet_length){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        Uint16 x_pos;
        Uint16 y_pos;
        unsigned char bag_list_number;
    }_packet;

    //calculate the packet length
    *packet_length=sizeof(_packet);

    //clear the structs
    memset(&_packet, '0', sizeof(_packet));

    //add data to packet
    _packet.protocol=GET_NEW_BAG;
    _packet.data_length=*packet_length-2;
    _packet.x_pos=get_x_pos(clients.client[connection].map_tile, clients.client[connection].map_id);
    _packet.y_pos=get_y_pos(clients.client[connection].map_tile, clients.client[connection].map_id);
    _packet.bag_list_number=bag_list_number;

    memcpy(packet, &_packet, sizeof(_packet));
}
