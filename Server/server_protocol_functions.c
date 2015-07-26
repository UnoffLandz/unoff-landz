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

#define DEBUG_SERVER_PROTOCOL_FUNCTIONS 0

void send_packet(int connection, unsigned char *packet, int packet_length){

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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
}


void send_raw_text(int connection, int channel, char *text){

    /** public function - see header */

    /* the send_raw_text packet contains a variable length element carrying the text message. As C99
    compliance prevents us having a calculated field within a struct declaration, we deal with this by
    using a struct to hold the fixed length data elements, and then combine this to an unsigned char
    array carrying the variable length element.*/

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
        unsigned char channel;
    }packet;

    //clear the struct
    memset(&packet, '0', sizeof(packet));

    int packet_length=sizeof(packet) + strlen(text)+1;

    //add data
    packet.protocol=RAW_TEXT;
    packet.data_length=packet_length-2;
    packet.channel=channel;

    unsigned char p[1024]={0};
    memcpy(p, &packet, sizeof(packet));
    memcpy(p+sizeof(packet), text, strlen(text));

    send(connection, &p, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
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

    send(connection, &packet, packet_length, 0);
}


void send_here_your_stats(int connection){

    /** public function - see header */

    struct packet_element_type element[55];
    unsigned char packet[1024]={0};

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=HERE_YOUR_STATS;

    element[1].data_type=DATA_LENGTH;

    //attributes
    element[2].data_type=UINT16;
    element[2].data.numeric=clients.client[connection].physique_pp;
    element[3].data_type=UINT16;
    element[3].data.numeric=clients.client[connection].max_physique;
    element[4].data_type=UINT16;
    element[4].data.numeric=clients.client[connection].coordination_pp;
    element[5].data_type=UINT16;
    element[5].data.numeric=clients.client[connection].max_coordination;
    element[6].data_type=UINT16;
    element[6].data.numeric=clients.client[connection].reasoning_pp;
    element[7].data_type=UINT16;
    element[7].data.numeric=clients.client[connection].max_reasoning;
    element[8].data_type=UINT16;
    element[8].data.numeric=clients.client[connection].will_pp;
    element[9].data_type=UINT16;
    element[9].data.numeric=clients.client[connection].max_will;
    element[10].data_type=UINT16;
    element[10].data.numeric=clients.client[connection].instinct_pp;
    element[11].data_type=UINT16;
    element[11].data.numeric=clients.client[connection].max_instinct;
    element[12].data_type=UINT16;
    element[12].data.numeric=clients.client[connection].vitality_pp;
    element[13].data_type=UINT16;
    element[13].data.numeric=clients.client[connection].max_vitality;

    //nexus
    element[14].data_type=UINT16;
    element[14].data.numeric=clients.client[connection].human;
    element[15].data_type=UINT16;
    element[15].data.numeric=clients.client[connection].max_human;
    element[16].data_type=UINT16;
    element[16].data.numeric=clients.client[connection].animal;
    element[17].data_type=UINT16;
    element[17].data.numeric=clients.client[connection].max_animal;
    element[18].data_type=UINT16;
    element[18].data.numeric=clients.client[connection].vegetal;
    element[19].data_type=UINT16;
    element[19].data.numeric=clients.client[connection].max_vegetal;
    element[20].data_type=UINT16;
    element[20].data.numeric=clients.client[connection].inorganic;
    element[21].data_type=UINT16;
    element[21].data.numeric=clients.client[connection].max_inorganic;
    element[22].data_type=UINT16;
    element[22].data.numeric=clients.client[connection].artificial;
    element[23].data_type=UINT16;
    element[23].data.numeric=clients.client[connection].max_artificial;
    element[24].data_type=UINT16;
    element[24].data.numeric=clients.client[connection].magic;
    element[25].data_type=UINT16;
    element[25].data.numeric=clients.client[connection].max_magic;

    //skills
    element[26].data_type=UINT16;
    element[26].data.numeric=clients.client[connection].manufacturing_lvl;
    element[27].data_type=UINT16;
    element[27].data.numeric=clients.client[connection].max_manufacturing_lvl;
    element[28].data_type=UINT16;
    element[28].data.numeric=clients.client[connection].harvest_lvl;
    element[27].data_type=UINT16;
    element[27].data.numeric=clients.client[connection].max_harvest_lvl;
    element[28].data_type=UINT16;
    element[28].data.numeric=clients.client[connection].alchemy_lvl;
    element[29].data_type=UINT16;
    element[29].data.numeric=clients.client[connection].max_alchemy_lvl;
    element[30].data_type=UINT16;
    element[30].data.numeric=clients.client[connection].overall_lvl;
    element[31].data_type=UINT16;
    element[31].data.numeric=clients.client[connection].max_overall_lvl;
    element[32].data_type=UINT16;
    element[32].data.numeric=clients.client[connection].attack_lvl;
    element[33].data_type=UINT16;
    element[33].data.numeric=clients.client[connection].max_attack_lvl;
    element[34].data_type=UINT16;
    element[34].data.numeric=clients.client[connection].defence_lvl;
    element[35].data_type=UINT16;
    element[35].data.numeric=clients.client[connection].max_defence_lvl;
    element[36].data_type=UINT16;
    element[36].data.numeric=clients.client[connection].magic_lvl;
    element[37].data_type=UINT16;
    element[37].data.numeric=clients.client[connection].max_magic_lvl;
    element[38].data_type=UINT16;
    element[38].data.numeric=clients.client[connection].potion_lvl;
    element[39].data_type=UINT16;
    element[39].data.numeric=clients.client[connection].max_potion_lvl;

    //sundry
    element[40].data_type=UINT16;
    element[40].data.numeric=get_inventory_emu(connection);
    element[41].data_type=UINT16;
    element[41].data.numeric=get_max_inventory_emu(connection);
    element[42].data_type=UINT16;
    element[42].data.numeric=clients.client[connection].material_pts;
    element[43].data_type=UINT16;
    element[43].data.numeric=clients.client[connection].max_material_pts;
    element[44].data_type=UINT16;
    element[44].data.numeric=clients.client[connection].ethereal_pts;
    element[45].data_type=UINT16;
    element[45].data.numeric=clients.client[connection].max_ethereal_pts;
    element[46].data_type=UINT16;
    element[46].data.numeric=clients.client[connection].food_lvl;
    element[47].data_type=UINT16;
    element[47].data.numeric=clients.client[connection].elapsed_book_time;
    element[38].data_type=UINT16;// unused
    element[38].data.numeric=0;

    //skills
    element[39].data_type=UINT32;
    element[39].data.numeric=clients.client[connection].manufacture_exp;
    element[40].data_type=UINT32;
    element[40].data.numeric=clients.client[connection].max_manufacture_exp;
    element[41].data_type=UINT32;
    element[41].data.numeric=clients.client[connection].harvest_exp;
    element[42].data_type=UINT32;
    element[42].data.numeric=clients.client[connection].max_harvest_exp;
    element[43].data_type=UINT32;
    element[43].data.numeric=clients.client[connection].alchemy_exp;
    element[43].data_type=UINT32;
    element[43].data.numeric=clients.client[connection].overall_exp;
    element[44].data_type=UINT32;
    element[44].data.numeric=clients.client[connection].max_overall_exp;
    element[45].data_type=UINT32;
    element[45].data.numeric=clients.client[connection].attack_exp;
    element[46].data_type=UINT32;
    element[46].data.numeric=clients.client[connection].max_attack_exp;
    element[47].data_type=UINT32;
    element[47].data.numeric=clients.client[connection].defence_exp;
    element[48].data_type=UINT32;
    element[48].data.numeric=clients.client[connection].max_defence_exp;
    element[49].data_type=UINT32;
    element[49].data.numeric=clients.client[connection].magic_exp;
    element[50].data_type=UINT32;
    element[50].data.numeric=clients.client[connection].max_magic_exp;
    element[51].data_type=UINT32;
    element[51].data.numeric=clients.client[connection].potion_exp;
    element[52].data_type=UINT32;
    element[52].data.numeric=clients.client[connection].max_potion_exp;
    element[53].data_type=UINT16;
    element[53].data.numeric=clients.client[connection].book_id;
    element[54].data_type=UINT16;
    element[54].data.numeric=clients.client[connection].max_book_time;

    //packet[169]=10; summoning lvl

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 55, packet);

    send_packet(connection, packet, packet_length);
}


