/****************************************************************************************************
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
******************************************************************************************************/
/*****************************************************************************************************

                    ~~~~ credit goes to Nemerle for this module ~~~~

*****************************************************************************************************/
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "database_functions.h"
#include "../file_functions.h"
#include "db_object_tbl.h"
#include "db_e3d_tbl.h"
#include "db_map_object_tbl.h"
#include "../game_data.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "db_game_data_tbl.h"
#include "db_upgrade.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Upgrade helper functions (originally by Nemerle then appallingly hacked about by the ebul muntdregger)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int callback(void *unused, int argc, char **argv, char **azColName){

    (void)unused; // suppress warning

    for(int i=0; i<argc; i++){

        fprintf(stderr, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    fprintf(stderr, "\n");

    return 0;
}
*/

void set_db_version(int db_version) {

    /** RESULT  : sets the database version entry in the database

        RETURNS : void

        PURPOSE : to enable database upgrade procedures and error checking

        NOTES   :
    **/

    //check database is open
    check_db_open(GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN,"UPDATE GAME_DATA_TABLE SET DB_VERSION = %d", db_version);
    process_sql(sql);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Upgrade functions go here
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int upgrade_v0_to_v1(const char *dbname) {

    /** RESULT   : upgrades database from v0 to v1

        RETURNS  : void

        PURPOSE  :

        NOTES    :
    **/

    (void) dbname;

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="ALTER TABLE GAME_DATA_TABLE ADD COLUMN TEST TEXT";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //process the sql statement
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW);

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //set the new database version
    set_db_version(1);


    fprintf(stdout,"Database upgrade from version 0 to version 1 successful\n");

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Array of upgrade procedures with their associated db versions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int (*upgrade_function)(const char *dbname);

struct upgrade_array_entry {

    int from_version;
    int to_version;
    upgrade_function fn;
};

struct upgrade_array_entry entries[] = {

//    Version   Version
//    Upgrading Upgrading   Upgrade
//    From      To          Function
//    --------- ----------- -------------
    { 0,        1,          upgrade_v0_to_v1},
    { 0,        0,          NULL}
};

static const struct upgrade_array_entry *find_upgrade_entry(int old_version) {

    /** RESULT  : finds upgrade command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int idx=0;

    while(entries[idx].fn!=NULL) {

        if(entries[idx].from_version==old_version)
            return &entries[idx];

        ++idx;
    }

    return NULL;
}


void upgrade_database(const char *dbname) {

    /** public function - see header */

    //check database is open
    check_db_open(GET_CALL_INFO);

    load_db_game_data();

    //compare database version entry with the hard-coded version in the code-base
    if(game_data.database_version==REQUIRED_DATABASE_VERSION) {

        fprintf(stderr,"Database [%s] is already up to date at version [%i]\n", dbname, REQUIRED_DATABASE_VERSION);
        log_event(EVENT_ERROR, "Database [%s] is already up to date at version [%i]", dbname, REQUIRED_DATABASE_VERSION);
        stop_server();
    }
    else if(game_data.database_version>REQUIRED_DATABASE_VERSION) {

        fprintf(stderr,"Database [%s] is version [%i] which is newer than this codebase version [%i]\n", dbname, game_data.database_version, REQUIRED_DATABASE_VERSION);
        log_event(EVENT_ERROR, "Database [%s] is version [%i] which is newer than this codebase version [%i]", dbname, game_data.database_version, REQUIRED_DATABASE_VERSION);
        stop_server();
    }

    while(game_data.database_version<REQUIRED_DATABASE_VERSION) {

        // find the function that upgrades the database to the next version
        const struct upgrade_array_entry *entry = find_upgrade_entry(game_data.database_version);

        if(!entry){

            log_event(EVENT_ERROR, "can't find entry for database in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            stop_server();
        }

        fprintf(stderr,"Database version update %d to %d\n", entry->from_version, entry->to_version);
        log_event(EVENT_ERROR, "Database version update %d to %d:", entry->from_version, entry->to_version);

        // backup is created before calling each upgrade function
        create_backup_file(dbname, game_data.database_version);

        if(entry->fn(dbname)==0) {

            game_data.database_version = entry->to_version;
            fprintf(stderr,"Database update successful\n");
            log_event(EVENT_ERROR, "Database update successful");
        }
        else {

            fprintf(stderr,"Database update failed\n");
            log_event(EVENT_ERROR, "Database update failed");
            stop_server();
        }
    }
}
