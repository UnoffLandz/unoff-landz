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

#include <stdio.h>  //support for NULL snprintf printf
#include <string.h> //support for strcpy

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../game_data.h"
#include "../season.h"
#include "../string_functions.h"

int load_db_seasons(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    //prepare the sql statement
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM SEASON_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading season data...");

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

    //return the number of query rows we were able to read
    return i;
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
