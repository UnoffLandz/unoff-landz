/****************************************************************************************************
    Copyright 2014, 2015 UnoffLandz

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Upgrade helper functions (by Nemerle)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static char *create_backup_name(const char *dbname,int ver) {

    char buf[4096];
    snprintf(buf,sizeof(buf),"ver_%d_of_%s",ver,dbname);
    char *res = (char *)malloc(strlen(buf)+1);
    strncpy(res,buf,strlen(buf)+1);
    return res;
}

static int create_backup(const char *dbname,int ver) {

    int copy_result;
    char *bak_fname = create_backup_name(dbname,ver);

    printf("UPGRADE [v%d]: Creating database backup - %s\n",ver,bak_fname);
    if(file_exists(bak_fname)) {
        printf("UPGRADE [v%d]: Backup file [%s] already exists - "
               "I'm not sure what to do, if it's old failed backup remove it by hand and retry\n",ver,bak_fname);
        return -1;

    }

    copy_result = fcopy(dbname,bak_fname);
    free(bak_fname);

    if(-1==copy_result) {
        printf("UPGRADE [v%d]: Copying existing db to backup failed\n",ver);
        return -1;
    }

    return 0;
}

static int callback(void *unused, int argc, char **argv, char **azColName){

    (void)unused; // suppress warning

    for(int i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");

    return 0;
}

static int set_db_version(int db_version) {

    char *err_msg = NULL;
    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN,"UPDATE GAME_DATA_TABLE SET DB_VERSION = %d", db_version);

    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if( rc != SQLITE_OK ) {

        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Upgrade functions go here
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int upgrade_v0_to_v1(const char *dbname) {

    sqlite3 *db;
    int rc;
    char *err_msg = NULL;

    rc = sqlite3_open(dbname, &db);

    if( rc !=SQLITE_OK ) {
        return -1;
    }

    rc = sqlite3_exec(db,"ALTER TABLE GAME_DATA_TABLE ADD COLUMN db_version INTEGER",callback,0,&err_msg);
    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    set_db_version(1);
    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n",1);

    return 0;
}

static int upgrade_v1_to_v2() {

    set_db_version(2);

    fprintf(stderr,"UPGRADE [v%d]: Success\n", 2);

    return 0;
}

static int upgrade_v2_to_v3(const char *dbname) {

    create_database_table(E3D_TABLE_SQL);

    create_database_table(OBJECT_TABLE_SQL);

    sqlite3 *db;
    char *err_msg = NULL;

    int rc = sqlite3_open(dbname, &db);

    if( rc !=SQLITE_OK ) {

        return -1;
    }

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS MAP_OBJECT_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    set_db_version(3);

    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n", 3);

    return 0;
}

static int upgrade_v3_to_v4(const char *dbname) {

    sqlite3 *db;
    char *err_msg = NULL;

    int rc = sqlite3_open(dbname, &db);

    if( rc !=SQLITE_OK ) {

        return -1;
    }

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS MAP_OBJECT_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    create_database_table(OBJECT_TABLE_SQL);

    set_db_version(4);

    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n", 4);

    return 0;
}

static int upgrade_v4_to_v5(const char *dbname) {

    sqlite3 *db;
    char *err_msg = NULL;

    int rc = sqlite3_open(dbname, &db);

    if( rc !=SQLITE_OK ) {

        return -1;
    }

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS E3D_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS MAP_OBJECT_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    create_database_table(E3D_TABLE_SQL);

    add_db_e3d(1, "cabbage.e3d", 405);
    add_db_e3d(2, "tomatoeplant1.e3d", 407);
    add_db_e3d(3, "tomatoeplant2.e3d", 407);
    add_db_e3d(4, "foodtomatoe.e3d", 407);
    add_db_e3d(5, "food_carrot.e3d", 408);
    add_db_e3d(8, "branch1.e3d", 140);
    add_db_e3d(9, "branch2.e3d", 140);
    add_db_e3d(10, "branch3.e3d", 140);
    add_db_e3d(11, "branch4.e3d", 140);
    add_db_e3d(12, "branch5.e3d", 140);
    add_db_e3d(13, "branch6.e3d", 140);
    add_db_e3d(15, "flowerorange1.e3d", 29);
    add_db_e3d(16, "flowerorange2.e3d", 29);
    add_db_e3d(17, "flowerorange3.e3d", 29);

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS OBJECT_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    create_database_table(OBJECT_TABLE_SQL);

    add_db_object(405, "cabbage", 1, 1, 2);
    add_db_object(407, "tomato", 1, 1, 2);
    add_db_object(408, "carrot", 1, 1, 2);
    add_db_object(140, "stick", 1, 0, 2);
    add_db_object(29, "tiger lily", 1, 0, 1);

    rc = sqlite3_exec(db,"DROP TABLE IF EXISTS MAP_OBJECT_TABLE", callback, 0, &err_msg);

    if( rc != SQLITE_OK ){

        fprintf(stderr,"UPGRADE [v%d]: Database alteration failed - %s\n",1,err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return -1;
    }

    create_database_table(MAP_OBJECT_TABLE_SQL);

    //add_db_map_objects("startup.elm", 1);

    set_db_version(5);

    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n", 5);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Array of upgrade procedures with their associated db versions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int (*upgrade_function)(const char *dbname);

struct upgrade_array_entry {

    uint32_t from_version;
    uint32_t to_version;
    upgrade_function fn;
};

struct upgrade_array_entry entries[] = {

    { 4, 5, upgrade_v4_to_v5},
    { 3, 4, upgrade_v3_to_v4},
    { 2, 3, upgrade_v2_to_v3},
    { 1, 2, upgrade_v1_to_v2},
    { 0, 1, upgrade_v0_to_v1},
    { 0, 0, NULL}
};

static const struct upgrade_array_entry *find_upgrade_entry(uint32_t old_version) {

    int idx=0;

    while(entries[idx].fn!=NULL) {

        if(entries[idx].from_version==old_version)
            return &entries[idx];

        ++idx;
    }

    return NULL;
}

extern int current_database_version();

void upgrade_database(const char *dbname) {

    if(!file_exists(dbname)) {

        fprintf(stderr,"Cannot upgrade database %s - no such file\n", dbname);
        return;
    }

    int old_version = current_database_version();
    int new_version = CURRENT_DB_VERSION;

    if(old_version>new_version) {

        fprintf(stderr,"Cannot update database : database is newer than server !\n");
        return;
    }

    while(old_version<new_version) {

        const struct upgrade_array_entry *entry = find_upgrade_entry(old_version);

        if(!entry){

            fprintf(stderr,"Cannot find entry for database\n");
            return;
        }

        fprintf(stdout,"DB version update %d to %d:",entry->from_version,entry->to_version);

        // backup is created before calling each upgrade function
        if(-1==create_backup(dbname,old_version)){

            return;
        }

        if(0==entry->fn(dbname)) {

            old_version = entry->to_version; // version upgrade successful
            fprintf(stdout,"OK\n");
        }
        else {

            fprintf(stdout,"FAILED\n");
            return;
        }
    }
}
