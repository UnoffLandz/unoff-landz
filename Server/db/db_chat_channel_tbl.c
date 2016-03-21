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

#include <string.h> //support for strcpy
#include <stdio.h> //support for snprintf
#include <stdlib.h> //support for EXIT_FAILURE macro and exit functions

#include "database_functions.h"
#include "../logging.h"
#include "../chat.h"
#include "../server_start_stop.h"
#include "../string_functions.h"


void load_db_channels(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading channels...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHANNEL_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM CHANNEL_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int chan_id=sqlite3_column_int(stmt, 0);

        if(i>MAX_CHANNELS) {

            log_event(EVENT_ERROR, "chan_id [%i] exceeds range [0 - %i] in function %s: module %s: line %i", chan_id, MAX_CHANNELS, GET_CALL_INFO);
            stop_server();
        }

        channels.channel[chan_id].chan_type=sqlite3_column_int(stmt, 1);
        channels.channel[chan_id].owner_id=sqlite3_column_int(stmt, 2);

        //test each string for null values otherwise the strcpy will segfault
        if(sqlite3_column_text(stmt, 3))
            strcpy(channels.channel[chan_id].password, (char*)sqlite3_column_text(stmt, 3));

        if(sqlite3_column_text(stmt, 4))
            strcpy(channels.channel[chan_id].channel_name, (char*)sqlite3_column_text(stmt, 4));

        if(sqlite3_column_text(stmt, 5))
            strcpy(channels.channel[chan_id].description, (char*)sqlite3_column_text(stmt, 5));

        if(sqlite3_column_int(stmt, 6)==1) {

            channels.channel[chan_id].new_chars=true;
        }
        else if(sqlite3_column_int(stmt, 6)==0){

            channels.channel[chan_id].new_chars=false;
        }
        else {

            log_event(EVENT_ERROR, "illegal value for newchars in entry [%i] of channel table", sqlite3_column_int(stmt, 1), GET_CALL_INFO);
            stop_server();
        }

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", chan_id, channels.channel[chan_id].channel_name);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);
}


void add_db_channel(int channel_id, int owner_id, int channel_type, char *password, char *channel_name, char*channel_description, int new_chars){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHANNEL_TABLE", GET_CALL_INFO);

    char *sql="INSERT INTO CHANNEL_TABLE("  \
        "CHANNEL_ID," \
        "OWNER_ID," \
        "TYPE," \
        "PASSWORD," \
        "NAME,"  \
        "DESCRIPTION," \
        "NEW_CHARS " \
        ") VALUES(?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, channel_id);
    sqlite3_bind_int(stmt, 2, owner_id);
    sqlite3_bind_int(stmt, 3, channel_type);
    sqlite3_bind_text(stmt, 4, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, channel_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, channel_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, new_chars);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "Channel [%i] [%s] added successfully\n", channel_id, channel_name);

    log_event(EVENT_INITIALISATION, "Added channel [%i] [%s] to CHANNEL_TABLE", channel_id, channel_name);
}


void batch_add_channels(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "channels load file [%s] not found", file_name);
        exit(EXIT_FAILURE);
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding channels specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding channels specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[7][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_channel(atoi(output[0]), atoi(output[1]), atoi(output[2]), output[3], output[4], output[5], atoi(output[6]));
     }

    fclose(file);

    //load channel data to memory so this can be used by batch_add_character
    load_db_channels();

    //mark data as loaded
    channels.data_loaded=true;


}
