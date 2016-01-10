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

#include "idle_buffer2.h"
#include "logging.h"
#include "server_start_stop.h"
#include "log_in.h"
#include "character_creation.h"
#include "db/database_functions.h"
#include "server_start_stop.h"
#include "characters.h"
#include "guilds.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <stdexcept>

#define IDLE_BUFFER2_MAX 100
#define MAX_PROTOCOL_PACKET_SIZE2 160

//set debug mode
#define DEBUG_IDLE_BUFFER2 0
// add a helper macro that is performing printf when DEBUG_IDLE_BUFFER2 != 0
// and does nothing otherwise
// this is meant to simplify all "#if DEBUG_IDLE_BUFFER2  printf()  #endif " parts of code

#if DEBUG_IDLE_BUFFER2
#define D_PRINT(...) printf(__VA_ARGS__)
#else
#define D_PRINT(...)
#endif

struct data_{

    std::string sql;
    unsigned char packet[MAX_PROTOCOL_PACKET_SIZE2];
    size_t packet_len;
    int actor_node;
    int process_type;
    char char_name[MAX_CHAR_NAME_LEN];
    int guild_id;
};

// command storage typedef
typedef std::deque<data_> buffer_list_type;

buffer_list_type idle_buffer2;

void push_idle_buffer2(int actor_node, int process_type, const unsigned char *packet, size_t packet_len){

    /** public function - see header **/

    if(idle_buffer2.size()>=IDLE_BUFFER2_MAX) {

        //buffer overflow
        log_event(EVENT_ERROR, "database buffer overflow in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    data_ entry;

    entry.sql.clear();
    entry.actor_node=actor_node;
    entry.process_type=process_type;
    entry.packet_len=packet_len;

    //if packet length > 0 then copy packet data to buffer
    if(packet_len > 0){

        if(packet_len > MAX_PROTOCOL_PACKET_SIZE2-1){

            log_event(EVENT_ERROR, "maximum protocol packet size exceeded in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        memcpy(entry.packet, packet, packet_len);
    }

    idle_buffer2.push_back(entry);
}


void push_command(int actor_node, int process_type, char *char_name, int guild_id){

    /** public function - see header **/

    if(idle_buffer2.size()>=IDLE_BUFFER2_MAX) {

        //buffer overflow
        log_event(EVENT_ERROR, "database buffer overflow in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    data_ entry;

    entry.sql.clear();
    entry.actor_node=actor_node;
    entry.process_type=process_type;
    strcpy(entry.char_name, char_name);
    entry.guild_id=guild_id;

    idle_buffer2.push_back(entry);
}


void push_sql_command(const char *fmt, ...){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    va_list args;
    va_start(args, fmt);

    if (vsprintf(sql, fmt, args)>MAX_SQL_LEN){

        log_event(EVENT_ERROR, "sql string exceeds max [%i] in function %s: module %s: line %i", MAX_SQL_LEN, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(idle_buffer2.size()>=IDLE_BUFFER2_MAX) {

        //buffer overflow
        log_event(EVENT_ERROR, "database buffer overflow in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    data_ entry;

    entry.sql = sql;
    entry.process_type=IDLE_BUFFER_PROCESS_SQL;

    idle_buffer2.push_back(entry);

    va_end(args);
}


void process_idle_buffer2(){

    /** public function - see header **/

    //make sure we have something in the buffer to process
    if(idle_buffer2.empty())
        return;

    const data_ &command(idle_buffer2.front());

    int actor_node=command.actor_node;

    //use else if structure rather than switch, as this allows us to encapsulate
    //variables within each if statement
    /**********************************************************************************************/

    if(command.process_type==IDLE_BUFFER_PROCESS_CHECK_NEWCHAR){

        //Checks whether a character name exists in the character_table of the database. If the
        //name exists, character creation is aborted and a message sent to the client. If the
        //name does not exist, the character creation packet is placed in the idle buffer with
        //an instruction for IDLE_BUFFER_PROCESS_ADD_NEWCHAR so as the new character is added to
        //the database at the next idle event

        D_PRINT("IDLE_BUFFER2_PROCESS_CHECK_NEWCHAR\n");
        check_new_character(actor_node, command.packet);
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_ADD_NEWCHAR){

        D_PRINT("IDLE_BUFFER2_PROCESS_ADD_NEWCHAR\n");
        add_new_character(actor_node, command.packet);
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_LOGIN){

        D_PRINT("IDLE_BUFFER2_PROCESS_LOGIN\n");
        process_log_in(actor_node, command.packet);
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_SQL){

        D_PRINT("IDLE_BUFFER2_PROCESS_SQL\n");
        process_sql(command.sql.c_str());
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_HASH_DETAILS){

        D_PRINT("IDLE_BUFFER2_PROCESS_HASH_DETAILS\n");
        send_char_details(actor_node, command.char_name);
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_LIST_GUILD_BY_RANK){

        D_PRINT("IDLE_BUFFER2_PROCESS_LIST_GUILD_BY_RANK\n");
        list_guild_members(actor_node, GUILD_ORDER_RANK);

    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_LIST_GUILD_BY_TIME){

        D_PRINT("IDLE_BUFFER2_PROCESS_LIST_GUILD_BY_TIME\n");
        list_guild_members(actor_node, GUILD_ORDER_TIME);
    }
    /**********************************************************************************************/

    else if(idle_buffer2.front().process_type==IDLE_BUFFER_PROCESS_GUILD_DETAILS){

        D_PRINT("IDLE_BUFFER2_PROCESS_LIST_GUILD_BY_TIME\n");
        send_guild_details(actor_node, command.guild_id);
    }
    /**********************************************************************************************/

    else {

        log_event(EVENT_ERROR, "unknown process type in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        stop_server();
    }

    idle_buffer2.pop_front();
}
// remove file-wide macro to prevent clashes when doing 'unity' builds
#undef D_PRINT
