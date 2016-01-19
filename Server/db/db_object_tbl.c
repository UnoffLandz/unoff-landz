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

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM OBJECT_TABLE";

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

    //read the sql query result into the object array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the object id and check that the value does not exceed the maximum permitted
        int object_id=sqlite3_column_int(stmt, 0);

        if(object_id>MAX_OBJECT_ID){

            log_event(EVENT_ERROR, "object id [%i] exceeds range [%i] in function %s: module %s: line %i", object_id, MAX_OBJECT_ID, __func__, __FILE__, __LINE__);
            stop_server();
        }

        strcpy(object[object_id].object_name, (char*)sqlite3_column_text(stmt, 1));
        if(sqlite3_column_int(stmt, 2)==1) object[object_id].harvestable=true; else object[object_id].harvestable=false;
        if(sqlite3_column_int(stmt, 3)==1) object[object_id].edible=true; else object[object_id].edible=false;
        object[object_id].harvest_interval=sqlite3_column_int(stmt, 4);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", i, object[object_id].object_name);

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

        log_event(EVENT_ERROR, "no objects found in database", i);
        stop_server();
    }
}


void add_db_object(int object_id, char *object_name, bool harvestable, bool edible, int interval){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    int _harvest=0, _edible=0;

    if(harvestable==true)_harvest=1; else _harvest=0;
    if(edible==true)_edible=1; else _edible=0;

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO OBJECT_TABLE("  \
        "OBJECT_ID," \
        "OBJECT_NAME," \
        "HARVESTABLE," \
        "EDIBLE," \
        "HARVEST_INTERVAL"
        ") VALUES(%i, '%s', %i, %i, %i)", object_id, object_name, _harvest, _edible, interval);

    process_sql(sql);

    printf("Object [%s] added successfully\n", object_name);

    log_event(EVENT_SESSION, "Added object [%s] to OBJECT_TABLE", object_name);
}


void batch_add_objects(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "object load file [%s] not found", file_name);
        exit(EXIT_FAILURE);
    }

    char line[160]="";
    int line_counter=0;

    printf("\n");

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[5][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_object(atoi(output[0]), output[1], atoi(output[2]), atoi(output[3]), atoi(output[4]));
    }

    fclose(file);
}

