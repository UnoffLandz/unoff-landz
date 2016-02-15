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
#include <stdio.h> //support for sprintf

#include "../characters.h"
#include "database_functions.h"
#include "../logging.h"

void get_db_char_inventory(int character_id){

    /** public function - see header **/

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT SLOT, IMAGE_ID, AMOUNT FROM INVENTORY_TABLE WHERE CHAR_ID=%i", character_id);

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_DONE){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //cycle through all the inventory records and add them to the struct
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int slot=sqlite3_column_int(stmt, 0);
        character.inventory[slot].object_id=sqlite3_column_int(stmt, 1);
        character.inventory[slot].amount=sqlite3_column_int(stmt, 2);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }
}


void add_db_char_inventory(struct client_node_type character){

    /** public function - see header **/

    //check database is open
    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
    }

    int rc=0;
    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "INSERT INTO INVENTORY_TABLE(CHAR_ID, SLOT, IMAGE_ID, AMOUNT) VALUES(?, ?, ?, ?)");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //wrap in a transaction to speed up insertion
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    for(int i=0; i<MAX_EQUIP_SLOT+1; i++){

        //usually, we'd start by binding an inventory_id to column 0. However, sqlite creates this automatically
        sqlite3_bind_int(stmt, 1, character.character_id);
        sqlite3_bind_int(stmt, 2, i);
        sqlite3_bind_int(stmt, 3, character.inventory[i].object_id);
        sqlite3_bind_int(stmt, 4, character.inventory[i].amount);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {

            log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "added inventory of [%i] slots to database for char [%s]", MAX_EQUIP_SLOT+1, character.char_name);
}
