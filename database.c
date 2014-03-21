#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "global.h"
#include "database.h"
#include "log_in.h"
#include "harvesting.h"
#include "files.h"

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

void add_item(int image_id, char *item_name, int harvestable, int cycle_amount, int emu, int interval, int exp,
              int food_value,
              int food_cooldown,
              int organic_nexus,
              int vegetal_nexus){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024] ="INSERT INTO ITEM_TABLE("  \
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
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, image_id);
    sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, harvestable);
    sqlite3_bind_int(stmt, 4, cycle_amount);
    sqlite3_bind_int(stmt, 5, emu);
    sqlite3_bind_int(stmt, 6, interval);
    sqlite3_bind_int(stmt, 7, exp);
    sqlite3_bind_int(stmt, 8, food_value);
    sqlite3_bind_int(stmt, 9, food_cooldown);
    sqlite3_bind_int(stmt, 10, organic_nexus);
    sqlite3_bind_int(stmt, 11, vegetal_nexus);

    rc = sqlite3_step(stmt);


    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        //exit(1);
    }

    sqlite3_finalize(stmt);

    printf("Added item [%s] to ITEM_TABLE\n", item_name);
}

void add_threed_object(int id, char *filename, int image_id){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024] ="INSERT INTO THREED_OBJECT_TABLE("  \
        "OBJECT_ID," \
        "FILE_NAME,"  \
        "INVENTORY_IMAGE_ID " \
        ") VALUES( ?, ?, ?);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, filename, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, image_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);

    printf("Added 3d object [%s] to THREED_OBJECT_TABLE\n", filename);
}

