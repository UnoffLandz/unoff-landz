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
#include "../string_functions.h"

void load_db_attributes(){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("ATTRIBUTE_TABLE", GET_CALL_INFO);

    int i=0;

    for(int attribute_type_id=1; attribute_type_id<=MAX_ATTRIBUTES; attribute_type_id++){

        for(int race_id=0; race_id<MAX_RACES; race_id++){

            sqlite3_stmt *stmt;

            char *sql="SELECT * FROM ATTRIBUTE_TABLE WHERE ATTRIBUTE_TYPE_ID=? AND RACE_ID=?";

            prepare_query(sql, &stmt, GET_CALL_INFO);

            sqlite3_bind_int(stmt, 1, attribute_type_id);
            sqlite3_bind_int(stmt, 2, race_id);

            int rc=0;

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

            destroy_query(sql, &stmt, GET_CALL_INFO);
        }

        i++;
    }

    if(i==0){

        log_event(EVENT_ERROR, "no attributes found in database", i);
        stop_server();
    }
}


void delete_db_attribute(int attribute_type_id){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("ATTRIBUTE_TABLE", GET_CALL_INFO);

    char *sql="DELETE FROM ATTRIBUTE_TABLE WHERE ATTRIBUTE_TYPE_ID=?";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, attribute_type_id);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    //log results
    fprintf(stderr, "Attribute [%s] removed successfully\n", attribute_name[attribute_type_id].attribute);
    log_event(EVENT_SESSION, "Attribute [%s] removed from ATTRIBUTE_TABLE", attribute_name[attribute_type_id].attribute);
}


void batch_add_attributes(char *file_name, int attribute_type_id){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding attributes specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding attributes specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("ATTRIBUTE_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO ATTRIBUTE_TABLE("  \
        "RACE_ID," \
        "ATTRIBUTE_TYPE_ID,"  \
        "PICKPOINTS," \
        "ATTRIBUTE_VALUE" \
        ") VALUES(?, ?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[3][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        sqlite3_bind_int(stmt, 1, atoi(output[0]));    //race id
        sqlite3_bind_int(stmt, 2, attribute_type_id);  //attribute type
        sqlite3_bind_int(stmt, 3, atoi(output[1]));    //pickpoints
        sqlite3_bind_int(stmt, 4, atoi(output[2]));    //attribute value

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "Attribute pickpoint [%i] of attribute [%s] added successfully\n", atoi(output[1]), attribute_name[attribute_type_id].attribute);
        log_event(EVENT_SESSION, "Added pickpoint [%i] of attribute [%s] to ATTRIBUTE_TABLE", atoi(output[1]), attribute_name[attribute_type_id].attribute);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);
}
