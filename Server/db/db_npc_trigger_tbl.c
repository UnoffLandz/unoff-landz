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

void load_db_npc_triggers(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading npc trigger...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("NPC_TRIGGER_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM NPC_TRIGGER_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int i=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int npc_trigger_id=sqlite3_column_int(stmt,0);

        if(npc_trigger_id>MAX_NPC_TRIGGERS){

            log_event(EVENT_ERROR, "npc trigger id [%i] exceeds range [%i] in function %s: module %s: line %i", npc_trigger_id, MAX_NPC_TRIGGERS, GET_CALL_INFO);
            stop_server();
        }

        npc_trigger[npc_trigger_id].trigger_type=sqlite3_column_int(stmt, 1);
        npc_trigger[npc_trigger_id].trigger_time=sqlite3_column_int(stmt, 2);
        npc_trigger[npc_trigger_id].select_option=sqlite3_column_int(stmt, 3);
        npc_trigger[npc_trigger_id].action_node=sqlite3_column_int(stmt, 4);


        log_event(EVENT_INITIALISATION, "loaded [%i]", npc_trigger_id);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no npc triggers found in database", i);
        stop_server();
    }
}


void add_db_npc_trigger(int npc_trigger_id, int trigger_type, int trigger_time,
    int select_option, int action_node){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("NPC_TRIGGER_TABLE", GET_CALL_INFO);

    char *sql="INSERT INTO NPC_TRIGGER_TABLE(" \
        "NPC_TRIGGER_ID," \
        "NPC_TRIGGER_TYPE,"
        "NPC_TRIGGER_TIME,"  \
        "NPC_SELECT_OPTION,"
        "NPC_ACTION_NODE"
        ") VALUES(?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt=NULL;

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, npc_trigger_id);
    sqlite3_bind_int(stmt, 2, trigger_type);
    sqlite3_bind_int(stmt, 3, trigger_time);
    sqlite3_bind_int(stmt, 4, select_option);
    sqlite3_bind_int(stmt, 5, action_node);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "NPC trigger [%i] added successfully\n", npc_trigger_id);

    log_event(EVENT_SESSION, "Added NPC trigger [%i] to NPC_TRIGGER_TABLE", npc_trigger_id);
}
