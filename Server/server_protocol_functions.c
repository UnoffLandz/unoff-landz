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

/*                     Creation of server protocol functions
                       -------------------------------------

Targeting of C99 compliance requires that two different approaches are used in coding server
protocol functions, The easiest way is to place the individual elements of the packet in a
struct and then extract the binary representation using a union. However, where a packet has a
variable length which must is reflected within a struct as a calculated field, this generates a
compiler warning. To avoid this warning, a build_packet function is provided which enables
variable length packets to be created based on a template and values supplied to that function
via the 'packet_element_type' struct.
*/

#include <string.h>     //support for memmove strlen
#include <sys/socket.h> //support for send function
#include <stdio.h>      //support for sprintf

#include "server_protocol.h"
#include "character_inventory.h"
#include "clients.h"
#include "maps.h"
#include "logging.h"
#include "packet.h"
#include "colour.h"
#include "string_functions.h"
#include "guilds.h"

#define DEBUG_SERVER_PROTOCOL_FUNCTIONS 0

void send_packet(int connection, unsigned char *packet, int packet_length){

    /** public function - see header */

    char text[1024]="";

    //capture packet details for logging
    int i=0;
    for(i=0; i<packet_length; i++){

        //use the safe version of snprintf to capture any over-runs
        ssnprintf(text, 1024, "%s %i", text, packet[i]);
    }

    log_event(EVENT_PACKET, "send to [%i]%s", connection, text);

    //send the packet
    send(connection, packet, packet_length, 0);
}


void send_new_minute(int connection, int minute){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char minute_lsb;
        unsigned char minute_msb;
    }packet_data;

    union {

        unsigned char out[5];
        packet_data in;
    }packet;

    packet.in.protocol=NEW_MINUTE;

    packet.in.lsb=3;
    packet.in.msb=0;
    packet.in.minute_lsb=minute % 256;
    packet.in.minute_msb=minute / 256;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("NEW_MINUTE connection [%i] minute [%i]\n", connection, minute);
    #endif

    log_event(EVENT_SESSION, "NEW_MINUTE connection [%i] minute [%i]", connection, minute);

    send_packet(connection, packet.out, 5);
}


