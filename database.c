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

int get_chars_created_count(){

    int rc;
    sqlite3_stmt *stmt;

    int char_count=0;

    char sql[1024]="SELECT count(CHAR_ID) FROM CHARACTER_TABLE;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        char_count=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return char_count;
}

void get_last_char_created(){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="SELECT CHAR_NAME, CHAR_CREATED FROM CHARACTER_TABLE ORDER BY CHAR_CREATED LIMIT 1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        strcpy(game_data.name_last_char_created, (char*)sqlite3_column_text(stmt, 0));
        game_data.date_last_char_created=sqlite3_column_int(stmt,1);
    }

    sqlite3_finalize(stmt);
}

void add_char(struct character_type character){

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

        character.char_id,
        character.char_name,
        character.password,
        CHAR_ALIVE,
        0,   //active chan
        0,   //chan 0
        0,   //chan 1
        0,   //chan 2
        0,   //gm permission
        0,   //ig permission
        START_MAP_ID,
        START_MAP_TILE,
        character.char_type,
        character.skin_type,
        character.hair_type,
        character.shirt_type,
        character.pants_type,
        character.boots_type,
        character.head_type,
        SHIELD_NONE,
        WEAPON_NONE,
        CAPE_NONE,
        HELMET_NONE,
        0,  //max health
        0,  //current health
        character.visual_proximity,
        character.local_text_proximity,
        character.char_created
        );

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void add_item(struct item_type item){

    //function not used

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024] ="";

    sprintf(sql, "INSERT INTO ITEM_TABLE("  \

        "ITEM_ID," \
        "IMAGE_ID,"  \
        "ITEM_NAME," \
        "HARVESTABLE,"  \
        "EMU," \
        "INTERVAL," \
        "EXP," \
        "FOOD_VALUE," \
        "FOOD_COOLDOWN," \
        "ORGANIC_NEXUS," \
        "VEGETAL_NEXUS"
        ") VALUES(" \

        "%i," \
        "%i," \
        "'%s'," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i" \
        ");",

        item.item_id,
        item.image_id,
        item.item_name,
        item.harvestable,
        item.emu,
        item.interval,
        item.exp,
        item.food_value,
        item.food_cooldown,
        item.organic_nexus,
        item.vegetal_nexus
        );

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
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

int get_char_data(char *name){

    //loads character data from the db to the char struct

    int rc;
    sqlite3_stmt *stmt;
    char sql[1024]="";

    character.char_id=CHAR_NOT_FOUND;

    sprintf(sql, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_NAME='%s';", name);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        character.char_id=sqlite3_column_int(stmt, 0);
        strcpy(character.char_name, (char*) sqlite3_column_text(stmt, 1));
        strcpy(character.password, (char*) sqlite3_column_text(stmt,2));
        character.char_status=sqlite3_column_int(stmt, 3);
        character.active_chan=sqlite3_column_int(stmt, 4);
        character.chan[0]=sqlite3_column_int(stmt, 5);
        character.chan[1]=sqlite3_column_int(stmt, 6);
        character.chan[2]=sqlite3_column_int(stmt, 7);
        character.gm_permission=sqlite3_column_int(stmt, 8);
        character.ig_permission=sqlite3_column_int(stmt, 9);
        character.map_id=sqlite3_column_int(stmt, 10);
        character.map_tile=sqlite3_column_int(stmt, 11);
        character.guild_id=sqlite3_column_int(stmt, 12);
        character.char_type=sqlite3_column_int(stmt, 13);
        character.skin_type=sqlite3_column_int(stmt, 14);
        character.hair_type=sqlite3_column_int(stmt, 15);
        character.shirt_type=sqlite3_column_int(stmt, 16);
        character.pants_type=sqlite3_column_int(stmt, 17);
        character.boots_type=sqlite3_column_int(stmt, 18);
        character.head_type=sqlite3_column_int(stmt, 19);
        character.shield_type=sqlite3_column_int(stmt, 20);
        character.weapon_type=sqlite3_column_int(stmt, 21);
        character.cape_type=sqlite3_column_int(stmt, 22);
        character.helmet_type=sqlite3_column_int(stmt, 23);
        character.frame=sqlite3_column_int(stmt, 24);
        character.max_health=sqlite3_column_int(stmt, 25);
        character.current_health=sqlite3_column_int(stmt, 26);
        character.visual_proximity=sqlite3_column_int(stmt, 27);
        character.local_text_proximity=sqlite3_column_int(stmt, 28);
        character.last_in_game=sqlite3_column_int(stmt,  29);
        character.char_created=sqlite3_column_int(stmt, 30);
        character.joined_guild=sqlite3_column_int(stmt, 31);
        character.overall_exp=sqlite3_column_int(stmt, 32);
        character.harvest_exp=sqlite3_column_int(stmt, 33);
    }

    sqlite3_finalize(stmt);

    return character.char_id;
}

