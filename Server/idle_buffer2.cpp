#include <stdio.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <stdexcept>

#include "logging.h"
#include "db/database_functions.h"
#include "server_start_stop.h"
#include "log_in.h"
#include "character_creation.h"
#include "server_start_stop.h"
#include "idle_buffer2.h"

#define IDLE_BUFFER2_MAX 100
#define MAX_PROTOCOL_PACKET_SIZE2 160

#define DEBUG_IDLE_BUFFER2 0//set debug mode

struct data_{

    char sql[MAX_SQL_LEN];
    unsigned char packet[MAX_PROTOCOL_PACKET_SIZE2];
    int packet_len;
    int connection;
    int process_type;
};

std::queue<data_> idle_buffer2;

void push_idle_buffer2(const char *sql, int connection, int process_type, unsigned char *packet, int packet_len){

    /** public function - see header **/

    data_ data;

    if(idle_buffer2.size() < IDLE_BUFFER2_MAX){

        strcpy(data.sql, sql);
        data.connection=connection;
        data.process_type=process_type;
        data.packet_len=packet_len;

        //if packet length > 0 then copy packet data to buffer
        if(packet_len > 0){

            if(packet_len > MAX_PROTOCOL_PACKET_SIZE2-1){

                log_event(EVENT_ERROR, "maximum protocol packet size exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
                stop_server();
            }

            memcpy(data.packet, packet, packet_len);
/*
            for(int i=0; i<packet_len; i++){

                data.packet[i]=packet[i];
            }
*/
        }

        idle_buffer2.push(data);
    }
    else {

        //buffer overflow
        log_event(EVENT_ERROR, "database buffer overflow in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }
}

void process_idle_buffer2(){

    /** public function - see header **/

    //make sure we have something in the buffer to process
    if(idle_buffer2.size() > 0){

        int connection=idle_buffer2.front().connection;

        //use else if structure rather than switch, as this allows us to encapsulate
        //variables within each if statement
 /**********************************************************************************************/

        if(idle_buffer2.front().process_type==IDLE_BUFFER2_PROCESS_CHECK_NEWCHAR){

            //Checks whether a character name exists in the character_table of the database. If the
            //name exists, character creation is aborted and a message sent to the client. If the
            //name does not exist, the character creation packet is placed in the idle buffer with
            //an instruction for IDLE_BUFFER_PROCESS_ADD_NEWCHAR so as the new character is added to
            //the database at the next idle event

            #if DEBUG_IDLE_BUFFER2 == 1
            printf("IDLE_BUFFER2_PROCESS_CHECK_NEWCHAR\n");
            #endif

            check_new_character(connection, idle_buffer2.front().packet);
        }
/**********************************************************************************************/

        else if(idle_buffer2.front().process_type==IDLE_BUFFER2_PROCESS_ADD_NEWCHAR){

            #if DEBUG_IDLE_BUFFER2 == 1
            printf("IDLE_BUFFER2_PROCESS_ADD_NEWCHAR\n");
            #endif

            add_new_character(connection, idle_buffer2.front().packet);
        }
/**********************************************************************************************/

        else if(idle_buffer2.front().process_type==IDLE_BUFFER2_PROCESS_LOGIN){

            #if DEBUG_IDLE_BUFFER2 == 1
            printf("IDLE_BUFFER2_PROCESS_LOGIN\n");
            #endif

            process_log_in(connection, idle_buffer2.front().packet);
          }
/**********************************************************************************************/

        else if(idle_buffer2.front().process_type==IDLE_BUFFER2_PROCESS_SQL){

            #if DEBUG_IDLE_BUFFER2 == 1
            printf("IDLE_BUFFER2_PROCESS_SQL\n");
            #endif

            process_sql(idle_buffer2.front().sql);
        }
/**********************************************************************************************/

        else {

            log_event(EVENT_ERROR, "unknown process type in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        idle_buffer2.pop();
    }
}
