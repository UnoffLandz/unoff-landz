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

#include <stdio.h> //support for snprintf
#include <stdlib.h> //support for NULL data type and atoi function
#include <string.h> //support for memset function

#include "database_functions.h"
#include "../logging.h"
#include "../character_type.h"
#include "../server_start_stop.h"
#include "../character_race.h"
#include "../gender.h"
#include "../string_functions.h"
#include "db_character_race_tbl.h"
#include "db_gender_tbl.h"


void load_db_char_types(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading character types...");

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TYPE_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;

    char *sql="SELECT * FROM CHARACTER_TYPE_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the char type array
    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int char_type_id=sqlite3_column_int(stmt, 0);

        if(char_type_id>MAX_CHARACTER_TYPES) {

            log_event(EVENT_ERROR, "character_type_id [%i] exceeds max [%i] in function %s: module %s: line %i", char_type_id, MAX_CHARACTER_TYPES, GET_CALL_INFO);
            stop_server();
        }

        character_type[char_type_id].race_id=sqlite3_column_int(stmt, 1);
        character_type[char_type_id].gender_id=sqlite3_column_int(stmt, 2);

        log_event(EVENT_INITIALISATION, "loaded [%i] %s %s", char_type_id, race[character_type[char_type_id].race_id].race_name, gender[character_type[char_type_id].gender_id].gender_name);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no character types found in database", i);
        stop_server();
    }
}


void add_db_char_type(int char_type_id, int race_id, int gender_id){

    /** RESULT  : loads an entry to the character type table

        RETURNS : void

        PURPOSE : used in batch_add_char_types

        NOTES   :
    **/

    sqlite3_stmt *stmt=NULL;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TYPE_TABLE", GET_CALL_INFO);

    //prepare sql query
    char *sql="INSERT INTO CHARACTER_TYPE_TABLE("  \
        "CHARACTER_TYPE_ID," \
        "RACE_ID," \
        "GENDER_ID" \
        ") VALUES(?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, char_type_id);
    sqlite3_bind_int(stmt, 2, race_id);
    sqlite3_bind_int(stmt, 3, gender_id);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "Character type [%i] gender [%s] race [%s] added successfully\n", char_type_id, gender[gender_id].gender_name, race[race_id].race_name);

    log_event(EVENT_SESSION, "Added character type [%i] gender [%s] race [%s] to CHARACTER_TYPE_TABLE", char_type_id, gender[gender_id].gender_name, race[race_id].race_name);
}


void batch_add_char_types(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "char type list file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding char types specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding char types specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[4][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_char_type(atoi(output[0]), atoi(output[1]), atoi(output[2]));
    }

    fclose(file);
}
