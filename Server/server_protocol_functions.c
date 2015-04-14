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

#include <string.h> //support for memmove strlen
#include <sys/socket.h> //needed for send function
#include <stdio.h>

#include "server_protocol.h"
#include "character_inventory.h"
#include "clients.h"
#include "maps.h"
#include "logging.h"

#define DEBUG_SEND 0


void send_packet(int connection, unsigned char *packet, int packet_length){

    /** public function - see header */

    char text[1024]="";

    int i=0;
    for(i=0; i<packet_length; i++){

        sprintf(text, "%s %i", text, packet[i]);
    }

    log_event(EVENT_PACKET, "send to [%i]%s", connection, text);

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

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=NEW_MINUTE;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;
    packet.in.minute_lsb=minute % 256;
    packet.in.minute_msb=minute / 256;

    #if DEBUG_SEND==1
    printf("NEW_MINUTE connection [%i] minute [%i]\n", connection, minute);
    #endif

    log_event(EVENT_SESSION, "NEW_MINUTE connection [%i] minute [%i]", connection, minute);

    send_packet(connection, packet.out, packet_length);
}


void send_login_ok(int connection){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=LOG_IN_OK;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;

    #if DEBUG_SEND==1
    printf("LOG_IN_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "LOG_IN_OK connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
}


void send_login_not_ok(int connection){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=LOG_IN_NOT_OK;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;

    #if DEBUG_SEND==1
    printf("LOG_IN_NOT_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "LOG_IN_NOT_OK connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
}


void send_you_dont_exist(int connection){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=YOU_DONT_EXIST;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;

    #if DEBUG_SEND==1
    printf("YOU_DONT_EXIST connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "YOU_DONT_EXIST connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
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

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=YOU_ARE;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;
    packet.in.lsb_connection=connection % 256;
    packet.in.msb_connection=connection / 256;

    #if DEBUG_SEND==1
    printf("YOU_ARE connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "YOU_ARE connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
}


void send_create_char_ok(int connection){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    int packet_length=sizeof(packet_data);

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=CREATE_CHAR_OK;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;

    #if DEBUG_SEND==1
    printf("CREATE_CHAR_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "CREATE_CHAR_OK connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
}


void send_create_char_not_ok(int connection){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
    }packet_data;

    int packet_length=sizeof(packet_data)-1;

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=CREATE_CHAR_NOT_OK;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;

    #if DEBUG_SEND==1
    printf("CREATE_CHAR_OK connection [%i]\n", connection);
    #endif

    log_event(EVENT_SESSION, "CREATE_CHAR_NOT_OK connection [%i]", connection);

    send_packet(connection, packet.out, packet_length);
}


void send_raw_text(int connection, int channel, char *text){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char channel;
        char text[strlen(text)+1];
    }packet_data;

    int packet_length=sizeof(packet_data)-1;

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=RAW_TEXT;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;
    packet.in.channel=channel;
    strcpy(packet.in.text, text);

    #if DEBUG_SEND==1
    printf("RAW_TEXT connection [%i] channel [%i] text [%s]\n", connection, channel, text);
    #endif

    log_event(EVENT_SESSION, "RAW_TEXT connection [%i] channel [%i] text [%s]", connection, channel, text);

    send_packet(connection, packet.out, packet_length);
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

    int packet_length=sizeof(packet_data)-1;

    union {

        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=GET_ACTIVE_CHANNELS;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;
    packet.in.active_channel=clients.client[connection].active_chan;

    int i=0;
    for(i=0; i<MAX_CHAN_SLOTS; i++){

        packet.in.channel_slot[i]=clients.client[connection].chan[i];
    }

    #if DEBUG_SEND==1
    printf("GET_ACTIVE_CHANNELS connection [%i] active_channel slot [%i]\n", connection, clients.client[connection].active_chan);

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        printf("slot [%i] channel [%i]\n", i, clients.client[connection].chan[i]);
    }
    #endif

    log_event(EVENT_SESSION, "GET_ACTIVE_CHANNELS connection [%i] active channel slot [%i]", connection, clients.client[connection].active_chan);

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        log_text(EVENT_SESSION, "slot [%i] channel [%i]", i, clients.client[connection].chan[i]);
    }

    send_packet(connection, packet.out, packet_length);
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

    send_packet(connection, packet, 229);
}


void send_change_map(int connection, char *elm_filename){

    /** public function - see header */

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        char text[strlen(elm_filename)+1];
    }packet_data;

    int packet_length=sizeof(packet_data)-1;

    union {
        unsigned char out[packet_length];
        packet_data in;
    }packet;

    packet.in.protocol=CHANGE_MAP;
    packet.in.lsb=(packet_length-2) % 256;
    packet.in.msb=(packet_length-2) / 256;
    strcpy(packet.in.text, elm_filename);

    #if DEBUG_SEND==1
    printf("CHANGE MAP connection [%i] map [%s]\n", connection, elm_filename);
    #endif

    log_event(EVENT_SESSION, "CHANGE_MAP connection [%i] map [%s]", connection, elm_filename);
    send_packet(connection, packet.out, packet_length);
}


void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length){

    /** public function - see header */
/*
    int i=0,j=0;
    int data_length=0;
    //int guild_id=clients.client[connection].guild_id;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int x=clients.client[connection].map_tile % map_axis;
    int y=clients.client[connection].map_tile / map_axis;

    packet[i++]=ADD_NEW_ENHANCED_ACTOR;                               // protocol
    packet[i++]=0;                                                    // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;                                                    // dummy the msb (we'll put the proper value in later
    packet[i++]=connection % 256;                                     // char_id lsb
    packet[i++]=connection / 256;                                     // char_id msb
    packet[i++]=x % 256;                                              // x axis lsb  2
    packet[i++]=x / 256;                                              // x axis msb  3
    packet[i++]=y % 256;                                              // y axis lsb  4
    packet[i++]=y / 256;                                              // y axis msb  5
    packet[i++]=0;                                                    // z axis lsb  6
    packet[i++]=0;                                                    // z axis msb  7
    packet[i++]=45;                                                   // rotation lsb 8
	packet[i++]=0;                                                    // rotation msb 9

    packet[i++]=clients.client[connection].char_type;                //              10

    packet[i++]=0;                                                    // unknown
	packet[i++]=clients.client[connection].skin_type;
	packet[i++]=clients.client[connection].hair_type;
	packet[i++]=clients.client[connection].shirt_type;
	packet[i++]=clients.client[connection].pants_type;
	packet[i++]=clients.client[connection].boots_type;  //16
	packet[i++]=clients.client[connection].head_type;

	packet[i++]=clients.client[connection].shield_type;
	packet[i++]=clients.client[connection].weapon_type;
	packet[i++]=clients.client[connection].cape_type;
	packet[i++]=clients.client[connection].helmet_type; //21

    packet[i++]=clients.client[connection].frame;

	packet[i++]=clients.client[connection].max_health % 256;         // 23 max health lsb
	packet[i++]=clients.client[connection].max_health / 256;         // 24 max health msb
	packet[i++]=clients.client[connection].current_health % 256;     // 25 current health lsb
	packet[i++]=clients.client[connection].current_health / 256;     // 26 current health msb
	packet[i++]=1;                                                   // 27 special char type HUMAN / NPC

    // add char name to packet
	for(j=0;j< (int)strlen(clients.client[connection].char_name); j++){
            packet[i++]=clients.client[connection].char_name[j];
	}


    // add guild name
//	if(guild_id>0) {

//        packet[i++]=ASCII_SPACE;
//        packet[i++]=guilds.guild[guild_id]->tag_colour;

//        for(j=0; j< (int)strlen(guilds.guild[guild_id]->guild_tag); j++){
//	        packet[i++]=guilds.guild[guild_id]->guild_tag[j];
//        }
//	}

    packet[i++]='\0';

	packet[i++]=0; // unknown
	packet[i++]=64; //char height (min=2 max=127)
	packet[i++]=255; //char riding (none=255  brown horse=200)
	packet[i++]=64;// neck attachment (none=64 10=

    *packet_length=i;

    // now we know the packet length we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
*/

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char connection_lsb;
        unsigned char connection_msb;
        unsigned char x_axis_lsb;
        unsigned char x_axis_msb;
        unsigned char y_axis_lsb;
        unsigned char y_axis_msb;
        unsigned char z_axis_lsb;
        unsigned char z_axis_msb;
        unsigned char rotation_lsb;
        unsigned char rotation_msb;
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
        unsigned char frame;
        unsigned char max_health_lsb;
        unsigned char max_health_msb;
        unsigned char current_health_lsb;
        unsigned char current_health_msb;
        unsigned char special;

        char char_name[strlen(clients.client[connection].char_name)+1];
        char unknown;
        char char_height; // min=2 max=127
        char riding; // none=255  brown horse=200
        char neck_attachment; //none=64
    }packet_data;

    *packet_length=sizeof(packet_data);

    union {
        unsigned char out[*packet_length];
        packet_data in;
    }p;

    p.in.protocol=ADD_NEW_ENHANCED_ACTOR;

    p.in.lsb=(*packet_length-2) % 256;
    p.in.msb=(*packet_length-2) / 256;

    p.in.connection_lsb=connection % 256;
    p.in.connection_msb=connection / 256;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int x=clients.client[connection].map_tile % map_axis;
    p.in.x_axis_lsb=x % 256;
    p.in.x_axis_msb=x / 256;

    int y=clients.client[connection].map_tile / map_axis;
    p.in.y_axis_lsb=y % 256;
    p.in.y_axis_msb=y / 256;

    int z=0;
    p.in.z_axis_lsb=z % 256;
    p.in.z_axis_msb=z / 256;

    int rotation=45;
    p.in.rotation_lsb=rotation % 256;
    p.in.rotation_msb=rotation / 256;

    p.in.char_type=clients.client[connection].char_type;
    p.in.unused=0;
    p.in.skin_type=clients.client[connection].skin_type;
    p.in.hair_type=clients.client[connection].hair_type;
    p.in.shirt_type=clients.client[connection].shirt_type;
    p.in.pants_type=clients.client[connection].pants_type;
    p.in.boots_type=clients.client[connection].boots_type;
    p.in.head_type=clients.client[connection].head_type;
    p.in.shield_type=clients.client[connection].shield_type;
    p.in.weapon_type=clients.client[connection].weapon_type;
    p.in.cape_type=clients.client[connection].cape_type;
    p.in.helmet_type=clients.client[connection].helmet_type;
    p.in.frame=clients.client[connection].frame;

    p.in.max_health_lsb=clients.client[connection].max_health % 256;
    p.in.max_health_msb=clients.client[connection].max_health / 256;
    p.in.current_health_lsb=clients.client[connection].current_health % 256;
    p.in.current_health_msb=clients.client[connection].current_health / 256;
    p.in.special=1; //HUMAN / NPC

    strcpy(p.in.char_name, clients.client[connection].char_name);

    p.in.unknown=0;
    p.in.char_height=64; // min=2 max=127
    p.in.riding=255; // none=255  brown horse=200
    p.in.neck_attachment=64; //none=64

    memcpy(packet, p.out, *packet_length);
}


