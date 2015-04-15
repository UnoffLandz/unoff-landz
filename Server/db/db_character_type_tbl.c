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

#include "database_functions.h"
#include "../logging.h"
#include "../character_type.h"
#include "../server_start_stop.h"
#include "../character_race.h"
#include "../gender.h"

int load_db_char_types(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TYPE_TABLE");
    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading character types...");

    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int char_type_id=sqlite3_column_int(stmt, 0);

        if(char_type_id>MAX_CHARACTER_TYPES) {

            log_event(EVENT_ERROR, "character_type_id [%i] exceeds max [%i] in function %s: module %s: line %i", char_type_id, MAX_CHARACTER_TYPES, __func__, __FILE__, __LINE__);
            stop_server();
        }

        character_type[char_type_id].race_id=sqlite3_column_int(stmt, 1);
        character_type[char_type_id].gender_id=sqlite3_column_int(stmt, 2);

        log_event(EVENT_INITIALISATION, "loaded [%i] %s %s", char_type_id, race[character_type[char_type_id].race_id].race_name, gender[character_type[char_type_id].gender_id].gender_name);

        i++;
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return i;
}


void add_db_char_type(int char_type_id, int race_id, int gender_id){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO CHARACTER_TYPE_TABLE("  \
        "CHARACTER_TYPE_ID," \
        "RACE_ID," \
        "SEX_ID" \
        ") VALUES(%i, %i, %i)", char_type_id, race_id, gender_id);

    process_sql(sql);

    printf("Character type [%i] added successfully\n", char_type_id);

    log_event(EVENT_SESSION, "Added character type [%i] to CHARACTER_TYPE_TABLE", char_type_id);
}