void send_change_map(int connection, char *elm_filename){

    /** public function - see header */

    /* the send_change_map packet contains a variable length element carrying the map file name. As C99
    compliance prevents us having a calculated field within a struct declaration, we deal with this by
    using a struct to hold the fixed length data elements, and then combine this to an unsigned char
    array carrying the variable length element.*/

    struct __attribute__((__packed__)){

        unsigned char protocol;
        Uint16 data_length;
    }packet;

    //clear the struct
    memset(&packet, '0', sizeof(packet));

    int packet_length=sizeof(packet) + strlen(elm_filename)+1;

    //add data
    packet.protocol=CHANGE_MAP;
    packet.data_length=packet_length-2;

    unsigned char p[1024]={0};
    memcpy(p, &packet, sizeof(packet));
    memcpy(p+sizeof(packet), elm_filename, strlen(elm_filename));

    send(connection, &p, packet_length, 0);
}


void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */

    /* the add_new_enhanced_actor packet contains a variable length element carrying the character's
    name and guild (referred to as the 'banner'). As C99 compliance prevents us having a calculated
    field within a struct declaration, we deal with this by creating two structs (_packet1 and _packet2)
    to hold the fixed length data elements, and that we then proceed to join either side of an
    unsigned char array carrying the variable length element for the banner.*/

    //packet structure before the banner
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

    }_packet1;

    //packet structure following the banner
    struct __attribute__((__packed__)){

        unsigned char unknown;
        unsigned char char_size;
        unsigned char riding;
        unsigned char neck_attachment;
    }_packet2;

    //create a char array carrying the banner (char name and guild name separated by a space and
    //127+colour character)
    char banner[80]="";

    int guild_tag_colour=guilds.guild[clients.client[connection].guild_id].guild_tag_colour;

    sprintf(banner, "%s %c%s",
        clients.client[connection].char_name,
        127+guild_tag_colour,
        guilds.guild[clients.client[connection].guild_id].guild_tag);

    //calculate the total packet length
    *packet_length=sizeof(_packet1) + strlen(banner)+1 + sizeof(_packet2);

    //clear the structs
    memset(&_packet1, '0', sizeof(_packet1));
    memset(&_packet2, '0', sizeof(_packet2));

    //add data to packet 1
    _packet1.protocol=ADD_NEW_ENHANCED_ACTOR;
    _packet1.data_length=*packet_length-2;
    _packet1.connection=connection;

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
    memcpy(packet, &_packet1, sizeof(_packet1));
    memcpy(packet + sizeof(_packet1), banner, strlen(banner)+1);
    memcpy(packet + sizeof(_packet1)+ strlen(banner)+1, &_packet2, sizeof(_packet2));
}

/*
void send_add_new_enhanced_actor_packet(int connection, unsigned char *packet, int packet_length){

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_NEW_ENHANCED_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_NEW_ENHANCED_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}
*/

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

/*
    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char connection_lsb;
        unsigned char connection_msb;
    }packet_data;

    union {

        unsigned char out[5];
        packet_data in;
    }p;

    p.in.protocol=REMOVE_ACTOR;
    p.in.lsb=3;
    p.in.msb=0;
    p.in.connection_lsb=connection % 256;
    p.in.connection_msb=connection / 256;

    *packet_length=5;

    memcpy(packet, p.out, *packet_length);
*/
}

/*
void send_remove_actor_packet(int connection, unsigned char *packet, int packet_length){

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_REMOVE_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_REMOVE_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}
*/

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

/*
void send_add_actor_packet(int connection, unsigned char *packet, int packet_length){

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_ADD_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_ADD_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}
*/

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

    send(connection, &packet, packet_length, 0);
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