void send_login_ok(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    union {

        unsigned char out[3];
        packet_data in;
    }packet;

    packet.in.protocol=LOG_IN_OK;
    packet.in.lsb=1;
    packet.in.msb=0;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("LOG_IN_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "LOG_IN_OK connection [%i]", connection);

    send_packet(connection, packet.out, 3);
}


void send_login_not_ok(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;


    union {

        unsigned char out[3];
        packet_data in;
    }packet;

    packet.in.protocol=LOG_IN_NOT_OK;
    packet.in.lsb=1;
    packet.in.msb=0;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("LOG_IN_NOT_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "LOG_IN_NOT_OK connection [%i]", connection);

    send_packet(connection, packet.out, 3);
}


void send_you_dont_exist(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    union {

        unsigned char out[3];
        packet_data in;
    }packet;

    packet.in.protocol=YOU_DONT_EXIST;
    packet.in.lsb=1;
    packet.in.msb=0;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("YOU_DONT_EXIST connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "YOU_DONT_EXIST connection [%i]", connection);

    send_packet(connection, packet.out, 3);
}


void send_you_are(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char lsb_connection;
        unsigned char msb_connection;
    }packet_data;

    union {

        unsigned char out[5];
        packet_data in;
    }packet;

    packet.in.protocol=YOU_ARE;
    packet.in.lsb=3;
    packet.in.msb=0;
    packet.in.lsb_connection=connection % 256;
    packet.in.msb_connection=connection / 256;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("YOU_ARE connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "YOU_ARE connection [%i]", connection);

    send_packet(connection, packet.out, 5);
}


void send_create_char_ok(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    union {

        unsigned char out[3];
        packet_data in;
    }packet;

    packet.in.protocol=CREATE_CHAR_OK;
    packet.in.lsb=1;
    packet.in.msb=0;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("CREATE_CHAR_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "CREATE_CHAR_OK connection [%i]", connection);

    send_packet(connection, packet.out, 3);
}


void send_create_char_not_ok(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    union {

        unsigned char out[3];
        packet_data in;
    }packet;

    packet.in.protocol=CREATE_CHAR_NOT_OK;
    packet.in.lsb=1;
    packet.in.msb=0;

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("CREATE_CHAR_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "CREATE_CHAR_NOT_OK connection [%i]", connection);

    send_packet(connection, packet.out, 3);
}


void send_raw_text(int connection, int channel, char *text){

    /** public function - see header */

    struct packet_element_type element[4];
    unsigned char packet[1024]={0};

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=RAW_TEXT;

    element[1].data_type=DATA_LENGTH;

    //channel
    element[2].data_type=BYTE;
    element[2].data.numeric=channel;

    //text
    element[3].data_type=STRING_NULL;
    strcpy(element[3].data.string, text);

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 4, packet);

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("RAW_TEXT connection [%i] channel [%i] text [%s]\n", connection, channel, text);
    #endif

    log_event(EVENT_SESSION, "RAW_TEXT connection [%i] channel [%i] text [%s]", connection, channel, text);

    send_packet(connection, packet, packet_length);
}


void send_here_your_inventory(int connection){

    /** public function - see header */

    struct packet_element_type element[3+(MAX_INVENTORY_SLOTS*4)];
    unsigned char packet[1024]={0};

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=HERE_YOUR_INVENTORY;

    element[1].data_type=DATA_LENGTH;

    //slot count
    element[2].data_type=BYTE;
    element[2].data.numeric=MAX_INVENTORY_SLOTS;

    //inventory slots
    int i=0;
    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        //item
        element[(i*4)+3].data_type=UINT16;
        element[(i*4)+3].data.numeric=clients.client[connection].client_inventory[i].image_id;

        //amount
        element[(i*4)+4].data_type=UINT32;
        element[(i*4)+4].data.numeric=clients.client[connection].client_inventory[i].amount;

        //slot number
        element[(i*4)+5].data_type=BYTE;
        element[(i*4)+5].data.numeric=i;

        //flag
        element[(i*4)+6].data_type=BYTE;
        element[(i*4)+6].data.numeric=0;
    }

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 3+(MAX_INVENTORY_SLOTS*4), packet);

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("HERE_YOUR_INVENTORY connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "HERE_YOUR_INVENTORY connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}


void send_get_active_channels(int connection){

    /** public function - see header */

    struct packet_element_type element[3+MAX_CHAN_SLOTS];
    unsigned char packet[1024]={0};

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=GET_ACTIVE_CHANNELS;

    element[1].data_type=DATA_LENGTH;

    //active channel
    element[2].data_type=BYTE;
    element[2].data.numeric=(int)channel;

    //channel numbers
    int i=0;
    for(i=0; i<MAX_CHAN_SLOTS; i++){

        element[i+3].data_type=UINT32;
        element[i+3].data.numeric=clients.client[connection].chan[i];
    }

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 3+MAX_CHAN_SLOTS, packet);

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("GET_ACTIVE_CHANNELS connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "GET_ACTIVE_CHANNELS connection [%i]", connection);

    send_packet(connection, packet, packet_length);
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

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("HERE_YOUR_STATS connection [%i]\n", connection);
    #endif

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 55, packet);

    log_event(EVENT_SESSION, "HERE_YOUR_STATS connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}


void send_change_map(int connection, char *elm_filename){

    /** public function - see header */

    struct packet_element_type element[3];
    unsigned char packet[1024]={0};

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=CHANGE_MAP;

    element[1].data_type=DATA_LENGTH;

    //map file name
    element[2].data_type=STRING_NULL;
    strcpy(element[2].data.string, elm_filename);

    //add one to the total elements as packet length runs from zero
    int packet_length=build_packet(element, 3, packet);

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("CHANGE MAP connection [%i] map [%s]\n", connection, elm_filename);
    #endif

    log_event(EVENT_SESSION, "CHANGE_MAP connection [%i] map [%s]", connection, elm_filename);

    send_packet(connection, packet, packet_length);
}


void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */

    struct packet_element_type element[28];

    element[0].data_type=PROTOCOL;
    element[0].data.numeric=ADD_NEW_ENHANCED_ACTOR;

    element[1].data_type=DATA_LENGTH;

    //char id (same as socket number)
    element[2].data_type=UINT16;
    element[2].data.numeric=connection;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;

    //position x axis
    element[3].data_type=UINT16;
    element[3].data.numeric=clients.client[connection].map_tile % map_axis;

    //position y axis
    element[4].data_type=UINT16;
    element[4].data.numeric=clients.client[connection].map_tile / map_axis;

    //position z axis (set to 0 pending further development)
    element[5].data_type=UINT16;
    element[5].data.numeric=0;

    //rotation angle (set to 45 pending further development)
    element[6].data_type=UINT16;
    element[6].data.numeric=45;

    //char type
    element[7].data_type=BYTE;
    element[7].data.numeric=clients.client[connection].char_type;

    //unused (set to 0)
    element[8].data_type=BYTE;
    element[8].data.numeric=0;

    //skin type
    element[9].data_type=BYTE;
    element[9].data.numeric=clients.client[connection].skin_type;

    //hair type
    element[10].data_type=BYTE;
    element[10].data.numeric=clients.client[connection].hair_type;

    //shirt type
    element[11].data_type=BYTE;
    element[11].data.numeric=clients.client[connection].shirt_type;

    //pants type
    element[12].data_type=BYTE;
    element[12].data.numeric=clients.client[connection].pants_type;

    //boots type
    element[13].data_type=BYTE;
    element[13].data.numeric=clients.client[connection].boots_type;

    //head type
    element[14].data_type=BYTE;
    element[14].data.numeric=clients.client[connection].head_type;

    //shield type
    element[15].data_type=BYTE;
    element[15].data.numeric=clients.client[connection].shield_type;

    //weapon type
    element[16].data_type=BYTE;
    element[16].data.numeric=clients.client[connection].weapon_type;

    //cape type
    element[17].data_type=BYTE;
    element[17].data.numeric=clients.client[connection].cape_type;

    //helmet type
    element[18].data_type=BYTE;
    element[18].data.numeric=clients.client[connection].helmet_type;

    //frame type
    element[19].data_type=BYTE;
    element[19].data.numeric=clients.client[connection].frame;

    //max health
    element[20].data_type=UINT16;
    element[20].data.numeric=clients.client[connection].max_health;

    //current health
    element[21].data_type=UINT16;
    element[21].data.numeric=clients.client[connection].current_health;

    //special (PLAYER=1 NPC=??)
    element[22].data_type=BYTE;
    element[22].data.numeric=1;

    //banner (char name and guild name separated by a space and 127+colour character)
    char banner[80]="";
    int guild_tag_colour=guilds.guild[clients.client[connection].guild_id].guild_tag_colour;
    sprintf(banner, "%s %c%s",
        clients.client[connection].char_name,
        127+guild_tag_colour,
        guilds.guild[clients.client[connection].guild_id].guild_tag);

    element[23].data_type=STRING_NULL;
    strcpy(element[23].data.string, banner);

    //unknown (set to 0 until we know what it is)
    element[24].data_type=BYTE;
    element[24].data.numeric=0;

    //char size (min=2 max=127)
    element[25].data_type=BYTE;
    element[25].data.numeric=64;

    //riding (nothing=255  brown horse=200)
    element[26].data_type=BYTE;
    element[26].data.numeric=255;

    //neck attachment (none=64)
    element[27].data_type=BYTE;
    element[27].data.numeric=64;

    //add one to the total elements as packet length runs from zero
    *packet_length=build_packet(element, 28, packet);
}


void send_add_new_enhanced_actor_packet(int connection, unsigned char *packet, int packet_length){

    /** public function - see header */

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_NEW_ENHANCED_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_NEW_ENHANCED_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}


void remove_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */

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
}


void send_remove_actor_packet(int connection, unsigned char *packet, int packet_length){

    /** public function - see header */

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_REMOVE_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_REMOVE_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}


void add_actor_packet(int connection, unsigned char move, unsigned char *packet, int *packet_length){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char connection_lsb;
        unsigned char connection_msb;
        unsigned char move;
    }packet_data;

    union {

        unsigned char out[6];
        packet_data in;
    }p;

    p.in.protocol=ADD_ACTOR;
    p.in.lsb=4;
    p.in.msb=0;
    p.in.connection_lsb=connection % 256;
    p.in.connection_msb=connection / 256;
    p.in.move=move;

    *packet_length=6;

    memcpy(packet, p.out, *packet_length);
}


void send_add_actor_packet(int connection, unsigned char *packet, int packet_length){

    /** public function - see header */

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("SEND_ADD_ACTOR_PACKET connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "SEND_ADD_ACTOR_PACKET connection [%i]", connection);

    send_packet(connection, packet, packet_length);
}
