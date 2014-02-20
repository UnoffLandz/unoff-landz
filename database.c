#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "global.h"
#include "database.h"
#include "log_in.h"

void open_database(char *database_name){

    int rc;

    rc = sqlite3_open(database_name, &db);

    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    else{
        fprintf(stdout, "Opened database successfully\n");
    }
}

int get_table_count(){

    int rc;
    sqlite3_stmt *stmt;

    int table_count=0;

    char sql[1024]="SELECT count(*) FROM sqlite_master WHERE type='table';";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        table_count=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return table_count;
}

void add_char(struct new_character_type new_character){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024] ="";

    sprintf(sql, "INSERT INTO CHARACTER_TABLE("  \
        "CHAR_ID," \
        "CHAR_NAME," \
        "PASSWORD," \
        "CHAR_STATUS," \
        "ACTIVE_CHAN," \
        "CHAN_0," \
        "CHAN_1," \
        "CHAN_2," \
        "GM_PERMISSION," \
        "IG_PERMISSION," \
        "MAP_ID," \
        "MAP_TILE," \
        "CHAR_TYPE," \
        "SKIN_TYPE," \
        "HAIR_TYPE," \
        "SHIRT_TYPE," \
        "PANTS_TYPE," \
        "BOOTS_TYPE," \
        "HEAD_TYPE," \
        "SHIELD_TYPE," \
        "WEAPON_TYPE," \
        "CAPE_TYPE," \
        "HELMET_TYPE," \
        "MAX_HEALTH," \
        "CURRENT_HEALTH," \
        "VISUAL_PROXIMITY," \
        "LOCAL_TEXT_PROXIMITY," \
        "CHAR_CREATED" \

        ") VALUES(" \

        "%i," \
        "'%s'," \
        "'%s'," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i" \

        ");", \

        new_character.char_id,
        new_character.char_name,
        new_character.password,
        CHAR_ALIVE,
        0,   //active chan
        0,   //chan 0
        0,   //chan 1
        0,   //chan 2
        0,   //gm permission
        0,   //ig permission
        START_MAP_ID,
        START_MAP_TILE,
        new_character.char_type,
        new_character.skin_type,
        new_character.hair_type,
        new_character.shirt_type,
        new_character.pants_type,
        new_character.boots_type,
        new_character.head_type,
        SHIELD_NONE,
        WEAPON_NONE,
        CAPE_NONE,
        HELMET_NONE,
        0,  //max health
        0,  //current health
        new_character.visual_proximity,
        new_character.local_text_proximity,
        new_character.char_created
        );

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

int get_max_char_id(){

    int rc;
    sqlite3_stmt *stmt;

    int max_id=0;
    char sql[1024]="SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        max_id=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return max_id;
}

int get_char_id(char *name){

    int rc;
    sqlite3_stmt *stmt;

    int id=0;
    char sql[1024]="";

    sprintf(sql, "SELECT CHAR_ID FROM CHARACTER_TABLE WHERE CHAR_NAME='%s';", name);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        id=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return id;
}



