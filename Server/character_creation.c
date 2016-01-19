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

#include "character_creation.h"

#include <string.h> //support for memcpy
#include <stdio.h> //support for sprintf
#include <stdint.h>  //support for uint_16 data type

#include "colour.h"
#include "server_protocol_functions.h"
#include "logging.h"
#include "server_messaging.h"
#include "idle_buffer2.h"
#include "db/db_character_tbl.h"
#include "characters.h"
#include "game_data.h"
#include "character_race.h"
#include "db/db_character_inventory_tbl.h"
#include "packet.h"
#include "character_race.h"
#include "character_type.h"
#include "gender.h"
#include "string_functions.h"
#include "server_start_stop.h"
#include "characters.h"
#include "movement.h"

#define DEBUG_CHARACTER_CREATION 0

void check_new_character(int actor_node, const unsigned char *packet){

    /** public function - see header */

    char char_name_and_password[MAX_CHAR_NAME_LEN + MAX_CHAR_PASSWORD_LEN]="";
    char char_name[MAX_CHAR_NAME_LEN]="";
    char password[MAX_CHAR_PASSWORD_LEN]="";

    size_t packet_length=get_packet_length(packet);

    //we know that there are always 3 bytes before the char name
    //and password and 7 bytes after, so we can extract this adding
    //3 to the packet start and subtracting 7 from packet end
    strncpy(char_name_and_password, (char*)packet+3, packet_length-7);

    //we know that the char name and password are separated by an
    //ascii space, so we can extract each separately by scanning
    //for two strings separated by a space
    if(sscanf(char_name_and_password, "%s %s", char_name, password)!=2){

        log_event(EVENT_ERROR, "failed to find character name in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    //check if the character name already exists
    if(get_db_char_data(char_name, 0)==true){

        //if the char name exists, abort character creation and send warning to client
        int socket=clients.client[actor_node].socket;

        send_text(socket, CHAT_SERVER, "%cSorry, but that character name already exists", c_red1+127);
        send_create_char_not_ok(socket);

        log_event(EVENT_NEW_CHAR, "new character name [%s] rejected", char_name);
    }
    else {

        //if char name does not exist, tell idle buffer to create a new character with that name
        push_idle_buffer2(actor_node, IDLE_BUFFER_PROCESS_ADD_NEWCHAR, packet, packet_length);

        log_event(EVENT_NEW_CHAR, "new character name [%s] accepted. Proceed to add to db", char_name);
    }
}


void add_new_character(int actor_node, const unsigned char *packet){

    /** public function - see header */

    struct __attribute__((__packed__)){

        unsigned char protocol;
        uint16_t data_length;
        char char_name_and_password[MAX_CHAR_NAME_LEN + MAX_CHAR_PASSWORD_LEN];
    }_packet_1;

    struct __attribute__((__packed__)){

        unsigned char skin_type;
        unsigned char hair_type;
        unsigned char shirt_type;
        unsigned char pants_type;
        unsigned char boots_type;
        unsigned char char_type;
        unsigned char head_type;
    }_packet_2;

    //calculate the total packet length
    size_t packet_length=get_packet_length(packet);

    //clear the structs
    memset(&_packet_1, '0', sizeof(_packet_1));
    memset(&_packet_2, '0', sizeof(_packet_2));

    //populate the structs
    memcpy(&_packet_1, packet, (size_t)packet_length-7);
    memcpy(&_packet_2, packet+packet_length-7, 7);

    //extract the char name and password
    if(sscanf(_packet_1.char_name_and_password, "%s %s", character.char_name, character.password)!=2){

        log_event(EVENT_ERROR, "failed to find character name in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    character.skin_type=_packet_2.skin_type;
    character.hair_type=_packet_2.hair_type;
    character.shirt_type=_packet_2.shirt_type;
    character.pants_type=_packet_2.pants_type;
    character.boots_type=_packet_2.boots_type;
    character.char_type=_packet_2.char_type;
    character.head_type=_packet_2.head_type;

    //set the char to stand
    character.frame=frame_stand;

    //set the char creation time
    character.char_created=time(NULL);

    //set starting channels
    int j=0;
    for(int i=0; i<MAX_CHANNELS; i++){

        if(channel[i].new_chars==1){

            if(j<MAX_CHAN_SLOTS){

                if(j==0) {

                    character.active_chan=i;
                    character.chan[j]=i;
                }
            }
        }
    }

    //set starting map and tile
    character.map_id=game_data.beam_map_id;
    character.map_tile=get_nearest_unoccupied_tile(game_data.beam_map_id, game_data.beam_map_tile);

    //add character data to the database and retrieve the database entry for the character
    character.character_id=add_db_char_data(character);

    //TEST CODE -- add initial items to inventory
    //character.client_inventory[0].slot=1; //slots run from 1
    //character.client_inventory[0].image_id=612;
    //character.client_inventory[0].amount=10;

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
    int socket=clients.client[actor_node].socket;

    send_text(socket, CHAT_SERVER, "%cCongratulations. You've created your new game character.", c_green3+127);
    send_create_char_ok(socket);

    //log character creation event
    log_event(EVENT_NEW_CHAR, "new character created: name [%s] password [%s]", character.char_name, character.password);
}
