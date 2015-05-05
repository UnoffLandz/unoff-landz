#include "database_functions.h"

#include "../file_functions.h"

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
static int set_db_version(sqlite3 *db,int new_version) {
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
    set_db_version(db,1);
    sqlite3_close(db);
    fprintf(stderr,"UPGRADE [v%d]: Success\n",1);
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

extern int current_database_version(const char *dbaname);
int upgrade_database(const char *dbname) {

    int old_version;
    int new_version;
    if(!file_exists(dbname)) {
        fprintf(stderr,"Cannot upgrade database %s - no such file\n",dbname);
        return -1;
    }
    old_version = current_database_version(dbname);
    new_version = CURRENT_DB_VERSION;
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
