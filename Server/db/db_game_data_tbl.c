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

#include <stdio.h>  //support for NULL snprintf printf
#include <string.h> //support strcpy and strstr
#include <stdlib.h> //support atoi function

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../game_data.h"
#include "db_upgrade.h"
#include "../string_functions.h"

void load_db_game_data(){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GAME_DATA_TABLE", GET_CALL_INFO);

    log_event(EVENT_INITIALISATION, "loading game data...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM GAME_DATA_TABLE";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //read the sql query result into the game data array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the game data id and check that there is only one set
        int id=sqlite3_column_int(stmt,0);

        if(id!=1){

            log_event(EVENT_ERROR, "game data has incorrect id [%i] in function %s: module %s: line %i", id, GET_CALL_INFO);
            stop_server();
        }

        game_data.beam_map_id=sqlite3_column_int(stmt, 1);
        game_data.beam_map_tile=sqlite3_column_int(stmt, 2);
        game_data.start_map_id=sqlite3_column_int(stmt, 3);
        game_data.start_map_tile=sqlite3_column_int(stmt, 4);
        game_data.game_minutes=sqlite3_column_int(stmt, 5);
        game_data.game_days=sqlite3_column_int(stmt, 6);
        game_data.year_length=sqlite3_column_int(stmt, 7);
        game_data.database_version=sqlite3_column_int(stmt, 8);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 9)) strcpy(game_data.server_name, (char*)sqlite3_column_text(stmt, 9));

        i++;
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

   if(i!=1){

        log_event(EVENT_ERROR, "no game data found in database", i);
        stop_server();
    }
}


void add_db_game_data(int beam_map_id, int beam_map_tile, int start_map_id,
    int start_map_tile, int year_length, int db_version, char *server_name){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GAME_DATA_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="INSERT INTO GAME_DATA_TABLE("  \
        "GAME_DATA_ID,"   \
        "BEAM_MAP_ID,"    \
        "BEAM_MAP_TILE,"  \
        "START_MAP_ID,"   \
        "START_MAP_TILE," \
        "YEAR_LENGTH,"    \
        "DB_VERSION,"     \
        "SERVER_NAME"     \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?)";

    //prepare the sql statement
    prepare_query(sql, &stmt, GET_CALL_INFO);

    //bind the data to the sql statement
    sqlite3_bind_int(stmt, 1, (int)1);
    sqlite3_bind_int(stmt, 2, beam_map_id);
    sqlite3_bind_int(stmt, 3, beam_map_tile);
    sqlite3_bind_int(stmt, 4, start_map_id);
    sqlite3_bind_int(stmt, 5, start_map_tile);
    sqlite3_bind_int(stmt, 6, year_length);
    sqlite3_bind_int(stmt, 7, db_version);
    sqlite3_bind_text(stmt, 8, server_name, -1, SQLITE_STATIC);

    //process sql statement
    int rc=sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    //destroy sql statement
    destroy_query(sql, &stmt, GET_CALL_INFO);

    //log result
    fprintf(stderr, "Game Data added successfully\n");
    log_event(EVENT_SESSION, "Added game data to GAME_DATA_TABLE");
}


void batch_add_game_data(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "game data list file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding game data specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding game data specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[7][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_game_data(atoi(output[0]), atoi(output[1]), atoi(output[2]), atoi(output[3]),
            atoi(output[4]), REQUIRED_DATABASE_VERSION, output[6]);
    }


    fclose(file);
}
