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
    char *sql="SELECT * FROM RACE_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the race array
    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int race_id=sqlite3_column_int(stmt, 0);

        if(race_id>MAX_RACES) {

            log_event(EVENT_ERROR, "race_id [%i] exceeds max [%i] in function %s: module %s: line %i", race_id, MAX_RACES, GET_CALL_INFO);
            stop_server();
        }

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(races.race[race_id].race_name, (char*)sqlite3_column_text(stmt, 1));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(races.race[race_id].race_description, (char*)sqlite3_column_text(stmt, 2));

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", race_id, races.race[race_id].race_name);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no races found in database", i);
        stop_server();
    }
}


void add_db_race(sqlite3_stmt **stmt, int race_id, char *race_name, char *race_description){

    /** RESULT   : binds race data for loading to the database

        RETURNS  : void

        PURPOSE  :

        NOTES    : used by batch_add_attributes
    **/

    sqlite3_bind_int(*stmt, 1, race_id);
    sqlite3_bind_text(*stmt, 2, race_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(*stmt, 3, race_description, -1, SQLITE_STATIC);
}


void batch_add_races(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding races specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding races specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("RACE_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO RACE_TABLE("  \
        "RACE_ID," \
        "RACE_NAME," \
        "RACE_DESCRIPTION" \
        ") VALUES(?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[3][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        sqlite3_bind_int(stmt, 1, atoi(output[0]));                //race id
        sqlite3_bind_text(stmt, 2, output[1], -1, SQLITE_STATIC);  //race name
        sqlite3_bind_text(stmt, 3, output[2], -1, SQLITE_STATIC);  //race description

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "Race [%i] [%s] added successfully\n", atoi(output[0]), output[1]);
        log_event(EVENT_SESSION, "Added race [%i] [%s] to RACE_TABLE", atoi(output[0]), output[1]);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);

    //load race data to memory so this can be used by batch_add_character_types
    load_db_char_races();

    //mark data as loaded
    races.data_loaded=true;
}
