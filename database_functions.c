/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

#include <stdio.h> //supports snprintf
#include <string.h> //supports strlen

#include "database_functions.h"
#include "logging.h"
#include "db_character_tbl.h"
#include "db_map_tbl.h"
#include "db_character_type_tbl.h"
#include "db_character_race_tbl.h"
#include "db_character_inventory_tbl.h"
#include "db_gender_tbl.h"
#include "db_chat_channel_tbl.h"
#include "db_attribute_tbl.h"
#include "server_start_stop.h"

void open_database(char *database_name){

   /** public function - see header **/

    int rc;

    rc = sqlite3_open(database_name, &db);

    if( rc ){

        log_sqlite_error("sqlite3_open", __func__ , __FILE__, __LINE__, rc, "");
    }
}

int database_table_count(){

    /** public function - see header **/

    int rc;
    sqlite3_stmt *stmt;
    int table_count=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE type='table';");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        table_count=sqlite3_column_int(stmt, 0);
    }

    rc=sqlite3_finalize(stmt);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return table_count;
}

void create_database_table(char *sql){

    /** RESULT  : creates a database table

       RETURNS  : void

       PURPOSE  : used by function create_new_database
    **/

    int rc;
    sqlite3_stmt *stmt;

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, sql);
    }

    //extract table name from sql string
    char table_name[80]="";
    int str_len=strlen(sql);

    int i=0;

    for(i=13; sql[i]!='('; i++){

        table_name[i-13]=sql[i];

        if(i>=str_len) {

            log_event(EVENT_ERROR, "unable to extract table name from sql string [%s] in function %s: module %s: line %i", sql, __func__, __FILE__, __LINE__);
            stop_server();
        }
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);
}

void create_new_database() {

    /** public function - see header **/

    //create logical divider in log file
    log_text(EVENT_INITIALISATION, "\nCreating database tables...");

    //create database tables
    create_database_table(CHARACTER_TABLE_SQL);
    create_database_table(INVENTORY_TABLE_SQL);
    create_database_table(GENDER_TABLE_SQL);
    //create_database_table(ITEM_TABLE_SQL);
    //create_database_table(THREED_OBJECT_TABLE_SQL);
    create_database_table(MAP_TABLE_SQL);
    create_database_table(CHANNEL_TABLE_SQL);
    create_database_table(RACE_TABLE_SQL);
    //create_database_table(GUILD_TABLE_SQL);
    create_database_table(CHARACTER_TYPE_TABLE_SQL);
    create_database_table(ATTRIBUTE_TABLE_SQL);
    create_database_table(ATTRIBUTE_VALUE_TABLE_SQL);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");
}

void process_sql(char *sql_str){

    /** public function - see header **/

    int rc=0;
    sqlite3_stmt *stmt;

    rc=sqlite3_prepare_v2(db, sql_str, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }

    rc=sqlite3_finalize(stmt);
    if(rc != SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }
}
