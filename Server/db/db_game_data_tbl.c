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

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../game_data.h"

void load_db_game_data(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading game data...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM GAME_DATA_TABLE";

    //check database table exists
    char database_table[80];
    strcpy(database_table, strstr(sql, "FROM")+5);
    if(table_exists(database_table)==false){

        log_event(EVENT_ERROR, "table [%s] not found in database", database_table);
        stop_server();
    }

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //read the sql query result into the game data array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the game data id and check that there is only one set
        int id=sqlite3_column_int(stmt,0);

        if(id!=1){

            log_event(EVENT_ERROR, "game data has incorrect id [%i] in function %s: module %s: line %i", id, __func__, __FILE__, __LINE__);
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

        i++;
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

   if(i!=1){

        log_event(EVENT_ERROR, "no game data found in database", i);
        stop_server();
    }
}


void add_db_game_data(int beam_map_id, int beam_map_tile, int start_map_id, int start_map_tile, int year_length, int db_version){

   /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO GAME_DATA_TABLE("  \
        "GAME_DATA_ID,"   \
        "BEAM_MAP_ID,"    \
        "BEAM_MAP_TILE,"  \
        "START_MAP_ID,"   \
        "START_MAP_TILE," \
        "YEAR_LENGTH,"    \
        "DB_VERSION"      \
        ") VALUES(%i, %i, %i, %i, %i, %i, %i)", 1, beam_map_id, beam_map_tile, start_map_id, start_map_tile, year_length, db_version);

    process_sql(sql);

    printf("Game Data added successfully\n");

    log_event(EVENT_SESSION, "Added game data to GAME_DATA_TABLE");
}
