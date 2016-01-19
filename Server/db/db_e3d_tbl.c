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

    char sql[MAX_SQL_LEN]="SELECT * FROM E3D_TABLE";

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

    //read the sql query result into the e3d array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the object id and check that the value does not exceed the maximum permitted
        int id=sqlite3_column_int(stmt,0);

        if(id>MAX_E3D_TYPES){

            log_event(EVENT_ERROR, "id [%i] exceeds range [%i] in function %s: module %s: line %i", id, MAX_E3D_TYPES, __func__, __FILE__, __LINE__);
            stop_server();
        }

        strcpy(e3d[id].e3d_filename, (char*)sqlite3_column_text(stmt, 1));
        e3d[id].object_id=sqlite3_column_int(stmt, 2);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", id, e3d[id].e3d_filename);

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

        log_event(EVENT_ERROR, "no e3ds found in database", i);
        stop_server();
    }
}


void add_db_e3d(int id, char *e3d_filename, int object_id){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO E3D_TABLE("  \
        "E3D_ID," \
        "E3D_FILENAME,"  \
        "OBJECT_ID" \
        ") VALUES(%i, '%s', %i)", id, e3d_filename, object_id);

    process_sql(sql);

    printf("e3d [%s] added successfully\n", e3d_filename);

    log_event(EVENT_SESSION, "Added e3d [%s] to E3D_TABLE", e3d_filename);
}


void batch_add_e3ds(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "e3d load file [%s] not found", file_name);
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

        add_db_e3d(atoi(output[0]), output[1], atoi(output[2]));
    }

    fclose(file);
}