int get_item_data(int item_id){

    //loads item data from the db to the item struct

    int rc;
    sqlite3_stmt *stmt;
    char sql[1024]="";

    item.item_id=-1;

    sprintf(sql, "SELECT * FROM ITEM_TABLE WHERE ITEM_ID=%i;", item_id);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        item.item_id=sqlite3_column_int(stmt,0);
        item.image_id=sqlite3_column_int(stmt,1);
        strcpy(item.item_name, (char*) sqlite3_column_text(stmt,2));
        item.harvestable=sqlite3_column_int(stmt,3);
        item.emu=sqlite3_column_int(stmt,4);
        item.interval=sqlite3_column_int(stmt,5);
        item.exp=sqlite3_column_int(stmt,6);
        item.food_value=sqlite3_column_int(stmt,7);
        item.food_cooldown=sqlite3_column_int(stmt,8);
        item.organic_nexus=sqlite3_column_int(stmt,9);
        item.vegetal_nexus=sqlite3_column_int(stmt,10);
    }

    sqlite3_finalize(stmt);

    return item.item_id;
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

void create_character_table(){

    printf("Create CHARACTER_TABLE\n");

    char sql[1024] = "CREATE TABLE CHARACTER_TABLE("  \
        "CHAR_ID             INT PRIMARY KEY NOT NULL," \
        "CHAR_NAME           TEXT            NOT NULL," \
        "PASSWORD            TEXT            NOT NULL," \
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

void create_item_table(){

    printf("Create ITEM_TABLE\n");

    char sql[1024] = "CREATE TABLE ITEM_TABLE("  \
        "ITEM_ID             INT PRIMARY KEY     NOT NULL," \
        "IMAGE_ID            INT,"  \
        "ITEM_NAME           TEXT," \
        "HARVESTABLE         INT,"  \
        "EMU                 INT," \
        "INTERVAL            INT," \
        "EXP                 INT," \
        "FOOD_VALUE          INT," \
        "FOOD_COOLDOWN       INT," \
        "ORGANIC_NEXUS       INT," \
        "VEGETAL             INT );";

    execute_sql(sql);
}

void update_db_char_position(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET " \
        "MAP_TILE=%i, " \
        "MAP_ID=%i "
        "WHERE CHAR_ID=%i;",
        clients.client[connection]->map_tile,
        clients.client[connection]->map_id,
        clients.client[connection]->character_id );

    execute_sql(sql);
}

void update_db_char_name(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET CHAR_NAME='%s' WHERE CHAR_ID=%i;", clients.client[connection]->char_name, clients.client[connection]->character_id );
    execute_sql(sql);
}

void update_db_char_frame(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i;", clients.client[connection]->frame, clients.client[connection]->character_id );
    execute_sql(sql);
}

void update_db_char_stats(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET " \
        "OVERALL_EXP=%i, " \
        "HARVEST_EXP=%i "  \
        "WHERE CHAR_ID=%i;",
        clients.client[connection]->overall_exp,
        clients.client[connection]->harvest_exp,
        clients.client[connection]->character_id );

    execute_sql(sql);
}

void update_db_char_last_in_game(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET " \
        "LAST_IN_GAME=%i " \
        "WHERE CHAR_ID=%i;",
        (int) clients.client[connection]->last_in_game,
        clients.client[connection]->character_id );

    execute_sql(sql);
}

void update_db_char_channels(int connection){

    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET " \
        "ACTIVE_CHAN=%i, " \
        "CHAN_0=%i, " \
        "CHAN_1=%i, " \
        "CHAN_2=%i " \
        "WHERE CHAR_ID=%i;",
        clients.client[connection]->active_chan,
        clients.client[connection]->chan[0],
        clients.client[connection]->chan[1],
        clients.client[connection]->chan[2],
        clients.client[connection]->character_id );

    execute_sql(sql);
}

