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
#include "../gender.h"
#include "../server_start_stop.h"
#include "../objects.h"

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

        //get the object id and check that the value does not exceed the maximum permitted
        int id=sqlite3_column_int(stmt,0);

        if(id>MAX_OBJECTS){

            log_event(EVENT_ERROR, "object id [%i] exceeds range [%i] in function %s: module %s: line %i", id, MAX_OBJECTS, __func__, __FILE__, __LINE__);
            stop_server();
        }

        strcpy(object[id].object_name, (char*)sqlite3_column_text(stmt, 1));
        object[id].image_id=sqlite3_column_int(stmt, 2);
        if(sqlite3_column_int(stmt, 3)==1) object[id].harvestable=true; else object[id].harvestable=false;
        if(sqlite3_column_int(stmt, 4)==1) object[id].edible=true; else object[id].edible=false;

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", id, object[id].object_name);

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

void add_db_object(int id, char *object_name, int image_id, bool harvestable, bool edible ){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";

    int _harvest=0, _edible=0;

    if(harvestable==true)_harvest=1; else _harvest=0;
    if(edible==true)_edible=1; else _edible=0;

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO OBJECT_TABLE("  \
        "OBJECT_ID," \
        "OBJECT_NAME," \
        "IMAGE_ID," \
        "HARVESTABLE," \
        "EDIBLE" \
        ") VALUES(%i, '%s', %i, %i, %i)", id, object_name, image_id, _harvest, _edible);

    process_sql(sql);

    printf("Object [%s] added successfully\n", object_name);

    log_event(EVENT_SESSION, "Added object [%s] to OBJECT_TABLE", object_name);
}