void initialise_item_data(){

    /**                                               cycle                   food      org   veg
           id   item name          harvestable amount emu  interval exp value cooldown nexus nexus */
    add_item(28,  "Chrysanthemums",     1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(140, "Sticks",             1,           1,       4,   1,        1,   0,     0,        0,     0);
    add_item(33,  "Asiatic White Lily", 1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(29,  "Tiger Lily",         1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(35,  "Snapdragon",         1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(36,  "Lilac",              1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(214, "Logs",               1,           1,       6,   1,        1,   0,     0,        0,     0);
    add_item(26,  "Blue Star Flower",   1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(27,  "Impatiens",          1,           1,       1,   1,        1,   0,     0,        0,     0);
    add_item(407, "Tomato",             1,           1,       2,   2,        1,   1,     4,        0,     0);
    add_item(405, "Cabbage",            1,           1,       2,   2,        1,   1,     4,        0,     0);
    add_item(408, "Carrot",             1,           1,       2,   2,        1,   1,     4,        0,     0);
}

void initialise_threed_object_data(){

/**                order e3d filename      inventory
                                           image id */
    add_threed_object(1,    "flowerpink1.e3d",   28);
    add_threed_object(2,    "branch1.e3d",       140);
    add_threed_object(3,    "branch2.e3d",       140);
    add_threed_object(4,    "branch3.e3d",       140);
    add_threed_object(5,    "branch4.e3d",       140);
    add_threed_object(6,    "branch5.e3d",       140);
    add_threed_object(7,    "branch6.e3d",       140);
    add_threed_object(8,    "flowerorange1.e3d", 29);
    add_threed_object(9,    "flowerorange2.e3d", 29);
    add_threed_object(10,   "flowerorange3.e3d", 29);
    add_threed_object(11,   "flowerwhite1.e3d",  33);
    add_threed_object(12,   "flowerwhite2.e3d",  33);
    add_threed_object(13,   "flowerwhite3.e3d",  27);
    add_threed_object(14,   "flowerbush1.e3d",   36);
    add_threed_object(15,   "flowerbush2.e3d",   35);
    add_threed_object(16,   "flowerblue1.e3d",   26);
    add_threed_object(17,   "flowerblue2.e3d",   26);
    add_threed_object(18,   "log1.e3d",          214);
    add_threed_object(19,   "log2.e3d",          214);
    add_threed_object(20,   "tomatoeplant1.e3d", 407);
    add_threed_object(21,   "tomatoeplant2.e3d", 407);
    add_threed_object(22,   "foodtomatoe.e3d",   407);
    add_threed_object(23,   "food_carrot.e3d", 408);
    add_threed_object(24,   "cabbage.e3d", 405);
}

int get_max_char_id(){

    int rc;
    sqlite3_stmt *stmt;
    int max_id=0;

    char sql[1024]="SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

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

    char sql[1024]="SELECT * FROM CHARACTER_TABLE WHERE CHAR_NAME=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    character.char_id=NOT_FOUND;

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
        memcpy(character.inventory, sqlite3_column_blob(stmt, 33), (MAX_INVENTORY_SLOTS*8)+1);

        character.overall_exp=sqlite3_column_int(stmt, 34);
        character.harvest_exp=sqlite3_column_int(stmt, 35);
    }

    sqlite3_finalize(stmt);

    return character.char_id;
}

void create_character_table(){

    int rc;
    sqlite3_stmt *stmt;

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

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void create_item_table(){

    int rc;
    sqlite3_stmt *stmt;

    printf("Create ITEM_TABLE\n");

    char sql[1024] = "CREATE TABLE ITEM_TABLE("  \
        "IMAGE_ID           INT PRIMARY KEY     NOT NULL," \
        "ITEM_NAME           TEXT," \
        "HARVESTABLE         INT," \
        "CYCLE_AMOUNT        INT," \
        "EMU                 INT," \
        "INTERVAL            INT," \
        "EXP                 INT," \
        "FOOD_VALUE          INT," \
        "FOOD_COOLDOWN       INT," \
        "ORGANIC_NEXUS       INT," \
        "VEGETAL_NEXUS       INT );";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void create_3d_object_table(){

    int rc;
    sqlite3_stmt *stmt;

    printf("Create THREED_OBJECT TABLE\n");

    char sql[1024] = "CREATE TABLE THREED_OBJECT_TABLE("  \
        "OBJECT_ID           INT PRIMARY KEY     NOT NULL," \
        "FILE_NAME           TEXT,"  \
        "INVENTORY_IMAGE_ID  INT);";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void load_3d_objects(){

    int rc;
    sqlite3_stmt *stmt;
    int i=0;
    char text_out[1024]="";

    char sql[1024]="SELECT * FROM THREED_OBJECT_TABLE";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        strcpy(threed_object[i].file_name, (char*)sqlite3_column_text(stmt, 1));
        threed_object[i].inventory_image_id=sqlite3_column_int(stmt,2);

        i++;

        if(i>MAX_THREED_OBJECTS) {
            sprintf(text_out, "Maximum number of 3d objects exceeded in function load_3d_objects: module database.c");
            log_event(EVENT_ERROR, text_out);
            exit(1);
        }
    }

    sqlite3_finalize(stmt);
}

void load_items(){

    int rc;
    sqlite3_stmt *stmt;
    char text_out[1024]="";
    int id=0;

    char sql[1024]="SELECT * FROM ITEM_TABLE";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        id=sqlite3_column_int(stmt,0);

        if(id>MAX_ITEMS){
            sprintf(text_out, "item id [%i] exceeds MAX_ITEM array size [%i] in function load_items: module database.c", id, MAX_ITEMS);
            log_event(EVENT_ERROR, text_out);
            exit(1);
        }

        strcpy(item[id].item_name, (char*)sqlite3_column_text(stmt, 1));
        item[id].harvestable=sqlite3_column_int(stmt,2);
        item[id].cycle_amount=sqlite3_column_int(stmt,3);
        item[id].emu=sqlite3_column_int(stmt,4);
        item[id].interval=sqlite3_column_int(stmt,5);
        item[id].exp=sqlite3_column_int(stmt,6);
        item[id].food_value=sqlite3_column_int(stmt,7);
        item[id].food_cooldown=sqlite3_column_int(stmt,8);
        item[id].organic_nexus=sqlite3_column_int(stmt,9);
        item[id].vegetal_nexus=sqlite3_column_int(stmt,10);
     }

    sqlite3_finalize(stmt);
}

void update_db_char_position(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET MAP_TILE=?, MAP_ID=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection]->map_tile);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->map_id);
    sqlite3_bind_int(stmt, 3, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_name(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET CHAR_NAME='?' WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, clients.client[connection]->char_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
 }

void update_db_char_frame(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET FRAME=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

/** DEBUG */
    if(clients.client[connection]->frame==sit_down){
        printf("SIT saved to database\n");
    }
    else{
        printf("STAND saved to database\n");
    }

    sqlite3_bind_int(stmt, 1, clients.client[connection]->frame);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_stats(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET OVERALL_EXP=?, HARVEST_EXP=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection]->overall_exp);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->harvest_exp);
    sqlite3_bind_int(stmt, 3, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_last_in_game(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET LAST_IN_GAME=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, (int) clients.client[connection]->last_in_game);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_channels(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=?, CHAN_0=?, CHAN_1=?, CHAN_2=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection]->active_chan);
    sqlite3_bind_int(stmt, 2, clients.client[connection]->chan[0]);
    sqlite3_bind_int(stmt, 3, clients.client[connection]->chan[1]);
    sqlite3_bind_int(stmt, 4, clients.client[connection]->chan[2]);
    sqlite3_bind_int(stmt, 5, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function add_char\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_inventory(int connection){

    int rc;
    sqlite3_stmt *stmt;

    char sql[1024]="UPDATE CHARACTER_TABLE SET INVENTORY=?, INVENTORY_LENGTH=? WHERE CHAR_ID=?;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_blob(stmt, 1, clients.client[connection]->inventory, (MAX_INVENTORY_SLOTS*8)+1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, (MAX_INVENTORY_SLOTS*8)+1);
    sqlite3_bind_int(stmt, 3, clients.client[connection]->character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        printf("Error %s executing '%s' in function execute_sql\n", sql, sqlite3_errmsg(db));
        exit(1);
    }

    sqlite3_finalize(stmt);
}
