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

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    char sql[MAX_SQL_LEN]="SELECT * FROM NPC_TRIGGER_TABLE";

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

        int npc_trigger_id=sqlite3_column_int(stmt,0);

        if(npc_trigger_id>MAX_NPC_TRIGGERS){

            log_event(EVENT_ERROR, "npc trigger id [%i] exceeds range [%i] in function %s: module %s: line %i", npc_trigger_id, MAX_NPC_TRIGGERS, __func__, __FILE__, __LINE__);
            stop_server();
        }

        npc_trigger[npc_trigger_id].trigger_type=sqlite3_column_int(stmt, 1);
        npc_trigger[npc_trigger_id].trigger_time=sqlite3_column_int(stmt, 2);
        npc_trigger[npc_trigger_id].select_option=sqlite3_column_int(stmt, 3);
        npc_trigger[npc_trigger_id].action_node=sqlite3_column_int(stmt, 4);


        log_event(EVENT_INITIALISATION, "loaded [%i]", npc_trigger_id);

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

        log_event(EVENT_ERROR, "no npc triggers found in database", i);
        stop_server();
    }
}


void add_db_npc_trigger(int npc_trigger_id, int trigger_type, int trigger_time,
int select_option, int action_node){

    /** public function - see header */

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN,
        "INSERT INTO NPC_TRIGGER_TABLE("
        "NPC_TRIGGER_ID," \
        "NPC_TRIGGER_TYPE,"
        "NPC_TRIGGER_TIME,"  \
        "NPC_SELECT_OPTION,"
        "NPC_ACTION_NODE"
        ") VALUES(%i, %i, %i, %i, %i)", npc_trigger_id, trigger_type, trigger_time, select_option, action_node);

    process_sql(sql);

    fprintf(stderr, "NPC trigger [%i] added successfully\n", npc_trigger_id);

    log_event(EVENT_SESSION, "Added NPC trigger [%i] to NPC_TRIGGER_TABLE", npc_trigger_id);
}

