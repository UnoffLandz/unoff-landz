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

#include "character_creation.h"

#include <string.h> //support for memcpy
#include <stdio.h> //support for sprintf

#include "global.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "logging.h"
#include "server_messaging.h"
#include "idle_buffer.h"
#include "db/db_character_tbl.h"
#include "characters.h"
#include "game_data.h"
#include "character_race.h"
#include "db/db_character_inventory_tbl.h"
#include "packet.h"
#include "character_race.h"
#include "character_type.h"
#include "gender.h"

#define DEBUG_CHARACTER_CREATION 0

void check_new_character(int connection, unsigned char *packet){

    /** public function - see header */

    struct packet_element_type element[11];

    element[0].data_type=PROTOCOL;
    element[1].data_type=DATA_LENGTH;
    element[2].data_type=STRING_SPACE; //char name
    element[3].data_type=STRING_NULL; //password
    element[4].data_type=BYTE; //skin
    element[5].data_type=BYTE; //hair
    element[6].data_type=BYTE; //shirt
    element[7].data_type=BYTE; //pants
    element[8].data_type=BYTE; //boots
    element[9].data_type=BYTE; //type
    element[10].data_type=BYTE; //head

    read_packet(element, 11, packet);

    #if DEBUG_CHARACTER_CREATION==1

    int pl1=packet[1]+(packet[2]*256)+2;

    printf("***Debug check_new_character function***\n");
    printf("packet data...");

    int i1=0;
    for(i1=0; i1<pl1; i1++) printf("%i ", packet[i1]);

    printf("\npacket bytes read...\n");
    printf("protocol     %i\n", element[0].data.numeric);
    printf("data length  %i\n", element[1].data.numeric);
    printf("name         %s\n", element[2].data.string);
    printf("password     %s\n", element[3].data.string);
    printf("skin  %i\n", element[4].data.numeric);
    printf("hair  %i\n", element[5].data.numeric);
    printf("shirt %i\n", element[6].data.numeric);
    printf("pants  %i\n", element[7].data.numeric);
    printf("boots  %i\n", element[8].data.numeric);
    printf("type %i\n", element[9].data.numeric);
    printf("head %i\n", element[9].data.numeric);
    #endif

    //check if the character name already exists
    if(get_db_char_data(element[2].data.string)==FOUND){

        //if the char name exists, abort character creation and send warning to client
        char text_out[1024]="";
        sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        send_create_char_not_ok(connection);

        log_event(EVENT_NEW_CHAR, "new character name [%s] rejected", element[2].data.string);
    }
    else {

        //if char name does not exist, tell idle buffer to create a new character with that name
        push_idle_buffer("", connection, IDLE_BUFFER_PROCESS_ADD_NEWCHAR, packet);

        log_event(EVENT_NEW_CHAR, "new character name [%s] accepted. Proceed to add to db", element[2].data.string);
    }
}


void add_new_character(int connection, unsigned char *packet){

    /** public function - see header */

    struct packet_element_type element[11];

    element[0].data_type=PROTOCOL;
    element[1].data_type=DATA_LENGTH;
    element[2].data_type=STRING_SPACE; //char name
    element[3].data_type=STRING_NULL; //password
    element[4].data_type=BYTE; //skin
    element[5].data_type=BYTE; //hair
    element[6].data_type=BYTE; //shirt
    element[7].data_type=BYTE; //pants
    element[8].data_type=BYTE; //boots
    element[9].data_type=BYTE; //type
    element[10].data_type=BYTE; //head

    read_packet(element, 11, packet);

    #if DEBUG_CHARACTER_CREATION==1

    int pl2=packet[1]+(packet[2]*256)+2;

    printf("***Debug add_new_character function***\n");
    printf("packet data...");

    int i2=0;
    for(i2=0; i2<pl2; i2++) printf("%i ", packet[i2]);

    printf("\npacket bytes read...\n");
    printf("protocol     %i\n", element[0].data.numeric);
    printf("data length  %i\n", element[1].data.numeric);
    printf("name         %s\n", element[2].data.string);
    printf("password     %s\n", element[3].data.string);
    printf("skin  %i\n", element[4].data.numeric);
    printf("hair  %i\n", element[5].data.numeric);
    printf("shirt %i\n", element[6].data.numeric);
    printf("pants  %i\n", element[7].data.numeric);
    printf("boots  %i\n", element[8].data.numeric);
    printf("type %i\n", element[9].data.numeric);
    printf("head %i\n", element[9].data.numeric);
    #endif

    //We pass character data to the database via the character struct, so lets clear it first
    memset(&character, 0, sizeof(character));

    //extract character data from the union and place in the character struct
    strcpy(character.char_name, element[2].data.string);
    strcpy(character.password, element[3].data.string);
    character.skin_type=element[4].data.numeric;
    character.hair_type=element[5].data.numeric;
    character.shirt_type=element[6].data.numeric;
    character.pants_type=element[7].data.numeric;
    character.boots_type=element[8].data.numeric;
    character.char_type=element[9].data.numeric;
    character.head_type=element[10].data.numeric;

    //set the char to stand
    character.frame=frame_stand;

    //set the char creation time
    character.char_created=time(NULL);

    //set starting channels
    int i=0, j=0;
    for(i=0; i<MAX_CHANNELS; i++){

        if(channel[i].new_chars==1){

            if(j<MAX_CHAN_SLOTS){

                if(j==0) character.active_chan=i-CHAT_CHANNEL0;
                character.chan[j]=i;
            }
        }
    }

    //set starting map and tile
    character.map_id=game_data.beam_map_id;
    character.map_tile=game_data.beam_map_tile;

    //add character data to the database and retrieve the database entry for the character
    character.character_id=add_db_char_data(character);


    //TEST CODE -- add initial items to inventory
    character.client_inventory[0].slot=1; //slots run from 1
    character.client_inventory[0].image_id=612;
    character.client_inventory[0].amount=10;

    add_db_char_inventory(character);

    //update game details
    game_data.char_count++;
    strcpy(game_data.name_last_char_created, character.char_name);
    game_data.date_last_char_created=character.char_created;

    //update game stats
    int race_id=character_type[character.char_type].race_id;
    race[race_id].char_count++;

    int gender_id=character_type[character.char_type].gender_id;
    gender[gender_id].char_count++;

    //notify client that character has been created
    char text_out[160]="";
    sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);
    send_create_char_ok(connection);

    //log character creation event
    log_event(EVENT_NEW_CHAR, "new character created: name [%s] password [%s]", character.char_name, character.password);
}