void remove_actor_packet(int connection, unsigned char *packet, int *packet_length){

/*
    int i;
    int data_length=0;

    i=0;                       // zero the packet length counter
    packet[i++]=REMOVE_ACTOR;             // protocol
    packet[i++]=0;             // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;             // dummy the msb (we'll put the proper value in later
    packet[i++]=connection % 256;      // char_id lsb
    packet[i++]=connection / 256;      // char_id msb

    *packet_length=i;

    // now we know the packet length we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
*/

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char connection_lsb;
        unsigned char connection_msb;
    }packet_data;

    *packet_length=sizeof(packet_data);

    union {
        unsigned char out[*packet_length];
        packet_data in;
    }p;

    p.in.protocol=REMOVE_ACTOR;
    p.in.lsb=(*packet_length-2) % 256;
    p.in.msb=(*packet_length-2) / 256;
    p.in.connection_lsb=connection % 256;
    p.in.connection_msb=connection / 256;

    memcpy(packet, p.out, *packet_length);
}


void add_actor_packet(int connection, unsigned char move, unsigned char *packet, int *packet_length){

    /** public function - see header */
/*
    int data_length=4;

    //construct the packet
    packet[0]=ADD_ACTOR;          // protocol
    packet[1]=data_length % 256;// dummy the lsb (we'll put the proper value in later
    packet[2]=data_length / 256;// dummy the msb (we'll put the proper value in later
    packet[3]=connection % 256;  // char_id lsb
    packet[4]=connection / 256;  // char_id msb
    packet[5]=move;               // actor command

    //return the packet length
    *packet_length=6;
*/

    typedef struct {
        unsigned char protocol;
        unsigned char lsb;
        unsigned char msb;
        unsigned char connection_lsb;
        unsigned char connection_msb;
        unsigned char move;
    }packet_data;

    *packet_length=sizeof(packet_data);

    union {
        unsigned char out[*packet_length];
        packet_data in;
    }p;

    p.in.protocol=ADD_ACTOR;
    p.in.lsb=(*packet_length-2) % 256;
    p.in.msb=(*packet_length-2) / 256;
    p.in.connection_lsb=connection % 256;
    p.in.connection_msb=connection / 256;
    p.in.move=move;

    memcpy(packet, p.out, *packet_length);
}
