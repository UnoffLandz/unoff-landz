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

#include <stdlib.h> //support for NULL
#include <stdio.h> //support for printf
#include <string.h>

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../attributes.h"
#include "../server_start_stop.h"


void load_db_attributes(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading attributes...");

    //check database table exists
   if(table_exists("ATTRIBUTE_TABLE")==false){

        log_event(EVENT_ERROR, "table [ATTRIBUTE_TABLE] not found in database");
        stop_server();
    }

    int i=0;
    for(int attribute_type_id=1; attribute_type_id<=MAX_ATTRIBUTES; attribute_type_id++){

        for(int race_id=0; race_id<MAX_RACES; race_id++){

            sqlite3_stmt *stmt;

            char sql[MAX_SQL_LEN]="";
            snprintf(sql, MAX_SQL_LEN, "SELECT * FROM ATTRIBUTE_TABLE WHERE ATTRIBUTE_TYPE_ID=%i AND RACE_ID=%i", attribute_type_id, race_id);

            int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if(rc!=SQLITE_OK){

                log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
            }

            while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

                int pick_point=sqlite3_column_int(stmt, 3);

                switch(attribute_type_id){

                    case ATTR_DAY_VISION:
                    attribute[race_id].day_vision[pick_point]=sqlite3_column_int(stmt,4);
                    break;

                    case ATTR_NIGHT_VISION:
                    attribute[race_id].night_vision[pick_point]=sqlite3_column_int(stmt,4);
                    break;

                    case ATTR_CARRY_CAPACITY:
                    attribute[race_id].carry_capacity[pick_point]=sqlite3_column_int(stmt,4);
                    break;

                    default:
                    log_event(EVENT_ERROR, "unknown attribute type [%i]", attribute_type_id);
                    stop_server();
                    break;
                }
            }

            if (rc!= SQLITE_DONE) {

                log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
            }

            //destroy the prepared sql statement
            rc=sqlite3_finalize(stmt);
            if(rc!=SQLITE_OK){

             log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
            }
        }

        i++;
    }

    if(i==0){

        log_event(EVENT_ERROR, "no attributes found in database", i);
        stop_server();
    }
}

void add_db_attribute(int race_id, int attribute_type_id, int attribute_value[50]){

    /** public function - see header */

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char sql[MAX_SQL_LEN]="INSERT INTO ATTRIBUTE_TABLE("  \
        "RACE_ID," \
        "ATTRIBUTE_TYPE_ID,"  \
        "PICKPOINTS," \
        "ATTRIBUTE_VALUE" \
        ") VALUES(?, ?, ?, ?)";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    for(int pick_points=0; pick_points<MAX_PICKPOINTS; pick_points++){

        sqlite3_bind_int(stmt, 1, race_id);
        sqlite3_bind_int(stmt, 2, attribute_type_id);
        sqlite3_bind_int(stmt, 3, pick_points);
        sqlite3_bind_int(stmt, 4, attribute_value[pick_points]);

        rc = sqlite3_step(stmt);
        if (rc!= SQLITE_DONE) {

            log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Attribute [%s] added successfully\n", attribute_name[attribute_type_id]);

    log_event(EVENT_SESSION, "Added attribute [%s] to ATTRIBUTE_TABLE", attribute_name[attribute_type_id]);
 }
