#include "database_functions.h"
#include "../file_functions.h"
#include "db_object_tbl.h"

#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DB Upgrade helper functions
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

static int set_db_version(int new_version) {

    char *err_msg = NULL;
    char buf[512];

    snprintf(buf,512,"UPDATE GAME_DATA_TABLE SET db_version = %d",new_version);

    int rc = sqlite3_exec(db,buf,callback,0,&err_msg);
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

    create_database_table(OBJECT_TABLE_SQL);

    add_db_object(1, "cabbage.e3d", "cabbage", 405, 1, 1);
    add_db_object(2, "tomatoeplant1.e3d", "tomato", 407, 1, 1);
    add_db_object(3, "tomatoeplant2.e3d", "tomato", 407, 1, 1);
    add_db_object(4, "foodtomatoe.e3d", "tomato", 407, 1, 1);
    add_db_object(5, "food_carrot.e3d", "carrot", 408, 1, 1);
    add_db_object(6, "log1.e3d", "log", 408, 1, 0);
    add_db_object(7, "log2.e3d", "log", 408, 1, 0);
    add_db_object(8, "flowerpink1.e3d", "Chrysanthemum", 28, 1, 0);
    add_db_object(9, "branch1.e3d", "stick", 140, 1, 0);
    add_db_object(10, "branch2.e3d", "stick", 140, 1, 0);
    add_db_object(11, "branch3.e3d", "stick", 140, 1, 0);
    add_db_object(12, "branch4.e3d", "stick", 140, 1, 0);
    add_db_object(13, "branch5.e3d", "stick", 140, 1, 0);
    add_db_object(14, "branch6.e3d", "stick", 140, 1, 0);
    add_db_object(15, "flowerorange1.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(16, "flowerorange2.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(17, "flowerorange3.e3d", "Tiger Lily", 29, 1, 0);
    add_db_object(18, "flowerwhite1.e3d", "Impatiens", 29, 1, 0);
    add_db_object(19, "flowerwhite2.e3d", "Impatiens", 29, 1, 0);
    add_db_object(20, "flowerwhite3.e3d", "Impatiens", 29, 1, 0);

    set_db_version(2);

    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n", 2);

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

int upgrade_database(const char *dbname) {

    if(!file_exists(dbname)) {

        fprintf(stderr,"Cannot upgrade database %s - no such file\n", dbname);

        return -1;
    }

    open_database(dbname);

    int old_version = current_database_version();
    int new_version = CURRENT_DB_VERSION;

    if(old_version>new_version) {

        fprintf(stderr,"Cannot update database : database is newer than server !\n");

        return -1;
    }

    while(old_version<new_version) {

        const struct upgrade_array_entry *entry = find_upgrade_entry(old_version);

        if(!entry)
            return -1;

        fprintf(stdout,"DB version update %d to %d:",entry->from_version,entry->to_version);

        // backup is created before calling each upgrade function
        if(-1==create_backup(dbname,old_version)) {

            return -1;
        }

        if(0==entry->fn(dbname)) {

            old_version = entry->to_version; // version upgrade successful
            fprintf(stdout,"OK\n");
        }
        else {

            fprintf(stdout,"FAILED\n");

            return -1;
        }
    }

    return 0;
}
