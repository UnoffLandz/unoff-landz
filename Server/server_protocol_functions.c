/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

    int i=0;
    unsigned char packet[(MAX_INVENTORY_SLOTS*8)+4];

    int data_length=2+(MAX_INVENTORY_SLOTS*8);
    int j;

    packet[0]=HERE_YOUR_INVENTORY;
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;

    packet[3]=MAX_INVENTORY_SLOTS;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        j=4+(i*8);

        packet[j+0]=clients.client[connection].client_inventory[i].image_id % 256; //image_id of item
        packet[j+1]=clients.client[connection].client_inventory[i].image_id / 256;

        packet[j+2]=clients.client[connection].client_inventory[i].amount % 256; //amount (when zero nothing is shown in inventory)
        packet[j+3]=clients.client[connection].client_inventory[i].amount / 256 % 256;
        packet[j+4]=clients.client[connection].client_inventory[i].amount / 256 / 256 % 256;
        packet[j+5]=clients.client[connection].client_inventory[i].amount / 256 / 256 / 256 % 256;

        packet[j+6]=i; //inventory pos (starts at 0)
        packet[j+7]=0; //flags
    }

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("HERE_YOUR_INVENTORY connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "HERE_YOUR_INVENTORY connection [%i]", connection);

    send_packet(connection, packet, (MAX_INVENTORY_SLOTS*8)+4);
}


void send_get_active_channels(int connection){

    /** public function - see header */

    typedef struct {

        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char active_channel;
        int channel_slot[MAX_CHAN_SLOTS];
    }packet_data;

    //define the packet length as this allows the use of a calculated field within a union
    //without creating a compiler warning so that we have the flexibility to compile for
    //variable numbers of channel slots simply by changing the value of MAX_CHAN_SLOTS
    #define GET_ACTIVE_CHANNELS_PACKET_LENGTH MAX_CHAN_SLOTS+2

    union {

        unsigned char out[GET_ACTIVE_CHANNELS_PACKET_LENGTH];
        packet_data in;
    }packet;

    packet.in.protocol=GET_ACTIVE_CHANNELS;
    packet.in.lsb=(GET_ACTIVE_CHANNELS_PACKET_LENGTH-2) % 256;
    packet.in.msb=(GET_ACTIVE_CHANNELS_PACKET_LENGTH-2) / 256;
    packet.in.active_channel=clients.client[connection].active_chan;

    int i=0;
    for(i=0; i<MAX_CHAN_SLOTS; i++){

        packet.in.channel_slot[i]=clients.client[connection].chan[i];
    }

    send_packet(connection, packet.out, GET_ACTIVE_CHANNELS_PACKET_LENGTH);
}


