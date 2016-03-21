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
#include <stdlib.h> //testing only

#include "database_functions.h"
#include "../logging.h"
#include "../gender.h"
#include "../server_start_stop.h"
#include "../objects.h"
#include "../string_functions.h"


void load_db_objects(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading object...");

    sqlite3_stmt *stmt=NULL;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("OBJECT_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM OBJECT_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the object array
    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the object id and check that the value does not exceed the maximum permitted
        int object_id=sqlite3_column_int(stmt, 0);

        if(object_id>MAX_OBJECTS){

            log_event(EVENT_ERROR, "object id [%i] exceeds range [%i] in function %s: module %s: line %i", object_id, MAX_OBJECTS, GET_CALL_INFO);
            stop_server();
        }
        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(objects.object[object_id].object_name, (char*)sqlite3_column_text(stmt, 1));

        if(sqlite3_column_int(stmt, 2)==1) objects.object[object_id].harvestable=true; else objects.object[object_id].harvestable=false;
        if(sqlite3_column_int(stmt, 3)==1) objects.object[object_id].edible=true; else objects.object[object_id].edible=false;

        objects.object[object_id].harvest_interval=sqlite3_column_int(stmt, 4);
        objects.object[object_id].emu=sqlite3_column_int(stmt, 5);
        objects.object[object_id].equipable_item_type=sqlite3_column_int(stmt, 6);
        objects.object[object_id].equipable_item_id=sqlite3_column_int(stmt, 7);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", i, objects.object[object_id].object_name);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no objects found in database", i);
        stop_server();
    }
}


void batch_add_objects(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding objects specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding objects specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("OBJECT_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO OBJECT_TABLE("  \
        "OBJECT_ID," \
        "OBJECT_NAME," \
        "HARVESTABLE," \
        "EDIBLE," \
        "HARVEST_INTERVAL," \
        "EMU," \
        "EQUIPABLE_ITEM_TYPE, " \
        "EQUIPABLE_ITEM_ID" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[8][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        sqlite3_bind_int(stmt, 1, atoi(output[0]));
        sqlite3_bind_text(stmt, 2, output[1], -1, SQLITE_STATIC); //object name
        sqlite3_bind_int(stmt, 3, atoi(output[2]));     //harvest
        sqlite3_bind_int(stmt, 4, atoi(output[3]));     //edible
        sqlite3_bind_int(stmt, 5, atoi(output[4]));     //interval
        sqlite3_bind_double(stmt, 6, atoi(output[5]));  //emu
        sqlite3_bind_int(stmt, 7, atoi(output[6]));     //equipable_item_type
        sqlite3_bind_int(stmt, 8, atoi(output[7]));     //equipable_item_id

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "object [%i] [%s] added successfully\n", atoi(output[0]), output[1]);
        log_event(EVENT_SESSION, "Added object [%i] [%s] to RACE_TABLE", atoi(output[0]), output[1]);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);

    //load data to memory so this can be used by other functions
    load_db_objects();

    //mark data as loaded
    objects.data_loaded=true;
}
