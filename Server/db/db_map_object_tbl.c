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
#include <stdbool.h> // supports true/false
#include <stdlib.h> //support EXIT_FAILURE
#include <string.h> //support memcpy

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../map_object.h"
#include "../maps.h"
#include "../file_functions.h"
#include "../string_functions.h"
#include "../numeric_functions.h"
#include "../e3d.h"

void load_db_map_objects(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading map objects...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM MAP_OBJECT_TABLE";

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

    //read the sql query result into the map object array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id=sqlite3_column_int(stmt,0);
        int threedol_id=sqlite3_column_int(stmt, 1);
        int map_id=sqlite3_column_int(stmt, 2);

        map_object[threedol_id][map_id].tile=sqlite3_column_int(stmt, 3);
        map_object[threedol_id][map_id].e3d_id=sqlite3_column_int(stmt, 4);
        if(sqlite3_column_int(stmt, 5)==1) map_object[threedol_id][map_id].harvestable=true; else map_object[threedol_id][map_id].harvestable=false;
        map_object[threedol_id][map_id].reserve=sqlite3_column_int(stmt, 6);

        log_event(EVENT_INITIALISATION, "loaded [%i]", id);

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

        log_event(EVENT_ERROR, "no map objects found in database", i);
        stop_server();
    }
}
