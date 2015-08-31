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

#include <stdio.h>  //support for NULL snprintf printf
#include <string.h> //support for strcpy

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../game_data.h"
#include "../season.h"
#include "../string_functions.h"

void load_db_seasons(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading season data...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM SEASON_TABLE";

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

        strcpy(season[id].season_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(season[id].season_description, (char*)sqlite3_column_text(stmt, 2));
        season[id].start_day=sqlite3_column_int(stmt, 3);
        season[id].end_day=sqlite3_column_int(stmt, 4);

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

    if(i==0){

        log_event(EVENT_ERROR, "no seasons found in database", i);
        stop_server();
    }
}


void add_db_season(int season_id, char *season_name, char *season_description, int start_day, int end_day){

   /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    ssnprintf(sql, MAX_SQL_LEN,
        "INSERT INTO SEASON_TABLE("  \
        "SEASON_ID,"   \
        "SEASON_NAME,"    \
        "SEASON_DESCRIPTION,"  \
        "START_DAY,"   \
        "END_DAY" \
        ") VALUES(%i, '%s', '%s', %i, %i)", season_id, season_name, season_description, start_day, end_day);

    process_sql(sql);

    printf("Season [%s] added successfully\n", season_name);

    log_event(EVENT_SESSION, "Added season [%s] to SEASON_TABLE", season_name);
}
