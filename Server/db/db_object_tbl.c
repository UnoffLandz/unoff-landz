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
#include <stdio.h> //support snprintf
#include <string.h> //support strcpy

#include "database_functions.h"
#include "../logging.h"
#include "../gender.h"
#include "../server_start_stop.h"
#include "../map_objects.h"

int load_db_objects(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    //prepare the sql statement
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM OBJECT_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading object...");

    //read the sql query result into the map array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the gender id and check that the value does not exceed the maximum permitted
        int object_id=sqlite3_column_int(stmt,0);

        if(object_id>MAX_MAP_OBJECTS){

            log_event(EVENT_ERROR, "object id [%i] exceeds range [%i] in function %s: module %s: line %i", object_id, MAX_MAP_OBJECTS, __func__, __FILE__, __LINE__);
            stop_server();
        }

        strcpy(map_object[object_id].e3d_file_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(map_object[object_id].object_name, (char*)sqlite3_column_text(stmt, 2));
        map_object[object_id].image_id=sqlite3_column_int(stmt, 3);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", object_id, map_object[object_id].object_name);

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

void add_db_object(int object_id, char *e3d_file_name, char *object_name, int image_id, int harvestable, int edible ){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO OBJECT_TABLE("  \
        "OBJECT_ID," \
        "E3D_FILE_NAME,"  \
        "OBJECT_NAME," \
        "IMAGE_ID," \
        "HARVESTABLE," \
        "EDIBLE" \
        ") VALUES(%i, '%s', '%s', %i, %i, %i)", object_id, e3d_file_name, object_name, image_id, harvestable, edible);

    process_sql(sql);

    printf("Object [%s] added successfully\n", object_name);

    log_event(EVENT_SESSION, "Added object [%s] to OBJECT_TABLE", object_name);
}
