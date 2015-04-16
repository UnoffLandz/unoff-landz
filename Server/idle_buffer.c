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
#include "db/database_functions.h"
#include "server_start_stop.h"
#include "log_in.h"
#include "character_creation.h"

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

        int connection=idle_buffer.buffer[1].connection;
        int packet_length=idle_buffer.buffer[1].packet[1] + (idle_buffer.buffer[1].packet[2] * 256) + 2;

        unsigned char packet[1024]={0};

        int i=0;
        for(i=0; i<packet_length; i++){
            packet[i]=idle_buffer.buffer[1].packet[i];
        }

        //use else if structure rather than switch, as this allows us to encapsulate
        //variables within each if statement
 /**********************************************************************************************/

        if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_CHECK_NEWCHAR){

            //Checks whether a character name exists in the character_table of the database. If the
            //name exists, character creation is aborted and a message sent to the client. If the
            //name does not exist, the character creation packet is placed in the idle buffer with
            //an instruction for IDLE_BUFFER_PROCESS_ADD_NEWCHAR so as the new character is added to
            //the database at the next idle event
            check_new_character(connection, packet);
        }
/**********************************************************************************************/

        else if(idle_buffer.buffer[1].process_type==IDLE_BUFFER_PROCESS_ADD_NEWCHAR){

            add_new_character(connection, packet);
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
