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

#include <stdio.h>  //support for NULL data type and snprintf function
#include <string.h> //support for strcpy
#include <stdlib.h> //support for atoi function

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../game_data.h"
#include "../season.h"
#include "../string_functions.h"

void load_db_seasons(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading season data...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SEASON_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="SELECT * FROM SEASON_TABLE";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //read the sql query result into the game data array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the game data id and check that there is only one set
        int id=sqlite3_column_int(stmt,0);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(season[id].season_name, (char*)sqlite3_column_text(stmt, 1));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(season[id].season_description, (char*)sqlite3_column_text(stmt, 2));

        season[id].start_day=sqlite3_column_int(stmt, 3);
        season[id].end_day=sqlite3_column_int(stmt, 4);

        i++;
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    if(i==0){

        log_event(EVENT_ERROR, "no seasons found in database", i);
        stop_server();
    }
}


void add_db_season(int season_id, char *season_name, char *season_description, int start_day, int end_day){

    /** RESULT  : adds season data to the season data table

        RETURNS : void

        PURPOSE : used in batch_add_seasons function

        NOTES   :
    **/

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("SEASON_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";
    ssnprintf(sql, MAX_SQL_LEN,
        "INSERT INTO SEASON_TABLE("  \
        "SEASON_ID,"   \
        "SEASON_NAME,"    \
        "SEASON_DESCRIPTION,"  \
        "START_DAY,"   \
        "END_DAY" \
        ") VALUES(%i, '%s', '%s', %i, %i)", season_id, season_name, season_description, start_day, end_day);

    process_sql(sql);

    fprintf(stderr, "Season [%s] added successfully\n", season_name);

    log_event(EVENT_SESSION, "Added season [%s] to SEASON_TABLE", season_name);
}


void batch_add_seasons(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "season list file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding seasons specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding seasons specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[5][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_season(atoi(output[0]), output[1], output[2], atoi(output[3]), atoi(output[4]));
    }

    fclose(file);
}
