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

#include <stdio.h> //support sprintf function
#include <stdlib.h> //supports atoi function
#include <string.h> //support strcpy function

#include "../logging.h"
#include "database_functions.h"
#include "../server_start_stop.h"
#include "../character_skill.h"
#include "../string_functions.h"


void load_db_skills(){

    /** public function - see header */

    sqlite3_stmt *stmt;

    log_event(EVENT_INITIALISATION, "loading skills...");

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SKILL_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM SKILL_TABLE WHERE SKILL_TYPE_ID=?";

    //prepare query
    prepare_query(sql, &stmt, GET_CALL_INFO);

    for(int skill_type_id=0; skill_type_id<MAX_SKILLS; skill_type_id++){

        sqlite3_bind_int(stmt, 1, skill_type_id);

        //read the sql query result into the skill array
        int i=0;
        int rc=0;

        while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

            //get the level and check that the value does not exceed the maximum permitted
            int level=sqlite3_column_int(stmt, 2);

            //make sure maximum levels doesn't exceed limit for system
            if(level>MAX_LEVELS){

                log_event(EVENT_ERROR, "level [%i] exceeds range [%i] in function %s: module %s: line %i", level, MAX_LEVELS, GET_CALL_INFO);
                stop_server();
            }

            skill_level[skill_type_id].max_exp[level]=sqlite3_column_int(stmt, 3);

            log_event(EVENT_INITIALISATION, "loaded [%s] level [%i] exp [%i]", skill_name[skill_type_id].skill, level, skill_level[level].max_exp);

            i++;
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);
}


void add_db_skill_level(sqlite3_stmt **stmt, int skill_type_id, int level, int exp){

    /** public function - see header */

    sqlite3_bind_int(*stmt, 1, skill_type_id);
    sqlite3_bind_int(*stmt, 2, level);
    sqlite3_bind_int(*stmt, 3, exp);
}


void delete_db_skill(int skill_type_id){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SKILL_TABLE", GET_CALL_INFO);

    char *sql="DELETE FROM SKILL_TABLE WHERE SKILL_TYPE_ID=?";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, skill_type_id);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    //log results
    fprintf(stderr, "Skill [%s] removed successfully\n", skill_name[skill_type_id].skill);
    log_event(EVENT_SESSION, "Skill [%s] removed from SKILL_TABLE", skill_name[skill_type_id].skill);
}


void batch_add_skills(char *file_name, int skill_type_id){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding skills specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding skills specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SKILL_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO SKILL_TABLE("  \
        "SKILL_TYPE_ID," \
        "LEVEL," \
        "EXP " \
        ") VALUES(?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[2][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_skill_level(&stmt, skill_type_id, atoi(output[0]), atoi(output[1]));

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "Skill_level [%i] of skill [%s] added successfully\n", atoi(output[0]), skill_name[skill_type_id].skill);
        log_event(EVENT_SESSION, "Added skill level [%i] of skill [%s] to SKILL_TABLE", atoi(output[0]), skill_name[skill_type_id].skill);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);
}
