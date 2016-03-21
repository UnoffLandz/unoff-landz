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

#include <string.h> //support strcpy function
#include <stdlib.h> //support for NULL data type and atoi function
#include <stdio.h> //support snprintf function

#include "database_functions.h"
#include "../logging.h"
#include "../gender.h"
#include "../server_start_stop.h"
#include "../string_functions.h"

void load_db_genders(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading gender...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GENDER_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM GENDER_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the gender array
    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the gender id and check that the value does not exceed the maximum permitted
        int gender_id=sqlite3_column_int(stmt,0);

        if(gender_id>MAX_GENDERS){

            log_event(EVENT_ERROR, "gender id [%i] exceeds range [%i] in function %s: module %s: line %i", gender_id, MAX_GENDERS, GET_CALL_INFO);
            stop_server();
        }

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(genders.gender[gender_id].gender_name, (char*)sqlite3_column_text(stmt, 1));

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", gender_id, genders.gender[gender_id].gender_name);

        i++;

    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no genders found in database", i);
        stop_server();
    }
}


void batch_add_genders(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding genders specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding genders specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GENDER_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO GENDER_TABLE("  \
        "GENDER_ID," \
        "GENDER_NAME"  \
        ") VALUES(?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[2][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        sqlite3_bind_int(stmt, 1, atoi(output[0]));                 //gender id
        sqlite3_bind_text(stmt, 2, output[1], -1, SQLITE_STATIC);   //gender name

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "Gender [%i] [%s] added successfully\n", atoi(output[0]), output[1]);
        log_event(EVENT_SESSION, "Added gender [%i] [%s] to GENDER_TABLE", atoi(output[0]), output[1]);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);

    //load gender data to memory so this can be used by other functions
    load_db_genders();

    //mark data as loaded
    genders.data_loaded=true;
}
