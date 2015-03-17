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

#include "database_buffer.h"
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

void db_push_buffer(char *sql, int connection, int process_type, unsigned char *packet){

    /** public function - see header **/

    if(db_buffer.buffer_count<DB_BUFFER_MAX){


        //add to buffer
        db_buffer.buffer_count++;

        strcpy(db_buffer.buffer[db_buffer.buffer_count].sql, sql);
        db_buffer.buffer[db_buffer.buffer_count].connection=connection;
        db_buffer.buffer[db_buffer.buffer_count].process_type=process_type;

        if(packet!=NULL){

            int packet_length=packet[1]+(packet[2]*256)-1+3;

            int i=0;
            for(i=0; i<packet_length; i++){

                if(i>MAX_PROTOCOL_PACKET_SIZE-1){

                    log_event(EVENT_ERROR, "maximum protocol packet size exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                    stop_server();
                }

                db_buffer.buffer[db_buffer.buffer_count].packet[i]=packet[i];
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
    if(db_buffer.buffer_count>0){

        //use else if structure rather than switch, as this allows us to encapsulate
        //variables within each if statement

        if(db_buffer.buffer[1].process_type==DB_BUFFER_PROCESS_HASH_DETAILS){
        }

        if(db_buffer.buffer[1].process_type==DB_BUFFER_PROCESS_CHECK_NEWCHAR){

            char char_name[80]="";
            int i=0;
            int packet_length=db_buffer.buffer[1].packet[1]+(db_buffer.buffer[1].packet[2]*256)-1+3;

            //extract char name from data by looking for the ascii space separator between
            //the char name and password
            for(i=0; i<packet_length; i++){

                if(db_buffer.buffer[1].packet[i]==ASCII_SPACE){

                    char_name[i]=ASCII_NULL;// terminate with NULL to make this a valid text string
                    break;
                }

                char_name[i]=db_buffer.buffer[1].packet[i];
            }

            if(i==packet_length-1){

                //if ascii space cannot be found in data, the new char packet is malformed
                log_event(EVENT_ERROR, "detected malformed new character packet in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                stop_server();
            }

            if(get_db_char_data(char_name)==FOUND){

                //if the char name is found then warn about duplicate
                char text_out[1024]="";
                sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
                send_raw_text(db_buffer.buffer[1].connection, CHAT_SERVER, text_out);

                send_create_char_not_ok(db_buffer.buffer[1].connection);

                log_event(EVENT_SESSION, "Attempt to create new char with existing char name [%s]\n", char_name);
            }
            else {

                db_push_buffer("", db_buffer.buffer[1].connection, DB_BUFFER_PROCESS_ADD_NEWCHAR, db_buffer.buffer[1].packet);
            }
        }

        else if(db_buffer.buffer[1].process_type==DB_BUFFER_PROCESS_ADD_NEWCHAR){

        }

        else if(db_buffer.buffer[1].process_type==DB_BUFFER_PROCESS_LOGIN){

            process_log_in(db_buffer.buffer[1].connection, db_buffer.buffer[1].packet);
         }

        else if(db_buffer.buffer[1].process_type==DB_BUFFER_PROCESS_SQL){

            process_sql(db_buffer.buffer[1].sql);
        }

        else {

            log_event(EVENT_ERROR, "unknown process type in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        //compress buffer
        int packet_length=db_buffer.buffer[1].packet[1]+(db_buffer.buffer[1].packet[2]*256)-1+3;

        int i=0;
        for(i=1; i<=db_buffer.buffer_count-1; i++){

            strcpy(db_buffer.buffer[i].sql, db_buffer.buffer[i+1].sql);

            db_buffer.buffer[i].connection=db_buffer.buffer[i+1].connection;

            db_buffer.buffer[i].process_type=db_buffer.buffer[i+1].process_type;

            if(db_buffer.buffer[i+1].packet!=NULL){

                //memcpy is unreliable hence copy data bytes across individually
                int j=0;
                for(j=0; j<packet_length; j++){
                    db_buffer.buffer[i].packet[j]=db_buffer.buffer[i+1].packet[j];
                }
            }
        }

        //clear last entry in database buffer
        memset(&db_buffer.buffer[i+1], 0, sizeof(db_buffer.buffer[i+1])) ;

        db_buffer.buffer_count--;

    }
}
