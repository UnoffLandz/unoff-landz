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

    log_event(EVENT_INITIALISATION, "loading skills...");

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SKILL_TABLE", GET_CALL_INFO);

    for(int skill_id=0; skill_id<MAX_SKILLS; skill_id++){

        char sql[MAX_SQL_LEN]="";
        sprintf(sql, "SELECT * FROM SKILL_TABLE WHERE SKILL_TYPE_ID=%i", skill_id);

        sqlite3_stmt *stmt;

        //prepare the sql statement
        int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

        if(rc!=SQLITE_OK){

            log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
        }

        //read the sql query result into the object array
        int i=0;

        while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

            //get the level and check that the value does not exceed the maximum permitted
            int level=sqlite3_column_int(stmt, 2);

            if(level>MAX_LEVELS){

                log_event(EVENT_ERROR, "level [%i] exceeds range [%i] in function %s: module %s: line %i", level, MAX_LEVELS, __func__, __FILE__, __LINE__);
                stop_server();
            }

            skill_level[skill_id].max_exp[level]=sqlite3_column_int(stmt, 3);

            log_event(EVENT_INITIALISATION, "loaded [%i] [Harvesting Skill Levels]", i);

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
    }
}


void add_db_skill_level(int skill_type_id, int level, int exp){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SKILL_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN, "INSERT INTO SKILL_TABLE("  \
        "SKILL_TYPE_ID," \
        "LEVEL," \
        "EXP " \
        ") VALUES(%i, %i, %i)", skill_type_id, level, exp);

    process_sql(sql);

    fprintf(stderr, "Skill_level [%i] of skill [%s] added successfully\n", level, skill_name[skill_type_id].skill);

    log_event(EVENT_SESSION, "Added skill level [%i] of skill [%s] to SKILL_TABLE", level, skill_name[skill_type_id].skill);
}


void batch_add_skills(int skill_type_id, char *file_name){

    /** public function - see header */

    FILE* file;

    char line[160]="";
    int line_counter=0;


    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "harvest skill list file [%s] not found", file_name);
        stop_server();
    }

    log_event(EVENT_INITIALISATION, "\nAdding harvest skill specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding harvest skill specified in file [%s]\n", file_name);

    line_counter=0;

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[8][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_skill_level(skill_type_id, atoi(output[0]), atoi(output[1]));
    }

    fclose(file);
}
