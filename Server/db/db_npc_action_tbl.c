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

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("NPC_ACTION_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM NPC_ACTION_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int npc_action_id=sqlite3_column_int(stmt,0);

        if(npc_action_id>MAX_NPC_ACTIONS){

            log_event(EVENT_ERROR, "npc action id [%i] exceeds range [%i] in function %s: module %s: line %i", npc_action_id, MAX_NPC_ACTIONS, GET_CALL_INFO);
            stop_server();
        }

        npc_action[npc_action_id].action_type=sqlite3_column_int(stmt, 1);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(npc_action[npc_action_id].text, (char*)sqlite3_column_text(stmt, 2));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 3)) strcpy(npc_action[npc_action_id].options_list, (char*)sqlite3_column_text(stmt, 3));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 4)) strcpy(npc_action[npc_action_id].text_success, (char*)sqlite3_column_text(stmt, 4));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 5)) strcpy(npc_action[npc_action_id].text_fail, (char*)sqlite3_column_text(stmt, 5));

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

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no npc actions found in database", i);
        stop_server();
    }
}


void add_db_npc_action(int npc_action_id, int action_type, char *npc_text, char *options_list,
    char *text_success, char *text_fail, int choice, int object_id_required, int object_amount_required,
    int object_id_given, int object_amount_given, int boat_node, int destination){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("NPC_ACTION_TABLE", GET_CALL_INFO);

    char *sql="INSERT INTO NPC_ACTION_TABLE(" \
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
        ") VALUES(?, ?, ?, ?, ?', ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt=NULL;

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, npc_action_id);
    sqlite3_bind_int(stmt, 2, action_type);
    sqlite3_bind_text(stmt, 3, npc_text, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, options_list, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, text_success, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, text_fail, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, choice);
    sqlite3_bind_int(stmt, 8, object_id_required);
    sqlite3_bind_int(stmt, 9, object_amount_required);
    sqlite3_bind_int(stmt, 10, object_id_given);
    sqlite3_bind_int(stmt, 11, object_amount_given);
    sqlite3_bind_int(stmt, 12, boat_node);
    sqlite3_bind_int(stmt, 13, destination);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "NPC action [%i] added successfully\n", npc_action_id);

    log_event(EVENT_SESSION, "Added NPC action [%i] to NPC_ACTION_TABLE", npc_action_id);
}
