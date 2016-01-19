/*****************************************************************************************************************
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

#include <string.h> //support strcpy
#include <stdio.h> //support snprintf

#include "database_functions.h"
#include "../logging.h"
#include "../npc.h"
#include "../server_start_stop.h"

void load_db_npc_actions(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading npc action...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM NPC_ACTION_TABLE";

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

    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int npc_action_id=sqlite3_column_int(stmt,0);

        if(npc_action_id>MAX_NPC_ACTIONS){

            log_event(EVENT_ERROR, "npc action id [%i] exceeds range [%i] in function %s: module %s: line %i", npc_action_id, MAX_NPC_ACTIONS, __func__, __FILE__, __LINE__);
            stop_server();
        }

        npc_action[npc_action_id].action_type=sqlite3_column_int(stmt, 1);
        strcpy(npc_action[npc_action_id].text, (char*)sqlite3_column_text(stmt, 2));
        strcpy(npc_action[npc_action_id].options_list, (char*)sqlite3_column_text(stmt, 3));
        strcpy(npc_action[npc_action_id].text_success, (char*)sqlite3_column_text(stmt, 4));
        strcpy(npc_action[npc_action_id].text_fail, (char*)sqlite3_column_text(stmt, 5));
        npc_action[npc_action_id].choice=sqlite3_column_int(stmt, 6);
        npc_action[npc_action_id].object_id_required=sqlite3_column_int(stmt, 7);
        npc_action[npc_action_id].object_amount_required=sqlite3_column_int(stmt, 8);
        npc_action[npc_action_id].object_id_given=sqlite3_column_int(stmt, 9);
        npc_action[npc_action_id].object_amount_given=sqlite3_column_int(stmt, 10);
        npc_action[npc_action_id].boat_node=sqlite3_column_int(stmt, 11);
        npc_action[npc_action_id].destination=sqlite3_column_int(stmt, 12);

        log_event(EVENT_INITIALISATION, "loaded [%i]", npc_action_id);

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

        log_event(EVENT_ERROR, "no npc actions found in database", i);
        stop_server();
    }
}


void add_db_npc_action(int npc_action_id, int action_type, char *npc_text, char *options_list,
char *text_success, char *text_fail, int choice, int object_id_required, int object_amount_required,
int object_id_given, int object_amount_given, int boat_node, int destination){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO NPC_ACTION_TABLE("
        "NPC_ACTION_ID," \
        "NPC_ACTION_TYPE,"
        "NPC_TEXT," \
        "OPTIONS_LIST,"
        "TEXT_SUCCESS,"  \
        "TEXT_FAIL," \
        "CHOICE," \
        "OBJECT_ID_REQUIRED," \
        "OBJECT_AMOUNT_REQUIRED," \
        "OBJECT_ID_GIVEN, " \
        "OBJECT_AMOUNT_GIVEN," \
        "BOAT_NODE," \
        "DESTINATION," \
        ") VALUES(%i, %i, '%s', %s', '%s', '%s', %i, %i, %i, %i, %i, %i, %i)",
        npc_action_id, action_type, npc_text, options_list, text_success, text_fail, choice,
        object_id_required, object_amount_required, object_id_given, object_amount_given,
        boat_node, destination);

    process_sql(sql);

    printf("NPC action [%i] added successfully\n", npc_action_id);

    log_event(EVENT_SESSION, "Added NPC action [%i] to NPC_ACTION_TABLE", npc_action_id);
}
