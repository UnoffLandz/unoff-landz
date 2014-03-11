#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "global.h"
#include "database.h"
#include "log_in.h"
#include "harvesting.h"

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

    //sqlite3_bind_int(stmt, 1, 16);

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

    //sqlite3_bind_int(stmt, 1, 16);

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

    //sqlite3_bind_int(stmt, 1, 16);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        strcpy(game_data.name_last_char_created, (char*)sqlite3_column_text(stmt, 0));
        game_data.date_last_char_created=sqlite3_column_int(stmt,1);
    }

    sqlite3_finalize(stmt);
}

void add_char(struct character_type character){

    int rc;
    sqlite3_stmt *stmt;
    unsigned char inventory[MAX_INVENTORY_SLOTS];

    inventory[0]=0;

    char sql[1024] ="INSERT INTO CHARACTER_TABLE(" \
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
        "CHAR_CREATED," \
        "INVENTORY_LENGTH," \
        "INVENTORY" \

        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, character.char_id);

    sqlite3_bind_text(stmt, 2, character.char_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, character.password, -1, SQLITE_STATIC);

    sqlite3_bind_int(stmt, 4, CHAR_ALIVE); // char status

    sqlite3_bind_int(stmt, 5, 0); // active_chan
    sqlite3_bind_int(stmt, 6, 0); // chan 0
    sqlite3_bind_int(stmt, 7, 0); // chan 1
    sqlite3_bind_int(stmt, 8, 0); // chan 2

    sqlite3_bind_int(stmt, 9, 0); // gm permission
    sqlite3_bind_int(stmt, 10, 0); // ig permission

    sqlite3_bind_int(stmt, 11, START_MAP_ID); // map id
    sqlite3_bind_int(stmt, 12, START_MAP_TILE); // map tile

    sqlite3_bind_int(stmt, 13, character.char_type);
    sqlite3_bind_int(stmt, 14, character.skin_type);
    sqlite3_bind_int(stmt, 15, character.hair_type);
    sqlite3_bind_int(stmt, 16, character.shirt_type);
    sqlite3_bind_int(stmt, 17, character.pants_type);
    sqlite3_bind_int(stmt, 18, character.boots_type);
    sqlite3_bind_int(stmt, 19, character.head_type);

    sqlite3_bind_int(stmt, 20, SHIELD_NONE);
    sqlite3_bind_int(stmt, 21, WEAPON_NONE);
    sqlite3_bind_int(stmt, 22, CAPE_NONE);
    sqlite3_bind_int(stmt, 23, HELMET_NONE);

    sqlite3_bind_int(stmt, 24, 0); // max health
    sqlite3_bind_int(stmt, 25, 0); // current health

    sqlite3_bind_int(stmt, 26, character.visual_proximity);
    sqlite3_bind_int(stmt, 27, character.local_text_proximity);

    sqlite3_bind_int(stmt, 28, character.char_created);

    sqlite3_bind_int(stmt, 29, 0); //inventory length
    sqlite3_bind_blob(stmt, 30, inventory, MAX_INVENTORY_SLOTS, SQLITE_STATIC); // inventory

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

        "IMAGE_ID,"  \
        "ITEM_NAME," \
        "HARVESTABLE,"  \
        "CYCLE_AMOUNT," \
        "EMU," \
        "INTERVAL," \
        "EXP," \
        "FOOD_VALUE," \
        "FOOD_COOLDOWN," \
        "ORGANIC_NEXUS," \
        "VEGETAL_NEXUS " \
        ") VALUES(" \

        "%i," \
        "'%s'," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i," \
        "%i" \
        ");",

        item.image_id,
        item.item_name,
        item.harvestable,
        item.cycle_amount,
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

void initialise_item_data(){

    strcpy(item.item_name, "Chrysanthemum");
    item.image_id=28;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=0;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Sticks");
    item.image_id=140;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=4;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Asiatic White Lily");
    item.image_id=33;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Tiger Lily");
    item.image_id=29;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Snapdragon");
    item.image_id=35;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Lilac");
    item.image_id=36;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Logs");
    item.image_id=214;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=10;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Blue Star Flower");
    item.image_id=26;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Impatiens");
    item.image_id=27;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Tomato");
    item.image_id=407;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=1;
    item.food_cooldown=1;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Cabbage");
    item.image_id=405;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=1;
    item.food_cooldown=1;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

    strcpy(item.item_name, "Carrot");
    item.image_id=408;
    item.exp=1;
    //item.harvestable=1;
    item.cycle_amount=1;
    item.emu=1;
    item.interval=1;
    item.exp=1;
    item.food_value=0;
    item.food_cooldown=0;
    item.organic_nexus=0;
    item.vegetal_nexus=0;

    add_item(item);
    printf("Added %s to item table\n", item.item_name);

}


void add_threed_object(int id, char *filename, int image_id){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024] ="";

    sprintf(sql, "INSERT INTO THREED_OBJECT_TABLE("  \

        "OBJECT_ID," \
        "FILE_NAME,"  \
        "IMAGE_ID " \
        ") VALUES(" \

        "%i," \
        "'%s'," \
        "%i" \
        ");",

        id,
        filename,
        image_id
        );

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void initialise_threed_object_data(){

    add_threed_object(1, "flowerpink1.e3d", 28);
}

int get_max_char_id(){

    int rc;
    sqlite3_stmt *stmt;

    int max_id=0;
    char sql[1024]="SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    //sqlite3_bind_int(stmt, 1, 16);

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

    character.char_id=NOT_FOUND;

    sprintf(sql, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_NAME='%s'", name);
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    //sqlite3_bind_int(stmt, 1, 16);

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

        character.inventory_length=sqlite3_column_int(stmt, 32);
        memcpy(character.inventory, sqlite3_column_blob(stmt, 33), character.inventory_length);

        character.overall_exp=sqlite3_column_int(stmt, 34);
        character.harvest_exp=sqlite3_column_int(stmt, 35);
    }

    sqlite3_finalize(stmt);

    return character.char_id;
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
        "INVENTORY_LENGTH    INT," \
        "INVENTORY           BLOB," \
        "OVERALL_EXP         INT," \
        "HARVEST_EXP         INT );";

    execute_sql(sql);
}

void create_item_table(){

    printf("Create ITEM_TABLE\n");

    char sql[1024] = "CREATE TABLE ITEM_TABLE("  \
        "IMAGE_ID           INT PRIMARY KEY     NOT NULL," \
        "ITEM_NAME           TEXT," \
        "CYCLE_AMOUNT        INT," \
        "EMU                 INT," \
        "INTERVAL            INT," \
        "EXP                 INT," \
        "FOOD_VALUE          INT," \
        "FOOD_COOLDOWN       INT," \
        "ORGANIC_NEXUS       INT," \
        "VEGETAL_NEXUS       INT );";

    execute_sql(sql);
}

void create_3d_object_table(){

    printf("Create THREED_OBJECT TABLE\n");

    char sql[1024] = "CREATE TABLE THREED_OBJECT_TABLE("  \
        "OBJECT_ID           INT PRIMARY KEY     NOT NULL," \
        "FILE_NAME           TEXT,"  \
        "IMAGE_ID            INT);";

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

void update_db_char_inventory(int connection){

    int rc;
    sqlite3_stmt *stmt;
    char sql[1024]="";

    sprintf(sql, "UPDATE CHARACTER_TABLE SET INVENTORY=?, INVENTORY_LENGTH=? WHERE CHAR_ID=%i;", clients.client[connection]->character_id );

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_blob(stmt, 1, clients.client[connection]->inventory, clients.client[connection]->inventory_length, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}
