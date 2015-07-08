/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

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
#include "../server_start_stop.h"
#include "../e3d.h"

int load_db_e3ds(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    //prepare the sql statement
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM E3D_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading e3d...");

    //read the sql query result into the e3d array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the object id and check that the value does not exceed the maximum permitted
        int id=sqlite3_column_int(stmt,0);

        if(id>MAX_E3D){

            log_event(EVENT_ERROR, "id [%i] exceeds range [%i] in function %s: module %s: line %i", id, MAX_E3D, __func__, __FILE__, __LINE__);
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

    //return the number of query rows we were able to read
    return i;
}

void add_db_e3d(int id, char *e3d_filename, int object_id){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO E3D_TABLE("  \
        "E3D_ID," \
        "E3D_FILE_NAME,"  \
        "OBJECT_ID" \
        ") VALUES(%i, '%s', %i)", id, e3d_filename, object_id);

    process_sql(sql);

    printf("e3d [%s] added successfully\n", e3d_filename);

    log_event(EVENT_SESSION, "Added e3d [%s] to E3D_TABLE", e3d_filename);
}
