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


#include <string.h> //support for memcpy
#include <stdio.h> //support for sprintf

#include "string_functions.h"
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


void check_new_character(int connection, unsigned char *packet){

    int packet_length=packet[1] + (packet[2] * 256) + 2;

    //create a union so we can convert the packet array into easily accessible individual data elements
    union {

        unsigned char buf[packet_length];

        struct {
            unsigned char protocol;
            unsigned char lsb;
            unsigned char msb;
            char name_and_password[packet_length-10];
            unsigned char skin;
            unsigned char hair;
            unsigned char shirt;
            unsigned char pants;
            unsigned char boots;
            unsigned char type;
            unsigned char head;
        }output;

    }convert;

    //copy the packet into the union
    memcpy(convert.buf, packet, packet_length);

    //extract the character name from the union
    char char_name[80]="";
    get_str_island(convert.output.name_and_password, char_name, 1);

    //check if the character name already exists
    if(get_db_char_data(char_name)==FOUND){

        //if the char name exists, abort character creation and send warning to client
        char text_out[1024]="";
        sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
        send_raw_text(connection, CHAT_SERVER, text_out);

        send_create_char_not_ok(connection);

        log_event(EVENT_SESSION, "Attempt to create new char with existing char name [%s]", char_name);

        return;
    }
    else {

        //if char name does not exist, tell idle buffer to create a new character with that name
        db_push_buffer("", connection, IDLE_BUFFER_PROCESS_ADD_NEWCHAR, packet);
    }
}


void add_new_character(int connection, unsigned char *packet){

    int packet_length=packet[1] + (packet[2] * 256) + 2;

    //create a union so we can convert the packet array into easily accessible individual data elements
    union {

        unsigned char buf[packet_length];

        struct {
            unsigned char protocol;
            unsigned char lsb;
            unsigned char msb;
            char name_and_password[packet_length-10];
            unsigned char skin;
            unsigned char hair;
            unsigned char shirt;
            unsigned char pants;
            unsigned char boots;
            unsigned char type;
            unsigned char head;
        }output;

    }convert;

    //copy the packet into the union
    memcpy(convert.buf, packet, packet_length);

    //We pass character data to the database via the character struct, so lets clear it first
    memset(&character, 0, sizeof(character));

    //extract character data from the union and place in the character struct
    get_str_island(convert.output.name_and_password, character.char_name, 1);
    get_str_island(convert.output.name_and_password, character.password, 2);
    character.skin_type=convert.output.skin;
    character.hair_type=convert.output.hair;
    character.shirt_type=convert.output.shirt;
    character.pants_type=convert.output.pants;
    character.boots_type=convert.output.boots;
    character.char_type=convert.output.type;
    character.head_type=convert.output.head;

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

    //we use the add_db_char_data function to add character to the database. This function returns
    //an integer corresponding to the character_table id for the new record which, we'll need to
    //link to corresponding entries for the character in the inventory table
    clients.client[connection].character_id=add_db_char_data(character);

    //add initial items to inventory
    //int slot=0;
    //add_item_to_inventory(connection, 612, 1, &slot);
    //add_item_to_inventory(connection, 613, 1, &slot);
    //add_item_to_inventory(connection, 216, 1, &slot);
    //add_item_to_inventory(connection, 217, 1, &slot);

    //update game data
    race[character.char_type].char_count++;
    game_data.char_count++;
    strcpy(game_data.name_last_char_created, character.char_name);
    game_data.date_last_char_created=character.char_created;

    //notify client that character has been created
    char text_out[160]="";
    sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);
    send_create_char_ok(connection);

    //log character creation event
    log_event(EVENT_NEW_CHAR, "[%s] password [%s]\n", character.char_name, character.password);
}
