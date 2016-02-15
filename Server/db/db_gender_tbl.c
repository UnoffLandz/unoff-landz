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

#include <string.h> //support strcpy function
#include <stdlib.h> //support for NULL data type and atoi function
#include <stdio.h> //support snprintf function

#include "database_functions.h"
#include "../logging.h"
#include "../gender.h"
#include "../server_start_stop.h"
#include "../string_functions.h"

void load_db_genders(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading gender...");

    sqlite3_stmt *stmt;

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    char sql[MAX_SQL_LEN]="SELECT * FROM GENDER_TABLE";

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

    //read the sql query result into the gender array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the gender id and check that the value does not exceed the maximum permitted
        int gender_id=sqlite3_column_int(stmt,0);

        if(gender_id>MAX_GENDER){

            log_event(EVENT_ERROR, "gender id [%i] exceeds range [%i] in function %s: module %s: line %i", gender_id, MAX_GENDER, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //get gender name
        strcpy(gender[gender_id].gender_name, (char*)sqlite3_column_text(stmt, 1));

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", gender_id, gender[gender_id].gender_name);

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

        log_event(EVENT_ERROR, "no genders found in database", i);
        stop_server();
    }
}


void add_db_gender(int gender_id, char *gender_name){

    /** public function - see header */

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO GENDER_TABLE("  \
        "GENDER_ID," \
        "GENDER_NAME"  \
        ") VALUES(%i, '%s')", gender_id, gender_name);

    process_sql(sql);

    fprintf(stderr, "Gender [%s] added successfully\n", gender_name);

    log_event(EVENT_SESSION, "Added gender [%s] to GENDER_TABLE", gender_name);
}


void batch_add_gender(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "gender list file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding genders specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding genders specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[2][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_gender(atoi(output[0]), output[1]);
    }

    fclose(file);
}
