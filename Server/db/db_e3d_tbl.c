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
#include <stdio.h> //support snprintf
#include <string.h> //support strcpy
#include <stdlib.h> //support EXIT_FAILURE

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../e3d.h"
#include "../string_functions.h"


void load_db_e3ds(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading e3d...");

    sqlite3_stmt *stmt;

     //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("E3D_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM E3D_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the e3d array
    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the object id and check that the value does not exceed the maximum permitted
        int id=sqlite3_column_int(stmt,0);

        if(id>MAX_E3D_TYPES){

            log_event(EVENT_ERROR, "id [%i] exceeds range [%i] in function %s: module %s: line %i", id, MAX_E3D_TYPES, GET_CALL_INFO);
            stop_server();
        }

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(e3ds.e3d[id].e3d_filename, (char*)sqlite3_column_text(stmt, 1));

        e3ds.e3d[id].object_id=sqlite3_column_int(stmt, 2);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", id, e3ds.e3d[id].e3d_filename);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no e3ds found in database", i);
        stop_server();
    }
}


void batch_add_e3ds(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding e3ds specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding e3ds specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("E3D_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO E3D_TABLE("  \
    "E3D_ID," \
    "E3D_FILENAME,"  \
    "OBJECT_ID" \
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

        char output[5][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        sqlite3_bind_int(stmt, 1, atoi(output[0]));                 //e3d id
        sqlite3_bind_text(stmt, 2, output[1], -1, SQLITE_STATIC);   //e3d file name
        sqlite3_bind_int(stmt, 3, atoi(output[2]));                 //object id

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "e3d [%s] added successfully\n", output[1]);
        log_event(EVENT_SESSION, "Added e3d [%s] to E3D_TABLE", output[1]);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);

    //load data so it can be used by other functions
    load_db_e3ds();

    e3ds.data_loaded=true;
}
