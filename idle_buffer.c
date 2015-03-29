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

#include <stdlib.h> //supports NULL
#include <stdio.h> //supports sprintf
#include <string.h> //supports strcpy

#include "idle_buffer.h"
#include "logging.h"
#include "database_functions.h"
#include "server_start_stop.h"
#include "log_in.h"
#include "db_character_tbl.h"
#include "string_functions.h"
#include "global.h"
#include "colour.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "server_protocol.h"
#include "client_protocol_handler.h"
#include "characters.h"
#include "game_data.h"
#include "character_race.h"
#include "test.h"

void db_push_buffer(char *sql, int connection, int process_type, unsigned char *packet){

    /** public function - see header **/

    if(idle_buffer.buffer_count<IDLE_BUFFER_MAX){

        //add to buffer
        idle_buffer.buffer_count++;

        strcpy(idle_buffer.buffer[idle_buffer.buffer_count].sql, sql);
        idle_buffer.buffer[idle_buffer.buffer_count].connection=connection;
        idle_buffer.buffer[idle_buffer.buffer_count].process_type=process_type;

        if(packet!=NULL){

            int packet_length=packet[1]+(packet[2]*256)-1+3;

            int i=0;
            for(i=0; i<packet_length; i++){

                if(i>MAX_PROTOCOL_PACKET_SIZE-1){

                    log_event(EVENT_ERROR, "maximum protocol packet size exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                    stop_server();
                }

                idle_buffer.buffer[idle_buffer.buffer_count].packet[i]=packet[i];
            }
        }
    }
    else {

        //buffer overflow
        log_event(EVENT_ERROR, "database buffer overflow in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

}

void db_process_buffer(){

    /** public function - see header **/

    //make sure we have something in the buffer to process
    if(idle_buffer.buffer_count>0){

        char text_out[1024]="";
        int i=0;

        int connection=idle_buffer.buffer[1].connection;
        int packet_length=idle_buffer.buffer[1].packet[1] + (idle_buffer.buffer[1].packet[2] * 256) + 2;

        unsigned char packet[1024]={0};

        i=0;
        for(i=0; i<packet_length; i++){
            packet[i]=idle_buffer.buffer[1].packet[i];
        }

        //use else if structure rather than switch, as this allows us to encapsulate
        //variables within each if statement
 /**********************************************************************************************/

        if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_CHECK_NEWCHAR){

            //When a new character is created, check name first
            int data_length=packet[1] + (packet[2] * 256);

            //place the packet in a union so we can extract individual bytes
            union {
                unsigned char buf[packet_length];

                struct {
                    unsigned char protocol;
                    unsigned char lsb;
                    unsigned char msb;
                    char name_and_password[data_length-8];
                    unsigned char skin;
                    unsigned char hair;
                    unsigned char shirt;
                    unsigned char pants;
                    unsigned char boots;
                    unsigned char type;
                    unsigned char head;
                }output;

            }convert;

            memcpy(convert.buf, packet, packet_length);

            char char_name[80]="";
            char password[80]="";
            get_str_island(convert.output.name_and_password, char_name, 1);
            get_str_island(convert.output.name_and_password, password, 2);

            if(get_db_char_data(char_name)==FOUND){

                //if the char name is found, warn about duplicate
                char text_out[1024]="";
                sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
                send_raw_text(connection, CHAT_SERVER, text_out);

                send_create_char_not_ok(connection);

                log_event(EVENT_SESSION, "Attempt to create new char with existing char name [%s]", char_name);
            }
            else {

                //if char name not found, add an entry to the database buffer to create a new char
                db_push_buffer("", connection, IDLE_BUFFER_PROCESS_ADD_NEWCHAR, packet);
            }
        }
/**********************************************************************************************/

        else if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_ADD_NEWCHAR){

            //when a new character is created, after name has been checked, add to game
            int data_length=packet[1] + (packet[2] * 256);

            //place the packet in a union so we can extract individual bytes
            union {
                unsigned char buf[packet_length];

                struct {
                    unsigned char protocol;
                    unsigned char lsb;
                    unsigned char msb;
                    char name_and_password[data_length-8];
                    unsigned char skin;
                    unsigned char hair;
                    unsigned char shirt;
                    unsigned char pants;
                    unsigned char boots;
                    unsigned char type;
                    unsigned char head;
                }output;

            }convert;

            memcpy(convert.buf, packet, packet_length);

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

            //set starting channel
            character.active_chan=32; // slot 0
            character.chan[0]=1; //nub chan

            //set starting map and tile
            character.map_id=game_data.beam_map_id;
            character.map_tile=game_data.beam_map_tile;

            //add character entry to database and get the record id
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
            sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
            send_raw_text(connection, CHAT_SERVER, text_out);
            send_create_char_ok(connection);

            //log character creation event
            log_event(EVENT_NEW_CHAR, "[%s] password [%s]\n", character.char_name, character.password);
        }
/**********************************************************************************************/

        else if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_LOGIN){

            process_log_in(connection, packet);
         }
/**********************************************************************************************/

        else if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_SQL){

            process_sql(idle_buffer.buffer[1].sql);
        }
/**********************************************************************************************/

        else {

            log_event(EVENT_ERROR, "unknown process type in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        //compress buffer
        i=0;
        for(i=1; i<=idle_buffer.buffer_count-1; i++){

            strcpy(idle_buffer.buffer[i].sql, idle_buffer.buffer[i+1].sql);

            idle_buffer.buffer[i].connection=idle_buffer.buffer[i+1].connection;
            idle_buffer.buffer[i].process_type=idle_buffer.buffer[i+1].process_type;

            if(idle_buffer.buffer[i+1].packet!=NULL){

                //memcpy is unreliable hence copy data bytes across individually
                int j=0;
                for(j=0; j<packet_length; j++){
                    idle_buffer.buffer[i].packet[j]=idle_buffer.buffer[i+1].packet[j];
                }
            }
        }

        //clear last entry in database buffer
        memset(&idle_buffer.buffer[i+1], 0, sizeof(idle_buffer.buffer[i+1])) ;

        idle_buffer.buffer_count--;
    }
}
