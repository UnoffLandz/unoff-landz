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

#include <stdio.h> //support for snprintf
#include <stdlib.h> //support for NULL data type
#include <string.h> //support for strcpy function

#include "database_functions.h"
#include "../logging.h"
#include "../character_race.h"
#include "../server_start_stop.h"

int load_db_char_races(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM RACE_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading races...");

    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int race_id=sqlite3_column_int(stmt, 0);

        if(race_id>MAX_RACES) {

            log_event(EVENT_ERROR, "race_id [%i] exceeds max [%i] in function %s: module %s: line %i", race_id, MAX_RACES, __func__, __FILE__, __LINE__);
            stop_server();
        }

        strcpy(race[race_id].race_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(race[race_id].race_description, (char*)sqlite3_column_text(stmt, 2));

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", race_id, race[race_id].race_name);

        i++;
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return i;
}


void add_db_race(int race_id, char *race_name, char *race_description){

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO RACE_TABLE("  \
        "RACE_ID," \
        "RACE_NAME," \
        "RACE_DESCRIPTION" \
        ") VALUES(%i, '%s', '%s')", race_id, race_name, race_description);

    process_sql(sql);

    printf("Race [%s] added successfully\n", race_name);

    log_event(EVENT_SESSION, "Added race [%i] [%s] to RACE_TABLE", race_id, race_name);
}
