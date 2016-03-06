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

#include <stdio.h> //support for snprintf
#include <stdlib.h> //support for NULL data type and atoi function
#include <string.h> //support for strcpy function

#include "database_functions.h"
#include "../logging.h"
#include "../character_race.h"
#include "../server_start_stop.h"
#include "../string_functions.h"

void load_db_char_races(){

    /** public function - see header */

     //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("RACE_TABLE", GET_CALL_INFO);

    log_event(EVENT_INITIALISATION, "loading races...");

    sqlite3_stmt *stmt;
    char sql[MAX_SQL_LEN]="SELECT * FROM RACE_TABLE";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //read the sql query result into the race array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int race_id=sqlite3_column_int(stmt, 0);

        if(race_id>MAX_RACES) {

            log_event(EVENT_ERROR, "race_id [%i] exceeds max [%i] in function %s: module %s: line %i", race_id, MAX_RACES, GET_CALL_INFO);
            stop_server();
        }

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(race[race_id].race_name, (char*)sqlite3_column_text(stmt, 1));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(race[race_id].race_description, (char*)sqlite3_column_text(stmt, 2));

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", race_id, race[race_id].race_name);

        i++;
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    if(i==0){

        log_event(EVENT_ERROR, "no races found in database", i);
        stop_server();
    }
}


void add_db_race(int race_id, char *race_name, char *race_description){

    /** RESULT  : loads an entry to the race table

        RETURNS : void

        PURPOSE : used in batch_add_races

        NOTES   :
    **/

    sqlite3_stmt *stmt=NULL;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("RACE_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="INSERT INTO RACE_TABLE("  \
        "RACE_ID," \
        "RACE_NAME," \
        "RACE_DESCRIPTION" \
        ") VALUES(?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, race_id);
    sqlite3_bind_text(stmt, 2, race_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, race_description, -1, SQLITE_STATIC);

    //process sql statement
    int rc=sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    //destroy query
    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "Race [%s] added successfully\n", race_name);

    log_event(EVENT_SESSION, "Added race [%i] [%s] to RACE_TABLE", race_id, race_name);
}


void batch_add_races(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "race list file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding races specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding races specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[3][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_race(atoi(output[0]), output[1], output[2]);
    }

    fclose(file);
}
