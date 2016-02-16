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

#include "database_functions.h"
#include "../logging.h"
#include "../chat.h"
#include "../server_start_stop.h"


void load_db_channels(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading channels...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHANNEL_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="SELECT * FROM CHANNEL_TABLE";

    //check database table exists
    char database_table[80];
    strcpy(database_table, strstr(sql, "FROM")+5);
    if(table_exists(database_table)==false){

        log_event(EVENT_ERROR, "table [%s] not found in database", database_table);
        stop_server();
    }

    //prepare sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int chan_id=sqlite3_column_int(stmt, 0);

        if(i>MAX_CHANNELS) {

            log_event(EVENT_ERROR, "chan_id [%i] exceeds range [0 - %i] in function %s: module %s: line %i", chan_id, MAX_CHANNELS, __func__, __FILE__, __LINE__);
            stop_server();
        }

        channel[chan_id].chan_type=sqlite3_column_int(stmt, 1);
        channel[chan_id].owner_id=sqlite3_column_int(stmt, 2);

        //test each string for null values otherwise the strcpy will segfault
        if(sqlite3_column_text(stmt, 3))
            strcpy(channel[chan_id].password, (char*)sqlite3_column_text(stmt, 3));

        if(sqlite3_column_text(stmt, 4))
            strcpy(channel[chan_id].channel_name, (char*)sqlite3_column_text(stmt, 4));

        if(sqlite3_column_text(stmt, 5))
            strcpy(channel[chan_id].description, (char*)sqlite3_column_text(stmt, 5));

        channel[chan_id].new_chars=sqlite3_column_int(stmt, 6);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", chan_id, channel[chan_id].channel_name);

        i++;
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy prepared sql statemnent
    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    if(i==0){

        log_event(EVENT_ERROR, "no chat channels found in database", i);
        stop_server();
    }
}


void add_db_channel(int channel_id, int owner_id, int channel_type, char *password, char *channel_name, char*channel_description, int new_chars){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHANNEL_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO CHANNEL_TABLE("  \
        "CHANNEL_ID," \
        "OWNER_ID," \
        "TYPE," \
        "PASSWORD," \
        "NAME,"  \
        "DESCRIPTION," \
        "NEW_CHARS " \
        ") VALUES(%i, %i, %i, '%s','%s', '%s', %i)", channel_id, owner_id, channel_type, password, channel_name, channel_description, new_chars);

    process_sql(sql);

    log_event(EVENT_INITIALISATION, "Added channel [%i] [%s] to CHANNEL_TABLE", channel_id, channel_name);
}