void send_here_your_stats(int connection){

    /** public function - see header */

    unsigned char packet[1024];

    int i=0;

    packet[0]=HERE_YOUR_STATS;
    packet[1]=227;
    packet[2]=0;

    for(i=3; i<230; i++){
        packet[i]=0;
    }

    packet[3]=clients.client[connection].physique_pp % 256;
    packet[4]=clients.client[connection].physique_pp / 256;
    packet[5]=clients.client[connection].max_physique % 256;
    packet[6]=clients.client[connection].max_physique / 256;
    packet[7]=clients.client[connection].coordination_pp % 256;
    packet[8]=clients.client[connection].coordination_pp / 256;
    packet[9]=clients.client[connection].max_coordination % 256;
    packet[10]=clients.client[connection].max_coordination / 256;
    packet[11]=clients.client[connection].reasoning_pp % 256;
    packet[12]=clients.client[connection].reasoning_pp / 256;
    packet[13]=clients.client[connection].max_reasoning % 256;
    packet[14]=clients.client[connection].max_reasoning / 256;
    packet[15]=clients.client[connection].will_pp % 256;
    packet[16]=clients.client[connection].will_pp / 256;
    packet[17]=clients.client[connection].max_will % 256;
    packet[18]=clients.client[connection].max_will / 256;
    packet[19]=clients.client[connection].instinct_pp % 256;
    packet[20]=clients.client[connection].instinct_pp / 256;
    packet[21]=clients.client[connection].max_instinct % 256;
    packet[22]=clients.client[connection].max_instinct / 256;
    packet[23]=clients.client[connection].vitality_pp % 256;
    packet[24]=clients.client[connection].vitality_pp / 256;
    packet[25]=clients.client[connection].max_vitality % 256;
    packet[26]=clients.client[connection].max_vitality / 256;

    packet[27]=clients.client[connection].human % 256;
    packet[28]=clients.client[connection].human / 256;
    packet[29]=clients.client[connection].max_human % 256;
    packet[30]=clients.client[connection].max_human / 256;
    packet[31]=clients.client[connection].animal % 256;
    packet[32]=clients.client[connection].animal / 256;
    packet[33]=clients.client[connection].max_animal % 256;
    packet[34]=clients.client[connection].max_animal / 256;
    packet[35]=clients.client[connection].vegetal % 256;
    packet[36]=clients.client[connection].vegetal / 256;
    packet[37]=clients.client[connection].max_vegetal % 256;
    packet[38]=clients.client[connection].max_vegetal / 256;
    packet[39]=clients.client[connection].inorganic % 256;
    packet[40]=clients.client[connection].inorganic / 256;
    packet[41]=clients.client[connection].max_inorganic % 256;
    packet[42]=clients.client[connection].max_inorganic / 256;
    packet[43]=clients.client[connection].artificial % 256;
    packet[44]=clients.client[connection].artificial / 256;
    packet[45]=clients.client[connection].max_artificial % 256;
    packet[46]=clients.client[connection].max_artificial / 256;
    packet[47]=clients.client[connection].magic % 256;
    packet[48]=clients.client[connection].magic / 256;
    packet[49]=clients.client[connection].max_magic % 256;
    packet[50]=clients.client[connection].max_magic / 256;

    packet[51]=clients.client[connection].manufacturing_lvl % 256;
    packet[52]=clients.client[connection].manufacturing_lvl / 256;
    packet[53]=clients.client[connection].max_manufacturing_lvl % 256;
    packet[54]=clients.client[connection].max_manufacturing_lvl / 256;
    packet[55]=clients.client[connection].harvest_lvl % 256;
    packet[56]=clients.client[connection].harvest_lvl / 256;
    packet[57]=clients.client[connection].max_harvest_lvl % 256;
    packet[58]=clients.client[connection].max_harvest_lvl / 256;
    packet[59]=clients.client[connection].alchemy_lvl % 256;
    packet[60]=clients.client[connection].alchemy_lvl / 256;
    packet[61]=clients.client[connection].max_alchemy_lvl % 256;
    packet[62]=clients.client[connection].max_alchemy_lvl / 256;
    packet[63]=clients.client[connection].overall_lvl % 256;
    packet[64]=clients.client[connection].overall_lvl / 256;
    packet[65]=clients.client[connection].max_overall_lvl % 256;
    packet[66]=clients.client[connection].max_overall_lvl / 256;
    packet[67]=clients.client[connection].attack_lvl % 256;
    packet[68]=clients.client[connection].attack_lvl / 256;
    packet[69]=clients.client[connection].max_attack_lvl % 256;
    packet[70]=clients.client[connection].max_attack_lvl / 256;
    packet[71]=clients.client[connection].defence_lvl % 256;
    packet[72]=clients.client[connection].defence_lvl / 256;
    packet[73]=clients.client[connection].max_defence_lvl % 256;
    packet[74]=clients.client[connection].max_defence_lvl / 256;
    packet[75]=clients.client[connection].magic_lvl % 256;
    packet[76]=clients.client[connection].magic_lvl / 256;
    packet[77]=clients.client[connection].max_magic_lvl % 256;
    packet[78]=clients.client[connection].max_magic_lvl / 256;
    packet[79]=clients.client[connection].potion_lvl % 256;
    packet[80]=clients.client[connection].potion_lvl / 256;
    packet[81]=clients.client[connection].max_potion_lvl % 256;
    packet[82]=clients.client[connection].max_potion_lvl / 256;

    int inventory_emu=get_inventory_emu(connection);
    packet[83]=inventory_emu % 256; // amount of emu in inventory
    packet[84]=inventory_emu / 256;

    int max_inventory_emu=get_max_inventory_emu(connection);
    packet[85]=max_inventory_emu % 256; // max emu that can be held in inventory
    packet[86]=max_inventory_emu / 256;

    packet[87]=clients.client[connection].material_pts % 256;
    packet[88]=clients.client[connection].material_pts / 256;
    packet[89]=clients.client[connection].max_material_pts % 256;
    packet[90]=clients.client[connection].max_material_pts / 256;

    packet[91]=clients.client[connection].ethereal_pts % 256;
    packet[92]=clients.client[connection].ethereal_pts / 256;
    packet[93]=clients.client[connection].max_ethereal_pts % 256;
    packet[94]=clients.client[connection].max_ethereal_pts / 256;

    packet[95]=clients.client[connection].food_lvl % 256;
    packet[96]=clients.client[connection].food_lvl / 256;
    packet[97]=clients.client[connection].elapsed_book_time % 256;
    packet[98]=clients.client[connection].elapsed_book_time / 256;

    //packet[99]=; Unused
    //packet[100]=; Unused

    packet[101]=clients.client[connection].manufacture_exp % 256;
    packet[102]=clients.client[connection].manufacture_exp / 256 % 256;
    packet[103]=clients.client[connection].manufacture_exp / 256 / 256 % 256;
    packet[104]=clients.client[connection].manufacture_exp / 256 / 256 / 256 % 256;
    packet[105]=clients.client[connection].max_manufacture_exp % 256;
    packet[106]=clients.client[connection].max_manufacture_exp / 256 % 256;
    packet[107]=clients.client[connection].max_manufacture_exp / 256 / 256 % 256;
    packet[108]=clients.client[connection].max_manufacture_exp / 256 / 256 / 256 % 256;
    packet[109]=clients.client[connection].harvest_exp % 256;
    packet[110]=clients.client[connection].harvest_exp / 256 % 256;
    packet[111]=clients.client[connection].harvest_exp / 256 / 256 % 256;
    packet[112]=clients.client[connection].harvest_exp / 256 / 256 / 256 % 256 ;
    packet[113]=clients.client[connection].max_harvest_exp % 256;
    packet[114]=clients.client[connection].max_harvest_exp / 256 % 256;
    packet[115]=clients.client[connection].max_harvest_exp / 256 / 256 % 256;
    packet[116]=clients.client[connection].max_harvest_exp / 256 / 256 / 256 % 256;
    packet[117]=clients.client[connection].alchemy_exp % 256;
    packet[118]=clients.client[connection].alchemy_exp / 256 % 256;
    packet[119]=clients.client[connection].alchemy_exp / 256 / 256 % 256;
    packet[120]=clients.client[connection].alchemy_exp / 256 / 256 / 256 % 256;
    packet[121]=clients.client[connection].max_alchemy_exp % 256;
    packet[122]=clients.client[connection].max_alchemy_exp / 256 % 256;
    packet[123]=clients.client[connection].max_alchemy_exp / 256 / 256 % 256;
    packet[124]=clients.client[connection].max_alchemy_exp / 256 / 256 / 256 % 256;
    packet[125]=clients.client[connection].overall_exp % 256;
    packet[126]=clients.client[connection].overall_exp / 256 % 256;
    packet[127]=clients.client[connection].overall_exp / 256 / 256 % 256;
    packet[128]=clients.client[connection].overall_exp / 256 / 256 / 256 % 256;
    packet[129]=clients.client[connection].max_overall_exp % 256;
    packet[130]=clients.client[connection].max_overall_exp / 256 % 256;
    packet[131]=clients.client[connection].max_overall_exp / 256 / 256 % 256;
    packet[132]=clients.client[connection].max_overall_exp / 256 / 256 / 256 % 256;
    packet[133]=clients.client[connection].attack_exp % 256;
    packet[134]=clients.client[connection].attack_exp / 256 % 256;
    packet[135]=clients.client[connection].attack_exp / 256 / 256 % 256;
    packet[136]=clients.client[connection].attack_exp / 256 / 256 / 256 % 256;
    packet[137]=clients.client[connection].max_attack_exp % 256;
    packet[138]=clients.client[connection].max_attack_exp / 256 % 256;
    packet[139]=clients.client[connection].max_attack_exp / 256 / 256 % 256;
    packet[140]=clients.client[connection].max_attack_exp / 256 / 256 / 256 % 256;
    packet[141]=clients.client[connection].defence_exp % 256;
    packet[142]=clients.client[connection].defence_exp / 256 % 256;
    packet[143]=clients.client[connection].defence_exp / 256 / 256 % 256;
    packet[144]=clients.client[connection].defence_exp / 256 / 256 / 256 % 256;
    packet[145]=clients.client[connection].max_defence_exp % 256;
    packet[146]=clients.client[connection].max_defence_exp / 256 % 256;
    packet[147]=clients.client[connection].max_defence_exp / 256 / 256 % 256;
    packet[148]=clients.client[connection].max_defence_exp / 256 / 256 / 256 % 256;
    packet[149]=clients.client[connection].magic_exp % 256;
    packet[150]=clients.client[connection].magic_exp / 256 % 256;
    packet[151]=clients.client[connection].magic_exp / 256 / 256 % 256;
    packet[152]=clients.client[connection].magic_exp / 256 / 256 / 256 % 256;
    packet[153]=clients.client[connection].max_magic_exp % 256;
    packet[154]=clients.client[connection].max_magic_exp / 256 % 256;
    packet[155]=clients.client[connection].max_magic_exp / 256 / 256 % 256;
    packet[156]=clients.client[connection].max_magic_exp / 256 / 256 / 256 % 256;
    packet[157]=clients.client[connection].potion_exp % 256;
    packet[158]=clients.client[connection].potion_exp / 256 % 256;
    packet[159]=clients.client[connection].potion_exp / 256 / 256 % 256;
    packet[160]=clients.client[connection].potion_exp / 256 / 256 / 256 % 256;
    packet[161]=clients.client[connection].max_potion_exp % 256;
    packet[162]=clients.client[connection].max_potion_exp / 256 % 256;
    packet[163]=clients.client[connection].max_potion_exp / 256 / 256 % 256;
    packet[164]=clients.client[connection].max_potion_exp / 256 / 256 / 256 % 256;

    packet[165]=clients.client[connection].book_id % 256;
    packet[166]=clients.client[connection].book_id / 256;
    packet[167]=clients.client[connection].max_book_time % 256;
    packet[168]=clients.client[connection].max_book_time / 256;

    //packet[169]=10; summoning lvl

    #if DEBUG_SERVER_PROTOCOL_FUNCTIONS==1
    printf("HERE_YOUR_STATS connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "HERE_YOUR_STATS connection [%i]", connection);

    send_packet(connection, packet, 229);
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
        guilds.guild[clients.client[connection].guild_id].guild_name);

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
