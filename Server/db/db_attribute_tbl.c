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

#include <stdlib.h> //support for NULL
#include <stdio.h> //support for printf
#include <string.h>

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../attributes.h"
#include "../server_start_stop.h"


int load_db_attribute_values(int attribute_id, int race_id, int attribute_type_id){

    /** RESULT   : loads attribute values

        RETURNS  : void

        PURPOSE  : code modularisation

        NOTES    :
    **/

    int rc;
    sqlite3_stmt *stmt;
    int i=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM ATTRIBUTE_VALUE_TABLE WHERE ATTRIBUTE_ID=%i", attribute_id);

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int pickpoints=sqlite3_column_int(stmt, 2);
        int attribute_value=sqlite3_column_int(stmt, 3);

        switch(attribute_type_id){

            case ATTR_CARRY_CAPACITY:
            attribute[race_id].carry_capacity[pickpoints]=attribute_value;
            break;

            case ATTR_DAY_VISION:
            attribute[race_id].day_vision[pickpoints]=attribute_value;
            break;

            case ATTR_NIGHT_VISION:
            attribute[race_id].night_vision[pickpoints]=attribute_value;
            break;

            default:
            log_event(EVENT_ERROR, "unknown attribute_type_id [%i] in function %s: Module %s: line %i", attribute_type_id, __func__, __FILE__, __LINE__, rc, sql);
            stop_server();
            break;
        }

        i++;
    }

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return i;
}


int load_db_attributes(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    char attribute_description[80]="";
    int i=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT ATTRIBUTE_ID,ATTRIBUTE_DESCRIPTION,RACE_ID,ATTRIBUTE_TYPE_ID FROM ATTRIBUTE_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int attribute_id=sqlite3_column_int(stmt, 0);
        strcpy(attribute_description, (char*)sqlite3_column_text(stmt, 1));
        int race_id=sqlite3_column_int(stmt,2);
        int attribute_type_id=sqlite3_column_int(stmt,3);

        //check attribute bounds
        if(attribute_type_id<1 || attribute_type_id>3){

            log_event(EVENT_ERROR, "attempt to load unknown attribute type [%i] in function %s: module %s: line %i", attribute_type_id, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //check race_id bounds
        if(race_id<0 || race_id>MAX_RACES){

            log_event(EVENT_ERROR, "attempt to load attribute for unknown race type [%i] in function %s: module %s: line %i", race_id, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //load attribute values
        int loaded=load_db_attribute_values(attribute_id, race_id, attribute_type_id);
        if(loaded==0){

            log_event(EVENT_ERROR, "no attribute values found attribute [%i] in function %s: module %s: line %i", attribute_id, __func__, __FILE__, __LINE__);
            stop_server();
        }

        log_event(EVENT_INITIALISATION, "loaded attribute [%i] [%s] with [%i] values", attribute_id, attribute_description, loaded);

        i++;
    }

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return i;
}


void add_db_attribute(int attribute_id, char *attribute_description, int race_id, int attribute_type_id){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO ATTRIBUTE_TABLE("  \
        "ATTRIBUTE_ID," \
        "ATTRIBUTE_DESCRIPTION,"  \
        "RACE_ID," \
        "ATTRIBUTE_TYPE_ID" \
        ") VALUES( %i, '%s', %i, %i)", attribute_id, attribute_description, race_id, attribute_type_id);

    process_sql(sql);

    printf("Attribute [%s] added successfully\n", attribute_description);

    log_event(EVENT_SESSION, "Added attribute [%s] to ATTRIBUTE_TABLE", attribute_description);
}


void add_db_attribute_value (int attribute_value_id, int attribute_id, int attribute_type_id, int pickpoints, int attribute_value){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO ATTRIBUTE_VALUE_TABLE("  \
        "ATTRIBUTE_VALUE_ID," \
        "ATTRIBUTE_ID," \
        "ATTRIBUTE_TYPE_ID,"  \
        "PICKPOINTS," \
        "ATTRIBUTE_VALUE" \
        ") VALUES(%i, %i, %i, %i, %i);", attribute_value_id, attribute_id, attribute_type_id, pickpoints, attribute_value);

    process_sql(sql);

    printf("Attribute value [%i] for pickpoints [%i] added successfully\n", attribute_value, pickpoints);

    log_event(EVENT_SESSION, "Added attribute value [%i] for pickpoints [%i] to ATTRIBUTE_VALUE_TABLE", attribute_value, pickpoints);
}