void execute_sql(char *sql){

    sqlite3_stmt *stmt;
    int  rc;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void create_tables(){

    printf("Create CHARACTER_TABLE\n");

    char sql[1024] = "CREATE TABLE CHARACTER_TABLE("  \
        "CHAR_ID INT PRIMARY KEY     NOT NULL," \
        "CHAR_NAME           TEXT    NOT NULL," \
        "PASSWORD            TEXT    NOT NULL," \
        "CHAR_STATUS         INT," \
        "ACTIVE_CHAN         INT," \
        "CHAN_0              INT," \
        "CHAN_1              INT," \
        "CHAN_2              INT," \
        "GM_PERMISSION       INT," \
        "IG_PERMISSION       INT," \
        "MAP_ID              INT," \
        "MAP_TILE            INT," \
        "GUILD_ID            INT," \
        "CHAR_TYPE           INT," \
        "SKIN_TYPE           INT," \
        "HAIR_TYPE           INT," \
        "SHIRT_TYPE          INT," \
        "PANTS_TYPE          INT," \
        "BOOTS_TYPE          INT," \
        "HEAD_TYPE           INT," \
        "SHIELD_TYPE         INT," \
        "WEAPON_TYPE         INT," \
        "CAPE_TYPE           INT," \
        "HELMET_TYPE         INT," \
        "FRAME               INT," \
        "MAX_HEALTH          INT," \
        "CURRENT_HEALTH      INT," \
        "VISUAL_PROXIMITY    INT," \
        "LOCAL_TEXT_PROXIMITY INT," \
        "LAST_IN_GAME        INT," \
        "CHAR_CREATED        INT," \
        "JOINED_GUILD        INT," \
        "OVERALL_EXP         INT," \
        "HARVEST_EXP         INT );";

    execute_sql(sql);
}

void load_character_from_database(int char_id, int connection){

    int rc;
    sqlite3_stmt *stmt;
    char sql[1024]="";

    sprintf(sql, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_ID=%i;", char_id);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        clients.client[connection]->character_id=sqlite3_column_int(stmt, 0);
        strcpy(clients.client[connection]->char_name, (char*) sqlite3_column_text(stmt, 1));
        strcpy(clients.client[connection]->password, (char*) sqlite3_column_text(stmt, 2));
        clients.client[connection]->char_status=sqlite3_column_int(stmt, 3);
        clients.client[connection]->active_chan=sqlite3_column_int(stmt, 4);
        clients.client[connection]->chan[0]=sqlite3_column_int(stmt, 5);
        clients.client[connection]->chan[1]=sqlite3_column_int(stmt, 6);
        clients.client[connection]->chan[2]=sqlite3_column_int(stmt, 7);
        clients.client[connection]->gm_permission=sqlite3_column_int(stmt, 8);
        clients.client[connection]->ig_permission=sqlite3_column_int(stmt, 9);
        clients.client[connection]->map_id=sqlite3_column_int(stmt, 10);
        clients.client[connection]->map_tile=sqlite3_column_int(stmt, 11);
        clients.client[connection]->guild_id=sqlite3_column_int(stmt, 12);
        clients.client[connection]->char_type=sqlite3_column_int(stmt, 13);
        clients.client[connection]->skin_type=sqlite3_column_int(stmt, 14);
        clients.client[connection]->hair_type=sqlite3_column_int(stmt, 15);
        clients.client[connection]->shirt_type=sqlite3_column_int(stmt, 16);
        clients.client[connection]->pants_type=sqlite3_column_int(stmt, 17);
        clients.client[connection]->boots_type=sqlite3_column_int(stmt, 18);
        clients.client[connection]->head_type=sqlite3_column_int(stmt, 19);
        clients.client[connection]->shield_type=sqlite3_column_int(stmt, 20);
        clients.client[connection]->weapon_type=sqlite3_column_int(stmt, 21);
        clients.client[connection]->cape_type=sqlite3_column_int(stmt, 22);
        clients.client[connection]->helmet_type=sqlite3_column_int(stmt, 23);
        clients.client[connection]->frame=sqlite3_column_int(stmt, 24);
        clients.client[connection]->max_health=sqlite3_column_int(stmt, 25);
        clients.client[connection]->current_health=sqlite3_column_int(stmt, 26);
        clients.client[connection]->visual_proximity=sqlite3_column_int(stmt, 27);
        clients.client[connection]->local_text_proximity=sqlite3_column_int(stmt, 28);
        clients.client[connection]->last_in_game=sqlite3_column_int(stmt, 29);
        clients.client[connection]->char_created=sqlite3_column_int(stmt, 30);
        clients.client[connection]->joined_guild=sqlite3_column_int(stmt, 31);
        clients.client[connection]->overall_exp=sqlite3_column_int(stmt, 32);
        clients.client[connection]->harvest_exp=sqlite3_column_int(stmt, 33);
    }

    sqlite3_finalize(stmt);
}

