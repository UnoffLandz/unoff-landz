#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "database.h"
#include "log_in.h"
#include "character_inventory.h"
#include "files.h"
#include "numeric_functions.h"

void open_database(char *database_name){

   /** public function - see header */

    int rc;

    rc = sqlite3_open(database_name, &db);

    if( rc ){

        log_event2(EVENT_INITIALISATION, "Can't open database [%s]. Error [%s] in function open_database: module database.c", database_name, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    log_event2(EVENT_INITIALISATION, "Opened database [%s] successfully", database_name);
    log_event2(EVENT_INITIALISATION, "---");
}

int database_table_count(){


    int rc;
    sqlite3_stmt *stmt;

    int table_count=0;

    char sql[]="SELECT count(*) FROM sqlite_master WHERE type='table';";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        table_count=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return table_count;
}

int get_max_char_id(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    int max_id=0;

    char sql[]="SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

     while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        max_id=sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return max_id;
}

void get_last_char_created(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="SELECT CHAR_NAME, CHAR_CREATED FROM CHARACTER_TABLE ORDER BY CHAR_CREATED LIMIT 1;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        strcpy(game_data.name_last_char_created, (char*)sqlite3_column_text(stmt, 0));
        game_data.date_last_char_created=sqlite3_column_int(stmt,1);
    }

    sqlite3_finalize(stmt);
}

void add_char(struct client_node_type character){

    /** public function - see header */

    int i=0;
    int rc;
    sqlite3_stmt *stmt;
    int char_id=0;
    char * sErrMsg = 0;

    char char_tbl_sql[] ="INSERT INTO CHARACTER_TABLE(" \
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
        "FRAME," \
        "MAX_HEALTH," \
        "CURRENT_HEALTH," \
        "CHAR_CREATED" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, character.char_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, character.password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, CHAR_ALIVE); // char status
    sqlite3_bind_int(stmt, 4, character.active_chan);
    sqlite3_bind_int(stmt, 5, character.chan[0]);
    sqlite3_bind_int(stmt, 6, character.chan[1]);
    sqlite3_bind_int(stmt, 7, character.chan[2]);
    sqlite3_bind_int(stmt, 8, FALSE); // gm permission
    sqlite3_bind_int(stmt, 9, FALSE); // ig permission
    sqlite3_bind_int(stmt, 10, character.map_id);
    sqlite3_bind_int(stmt, 11, character.map_tile);
    sqlite3_bind_int(stmt, 12, character.char_type);
    sqlite3_bind_int(stmt, 13, character.skin_type);
    sqlite3_bind_int(stmt, 14, character.hair_type);
    sqlite3_bind_int(stmt, 15, character.shirt_type);
    sqlite3_bind_int(stmt, 16, character.pants_type);
    sqlite3_bind_int(stmt, 17, character.boots_type);
    sqlite3_bind_int(stmt, 18, character.head_type);
    sqlite3_bind_int(stmt, 19, SHIELD_NONE);
    sqlite3_bind_int(stmt, 20, WEAPON_NONE);
    sqlite3_bind_int(stmt, 21, CAPE_NONE);
    sqlite3_bind_int(stmt, 22, HELMET_NONE);
    sqlite3_bind_int(stmt, 23, character.frame);
    sqlite3_bind_int(stmt, 24, 0); // max health
    sqlite3_bind_int(stmt, 25, 0); // current health
    sqlite3_bind_int(stmt, 26, character.char_created);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_char: module database.c", char_tbl_sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    //get id of the char we just created and add iventory slots to database
    char_id=get_max_char_id();

    char inventory_tbl_sql[]="INSERT INTO INVENTORY_TABLE(CHAR_ID, SLOT) VALUES(?, ?)";

    sqlite3_prepare_v2(db, inventory_tbl_sql, -1, &stmt, NULL);

    //wrap in a transaction to speed up insertion
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);

    //create the char inventory record on the database
    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        sqlite3_bind_int(stmt, 1, char_id);
        sqlite3_bind_int(stmt, 2, i);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {

            log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_char: module database.c", char_tbl_sql, sqlite3_errmsg(db));
            exit(EXIT_FAILURE);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);

    sqlite3_finalize(stmt);
}

void add_item(int image_id, char *item_name, int bag_token, int harvestable, int emu, int interval,
              int exp,
              int food_value,
              int food_cooldown,
              int organic_nexus,
              int vegetal_nexus){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO ITEM_TABLE("  \
        "IMAGE_ID,"  \
        "ITEM_NAME," \
        "BAG_TOKEN," \
        "HARVESTABLE,"  \
        "EMU," \
        "INTERVAL," \
        "EXP," \
        "FOOD_VALUE," \
        "FOOD_COOLDOWN," \
        "ORGANIC_NEXUS," \
        "VEGETAL_NEXUS" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, image_id);
    sqlite3_bind_text(stmt, 2, item_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, bag_token);
    sqlite3_bind_int(stmt, 4, harvestable);
    sqlite3_bind_int(stmt, 5, emu);
    sqlite3_bind_int(stmt, 6, interval);
    sqlite3_bind_int(stmt, 7, exp);
    sqlite3_bind_int(stmt, 8, food_value);
    sqlite3_bind_int(stmt, 9, food_cooldown);
    sqlite3_bind_int(stmt, 10, organic_nexus);
    sqlite3_bind_int(stmt, 11, vegetal_nexus);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_item: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added item [%s] to ITEM_TABLE", item_name);
}

void add_threed_object(char *filename, int image_id){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO THREED_OBJECT_TABLE(FILE_NAME, INVENTORY_IMAGE_ID) VALUES(?, ?);";
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, image_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_threed_object: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
   }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added 3d object [%s] to THREED_OBJECT_TABLE", filename);
}

void add_map(int map_id, char *map_name, char *elm_file_name){

   /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO MAP_TABLE("  \
        "MAP_ID," \
        "MAP_NAME,"  \
        "ELM_FILE_NAME" \
        ") VALUES( ?, ?, ?);";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, map_id);
    sqlite3_bind_text(stmt, 2, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, elm_file_name, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_map: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
   }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added map [%s] to MAP_TABLE", map_name);
}

void add_channel(int channel_id, int owner_id, int channel_type, char *password, char *channel_name, char*channel_description){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO CHANNEL_TABLE("  \
        "CHANNEL_ID," \
        "OWNER_ID," \
        "TYPE," \
        "PASSWORD," \
        "NAME,"  \
        "DESCRIPTION" \
        ") VALUES( ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, channel_id);
    sqlite3_bind_int(stmt, 2, owner_id);
    sqlite3_bind_int(stmt, 3, channel_type);
    sqlite3_bind_text(stmt, 4, password, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, channel_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, channel_description, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_INITIALISATION, "Error %s executing '%s' in function add_channel: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added channel [%i] [%s] to CHANNEL_TABLE", channel_id, channel_name);
}

void add_race(int race_id, char *race_name, char *race_description,
              int initial_carry_capacity,
              float carry_capacity_multiplier,
              int initial_visprox,
              float visprox_multiplier,
              int initial_chatprox,
              float chatprox_multiplier,
              float initial_nightvis,
              float nightvis_multiplier
              ){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO RACE_TABLE("  \
        "RACE_ID," \
        "RACE_NAME," \
        "RACE_DESCRIPTION," \
        "INITIAL_EMU," \
        "EMU_MULTIPLIER,"  \
        "INITIAL_VISPROX," \
        "VISPROX_MULTIPLIER," \
        "INITIAL_CHATPROX," \
        "CHATPROX_MULTIPLIER," \
        "INITIAL_NIGHTVIS," \
        "NIGHTVIS_MULTIPLIER" \
        ") VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, race_id);
    sqlite3_bind_text(stmt, 2, race_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, race_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, initial_carry_capacity);
    sqlite3_bind_double(stmt, 5, carry_capacity_multiplier);
    sqlite3_bind_int(stmt, 6, initial_visprox);
    sqlite3_bind_double(stmt, 7, visprox_multiplier);
    sqlite3_bind_int(stmt, 8, initial_chatprox);
    sqlite3_bind_double(stmt, 9, chatprox_multiplier);
    sqlite3_bind_double(stmt, 10, initial_nightvis);
    sqlite3_bind_double(stmt, 11, nightvis_multiplier);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_race: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added race [%i] [%s] to RACE_TABLE", race_id, race_name);
}

void add_guild(int guild_id, char *guild_tag, char *guild_name, char *guild_description,
              int tag_colour,
              int log_on_colour,
              int log_off_colour,
              int chan_text_colour,
              int chan_id
              ){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[] ="INSERT INTO GUILD_TABLE("  \
        "GUILD_ID," \
        "GUILD_TAG," \
        "GUILD_NAME," \
        "GUILD_DESCRIPTION," \
        "TAG_COLOUR," \
        "LOGON_COLOUR,"  \
        "LOGOFF_COLOUR," \
        "CHAN_TEXT_COLOUR," \
        "CHAN_ID," \
        "DATE_CREATED"
        ") VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, guild_id);
    sqlite3_bind_text(stmt, 2, guild_tag, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, guild_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, guild_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, tag_colour);
    sqlite3_bind_int(stmt, 6, log_on_colour);
    sqlite3_bind_int(stmt, 7, log_off_colour);
    sqlite3_bind_int(stmt, 8, chan_text_colour);
    sqlite3_bind_int(stmt, 9, chan_id);
    sqlite3_bind_int(stmt, 10, time(NULL));

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_guild: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added guild [%i] [%s] to GUILD_TABLE", guild_id, guild_name);
}

void add_bag_type(int bag_type_id, int image_id, char *bag_description, int max_emu,
                  float u_split_modifier,
                  float o_split_modifier,
                  int invisible_time,
                  int visible_time){

 /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="INSERT INTO BAG_TYPE_TABLE("  \
        "BAG_TYPE_ID," \
        "IMAGE_ID," \
        "BAG_TYPE_DESCRIPTION," \
        "MAX_EMU,"  \
        "O_SPLIT_MODIFIER," \
        "U_SPLIT_MODIFIER," \
        "INVISIBLE_TIME," \
        "VISIBLE_TIME," \
        ") VALUES( ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, bag_type_id);
    sqlite3_bind_int(stmt, 2, image_id);
    sqlite3_bind_text(stmt, 3, bag_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, max_emu);
    sqlite3_bind_double(stmt, 5, u_split_modifier);
    sqlite3_bind_double(stmt, 6, o_split_modifier);
    sqlite3_bind_int(stmt, 7, invisible_time);
    sqlite3_bind_int(stmt, 8, visible_time);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_bag_type: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added bag type [%i] [%s] to BAG_TYPE_TABLE", bag_type_id, bag_description);
}

void add_bag_tool(int bag_tool_id, int image_id, char *description, int make_visible, int bag_lock_type, int bag_unlock_type,
                  int bag_arm_type,
                  int bag_disarm_type,
                  int bag_publicity_type,
                  int single_use,
                  int break_chance){

 /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="INSERT INTO BAG_TOOL_TABLE("  \
        "BAG_TOOL_ID," \
        "IMAGE_ID," \
        "DESCRIPTION," \
        "MAKE_VISIBLE,"  \
        "BAG_LOCK_TYPE," \
        "BAG_UNLOCK_TYPE," \
        "BAG_ARM_TYPE," \
        "BAG_DISARM_TYPE," \
        "BAG_PUBLICITY_TYPE," \
        "SINGLE_USE," \
        "BREAK_CHANCE," \
        ") VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ? , ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, bag_tool_id);
    sqlite3_bind_int(stmt, 2, image_id);
    sqlite3_bind_text(stmt, 3, description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, make_visible);
    sqlite3_bind_int(stmt, 5, bag_lock_type);
    sqlite3_bind_int(stmt, 6, bag_unlock_type);
    sqlite3_bind_int(stmt, 7, bag_arm_type);
    sqlite3_bind_int(stmt, 8, bag_disarm_type);
    sqlite3_bind_int(stmt, 9, bag_publicity_type);
    sqlite3_bind_int(stmt, 10, single_use);
    sqlite3_bind_int(stmt, 11, break_chance);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_bag_tool: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added bag tool [%i] [%s] to BAG_TOOL_TABLE", bag_tool_id, description);
}



void add_char_type(int char_type_id, char *char_type_name, int race_id, int sex_id){

 /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="INSERT INTO CHARACTER_TYPE_TABLE("  \
        "CHARACTER_TYPE_ID," \
        "CHARACTER_TYPE_NAME," \
        "RACE_ID," \
        "SEX_ID"  \
        ") VALUES( ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, char_type_id);
    sqlite3_bind_text(stmt, 2, char_type_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, race_id);
    sqlite3_bind_int(stmt, 4, sex_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function add_char_type: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "Added character type [%i] [%s] to CHARACTER_TYPE_TABLE", char_type_id, char_type_name);
}

int get_char_data_from_db(char *name){

    /** public function - see header */

    int slot=0;
    int rc;
    sqlite3_stmt *stmt;

    character.character_id=0; //set to zero as we use this to determine if the char exists

    char char_tbl_sql[]="SELECT * FROM CHARACTER_TABLE WHERE CHAR_NAME=?";

    sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);

    //zero the struct
    memset(&character, 0, sizeof(character));

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        character.character_id=sqlite3_column_int(stmt, 0);
        strcpy(character.char_name, (char*) sqlite3_column_text(stmt, 1));
        strcpy(character.password, (char*) sqlite3_column_text(stmt,2));
        character.char_status=sqlite3_column_int(stmt, 3);
        character.time_played=sqlite3_column_int(stmt, 4);
        character.active_chan=sqlite3_column_int(stmt, 5);
        character.chan[0]=sqlite3_column_int(stmt, 6);
        character.chan[1]=sqlite3_column_int(stmt, 7);
        character.chan[2]=sqlite3_column_int(stmt, 8);
        character.gm_permission=sqlite3_column_int(stmt, 9);
        character.ig_permission=sqlite3_column_int(stmt, 10);
        character.map_id=sqlite3_column_int(stmt, 11);
        character.map_tile=sqlite3_column_int(stmt, 12);
        character.guild_id=sqlite3_column_int(stmt, 13);
        character.char_type=sqlite3_column_int(stmt, 14);
        character.skin_type=sqlite3_column_int(stmt, 15);
        character.hair_type=sqlite3_column_int(stmt, 16);
        character.shirt_type=sqlite3_column_int(stmt, 17);
        character.pants_type=sqlite3_column_int(stmt, 18);
        character.boots_type=sqlite3_column_int(stmt, 19);
        character.head_type=sqlite3_column_int(stmt, 20);
        character.shield_type=sqlite3_column_int(stmt, 21);
        character.weapon_type=sqlite3_column_int(stmt, 22);
        character.cape_type=sqlite3_column_int(stmt, 23);
        character.helmet_type=sqlite3_column_int(stmt, 24);
        character.frame=sqlite3_column_int(stmt, 25);
        character.max_health=sqlite3_column_int(stmt, 26);
        character.current_health=sqlite3_column_int(stmt, 27);
        character.last_in_game=sqlite3_column_int(stmt,  28);
        character.char_created=sqlite3_column_int(stmt, 29);
        character.joined_guild=sqlite3_column_int(stmt, 30);
        character.physique=sqlite3_column_int(stmt, 31);
        character.vitality=sqlite3_column_int(stmt, 32);
        character.will=sqlite3_column_int(stmt, 33);
        character.coordination=sqlite3_column_int(stmt, 34);
        character.overall_exp=sqlite3_column_int(stmt, 35);
        character.harvest_exp=sqlite3_column_int(stmt, 36);
     }

    sqlite3_finalize(stmt);

    //get inventory
    char inventory_tbl_sql[]="SELECT SLOT, IMAGE_ID, AMOUNT FROM INVENTORY_TABLE WHERE CHAR_ID=?";

    sqlite3_prepare_v2(db, inventory_tbl_sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, character.character_id);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        slot=sqlite3_column_int(stmt, 0);
        character.client_inventory[slot].image_id=sqlite3_column_int(stmt, 1);
        character.client_inventory[slot].amount=sqlite3_column_int(stmt, 2);
    }

    sqlite3_finalize(stmt);

    if(character.character_id==0) return NOT_FOUND;

    return FOUND;
}

void create_database_table(char *sql){

    int rc;
    sqlite3_stmt *stmt;
    char table_name[80]="";
    int i;
    int str_len=strlen(sql);


    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing [%s] in function create_database_table: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);

    //extract table name from sql string
    for(i=13; sql[i]!='('; i++){

        table_name[i-13]=sql[i];

        if(i>=str_len) {

            log_event2(EVENT_ERROR, "unable to extract table name from sql string [%s] in function create_database_table: module database.c", sql);
            exit(EXIT_FAILURE);
        }
    }

    log_event2(EVENT_INITIALISATION, "Created table [%s]", table_name);
}

void load_3d_objects(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int i=0;

    char sql[]="SELECT * FROM THREED_OBJECT_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading threed objects...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        strcpy(threed_object[i].file_name, (char*)sqlite3_column_text(stmt, 1));
        threed_object[i].inventory_image_id=sqlite3_column_int(stmt,2);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", i, threed_object[i].file_name);

        i++;

        if(i==MAX_THREED_OBJECTS) {

            log_event2(EVENT_ERROR, "Maximum number of threed objects exceeded in function load_3d_objects: module database.c");
            exit(EXIT_FAILURE);
        }
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] threed objects were loaded\n", i);
}

void load_races(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int race_id=0;
    int i=0;

    char sql[]="SELECT * FROM RACE_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading races...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        race_id=sqlite3_column_int(stmt, 0);

        if(race_id>MAX_RACES) {

            log_event2(EVENT_ERROR, "race_id [%i] exceeds max rang [0 - %i] in function load_races: module database.c", race_id, MAX_RACES);
            exit(EXIT_FAILURE);
        }

        strcpy(race[race_id].race_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(race[race_id].race_description, (char*)sqlite3_column_text(stmt, 2));
        race[race_id].initial_carry_capacity=sqlite3_column_int(stmt, 3);
        race[race_id].carry_capacity_multiplier=sqlite3_column_double(stmt, 4);
        race[race_id].initial_visual_proximity=sqlite3_column_int(stmt, 5);
        race[race_id].visual_proximity_multiplier=sqlite3_column_double(stmt, 6);
        race[race_id].initial_chat_proximity=sqlite3_column_int(stmt, 7);
        race[race_id].chat_proximity_multiplier=sqlite3_column_double(stmt, 8);
        race[race_id].initial_night_vis=sqlite3_column_double(stmt, 9);
        race[race_id].night_vis_multiplier=sqlite3_column_double(stmt, 10);

        race[race_id].char_count=sqlite3_column_int(stmt, 5);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", race_id, race[race_id].race_name);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] races were loaded\n", i);
}

void load_character_types(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int character_type_id=0;
    int i=0;

    char sql[]="SELECT * FROM CHARACTER_TYPE_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading character types...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        character_type_id=sqlite3_column_int(stmt, 0);

        if(character_type_id>MAX_CHARACTER_TYPES) {

            log_event2(EVENT_ERROR, "character type id [%i] exceeds max rang [0 - %i] in function load_character_types: module database.c", character_type_id, MAX_CHARACTER_TYPES);
            exit(EXIT_FAILURE);
        }

        strcpy(character_type[character_type_id].character_type_name, (char*)sqlite3_column_text(stmt, 1));
        character_type[character_type_id].race_id=sqlite3_column_int(stmt, 2);
        character_type[character_type_id].sex_id=sqlite3_column_int(stmt, 3);
        character_type[character_type_id].char_count=sqlite3_column_int(stmt, 4);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", character_type_id, character_type[character_type_id].character_type_name);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] races were loaded\n", i);
}

void load_channels(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int i=0;
    int chan_id=0;

    char sql[]="SELECT * FROM CHANNEL_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading channels...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        chan_id=sqlite3_column_int(stmt, 0);

        if(i>MAX_CHANNELS) {

            log_event2(EVENT_ERROR, "chan_id [%i] exceeds range [0 - %i] in function load_channels: module database.c", chan_id, MAX_CHANNELS);

            exit(EXIT_FAILURE);
        }

        channels.channel[chan_id]->channel_id=chan_id;
        channels.channel[chan_id]->chan_type=sqlite3_column_int(stmt, 1);
        channels.channel[chan_id]->owner_id=sqlite3_column_int(stmt, 2);
        strcpy(channels.channel[chan_id]->password, (char*)sqlite3_column_text(stmt, 3));
        strcpy(channels.channel[chan_id]->channel_name, (char*)sqlite3_column_text(stmt, 4));
        strcpy(channels.channel[chan_id]->description, (char*)sqlite3_column_text(stmt, 5));

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", i, channels.channel[i]->channel_name);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] channels were loaded\n", i);
}

void load_items(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int id=0;
    int i=0;

    char sql[]="SELECT * FROM ITEM_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading items...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        id=sqlite3_column_int(stmt,0);

        if(id>MAX_ITEMS){

            log_event2(EVENT_ERROR, "item id [%i] exceeds range [0 - %i] in function load_items: module database.c", id, MAX_ITEMS);
            exit(EXIT_FAILURE);
        }

        strcpy(item[id].item_name, (char*)sqlite3_column_text(stmt, 1));
        item[id].harvestable=sqlite3_column_int(stmt,3);
        item[id].emu=sqlite3_column_int(stmt,4);
        item[id].interval=sqlite3_column_int(stmt,5);
        item[id].exp=sqlite3_column_int(stmt,6);
        item[id].food_value=sqlite3_column_int(stmt,7);
        item[id].food_cooldown=sqlite3_column_int(stmt,8);
        item[id].organic_nexus=sqlite3_column_int(stmt,9);
        item[id].vegetal_nexus=sqlite3_column_int(stmt,10);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", id, item[id].item_name);

        i++;
     }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] items were loaded\n", i);
}

void load_maps(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int map_id=0;
    int i=0;

    char sql[]="SELECT * FROM MAP_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading maps...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        map_id=sqlite3_column_int(stmt,0);

        //make sure map_id doesn't exceed the size of the map array
        if(map_id>MAX_MAPS){

            log_event2(EVENT_ERROR, "map id [%i] exceeds range [0 - %i] in function load_maps: module database.c", map_id, MAX_MAPS);
            exit(EXIT_FAILURE);
        }

        strcpy(maps.map[map_id]->map_name, (char*)sqlite3_column_text(stmt, 1));
        strcpy(maps.map[map_id]->elm_filename, (char*)sqlite3_column_text(stmt, 2));

        load_map(map_id);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", map_id, maps.map[map_id]->map_name);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] maps were loaded\n", i);
}

void load_guilds(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int guild_id=0;
    int i=0;

    char sql[]="SELECT * FROM GUILD_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading guilds...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        guild_id=sqlite3_column_int(stmt,0);

        //make sure guild_id doesn't exceed the size of the map array
        if(guild_id>MAX_GUILDS){

            log_event2(EVENT_ERROR, "guild id [%i] exceeds range [0 - %i] in function load_guilds: module database.c", guild_id, MAX_GUILDS);
            exit(EXIT_FAILURE);
        }

        strcpy(guilds.guild[guild_id]->guild_tag, (char*)sqlite3_column_text(stmt, 1));
        strcpy(guilds.guild[guild_id]->guild_name, (char*)sqlite3_column_text(stmt, 2));
        strcpy(guilds.guild[guild_id]->guild_description, (char*)sqlite3_column_text(stmt, 3));

        guilds.guild[guild_id]->tag_colour= sqlite3_column_int(stmt, 4);

        guilds.guild[guild_id]->log_on_notification_colour= sqlite3_column_int(stmt, 5);
        guilds.guild[guild_id]->log_off_notification_colour= sqlite3_column_int(stmt, 6);
        guilds.guild[guild_id]->guild_chan_text_colour= sqlite3_column_int(stmt, 7);
        guilds.guild[guild_id]->guild_chan_number= sqlite3_column_int(stmt, 8);
        guilds.guild[guild_id]->date_created= sqlite3_column_int(stmt, 9);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", guild_id, guilds.guild[guild_id]->guild_name);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] guilds were loaded\n", i);
}

void load_bag_types(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int bag_type_id=0;
    int i=0;

    char sql[]="SELECT * FROM BAG_TYPE_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading bag types...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        bag_type_id=sqlite3_column_int(stmt,0);

        //make sure bag_type_id doesn't exceed the size of the bag type array
        if(bag_type_id>MAX_BAG_TYPES){

            log_event2(EVENT_ERROR, "bag type token [%i] exceeds range [0 - %i] in function load_bag_types: module database.c", bag_type_id, MAX_BAG_TYPES);
            exit(EXIT_FAILURE);
        }
        bag_type_id=sqlite3_column_int(stmt, 0);

        bag_type[bag_type_id].image_id=sqlite3_column_int(stmt, 1);
        strcpy(bag_type[bag_type_id].bag_type_description, (char*)sqlite3_column_text(stmt, 2));
        bag_type[bag_type_id].max_emu=sqlite3_column_int(stmt, 3);
        bag_type[bag_type_id].u_split_modifier=sqlite3_column_double(stmt,4);
        bag_type[bag_type_id].o_split_modifier=sqlite3_column_double(stmt,5);
        bag_type[bag_type_id].invisible_time=sqlite3_column_int(stmt,6);
        bag_type[bag_type_id].visible_time=sqlite3_column_int(stmt,7);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", bag_type_id, bag_type[bag_type_id].bag_type_description);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] bag types were loaded\n", i);
}

void load_bag_tools(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int bag_tool_id=0;
    int i=0;

    char sql[]="SELECT * FROM BAG_TOOL_TABLE";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    log_event2(EVENT_INITIALISATION, "loading bag tools...");

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        bag_tool_id=sqlite3_column_int(stmt,0);

        //make sure bag_type_id doesn't exceed the size of the bag type array
        if(bag_tool_id>MAX_BAG_TOOLS){

            log_event2(EVENT_ERROR, "bag tool id [%i] exceeds range [0 - %i] in function load_bag_tools: module database.c", bag_tool_id, MAX_BAG_TYPES);
            exit(EXIT_FAILURE);
        }
        bag_tool_id=sqlite3_column_int(stmt, 0);

        bag_tool[bag_tool_id].image_id=sqlite3_column_int(stmt, 1);
        strcpy(bag_tool[bag_tool_id].description, (char*)sqlite3_column_text(stmt, 2));
        bag_tool[bag_tool_id].make_visible=sqlite3_column_int(stmt, 3);
        bag_tool[bag_tool_id].bag_lock_type=sqlite3_column_int(stmt,4);
        bag_tool[bag_tool_id].bag_unlock_type=sqlite3_column_int(stmt,5);
        bag_tool[bag_tool_id].bag_arm_type=sqlite3_column_int(stmt,6);
        bag_tool[bag_tool_id].bag_disarm_type=sqlite3_column_int(stmt,7);
        bag_tool[bag_tool_id].single_use=sqlite3_column_int(stmt,8);
        bag_tool[bag_tool_id].break_chance=sqlite3_column_int(stmt,9);

        log_event2(EVENT_INITIALISATION, "loaded [%i] [%s]", bag_tool_id, bag_tool[bag_tool_id].description);

        i++;
    }

    sqlite3_finalize(stmt);

    log_event2(EVENT_INITIALISATION, "[%i] bag tools were loaded\n", i);
}


void update_db_char_position(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    //char text_out[80]="";

    char sql[]="UPDATE CHARACTER_TABLE SET MAP_TILE=?, MAP_ID=? WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection].map_tile);
    sqlite3_bind_int(stmt, 2, clients.client[connection].map_id);
    sqlite3_bind_int(stmt, 3, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_pos: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_name(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET CHAR_NAME='?' WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, clients.client[connection].char_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_name: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
 }

void update_db_char_frame(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET FRAME=? WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection].frame);
    sqlite3_bind_int(stmt, 2, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_frame: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_stats(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET \
                    OVERALL_EXP=?, \
                    HARVEST_EXP=? \
                    WHERE CHAR_ID=?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection].overall_exp);
    sqlite3_bind_int(stmt, 2, clients.client[connection].harvest_exp);

    sqlite3_bind_int(stmt, 3, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_stats: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_last_in_game(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET LAST_IN_GAME=? WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, (int) clients.client[connection].last_in_game);
    sqlite3_bind_int(stmt, 2, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_char_last_in_game: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_time_played(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET TIME_PLAYED=? WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, (int) clients.client[connection].time_played);
    sqlite3_bind_int(stmt, 2, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_time_played: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_channels(int connection){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=?, CHAN_0=?, CHAN_1=?, CHAN_2=? WHERE CHAR_ID=?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection].active_chan);
    sqlite3_bind_int(stmt, 2, clients.client[connection].chan[0]);
    sqlite3_bind_int(stmt, 3, clients.client[connection].chan[1]);
    sqlite3_bind_int(stmt, 4, clients.client[connection].chan[2]);
    sqlite3_bind_int(stmt, 5, clients.client[connection].character_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_channels: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}

void update_db_char_inventory(int connection){

    int i=0;
    int rc;
    sqlite3_stmt *stmt;
    char * sErrMsg = 0;

    char sql[]="UPDATE INVENTORY_TABLE SET IMAGE_ID=?, AMOUNT=? WHERE CHAR_ID=? AND SLOT=?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        sqlite3_bind_int(stmt, 1, clients.client[connection].client_inventory[i].image_id);
        sqlite3_bind_int(stmt, 2, clients.client[connection].client_inventory[i].amount);
        sqlite3_bind_int(stmt, 3, clients.client[connection].character_id);
        sqlite3_bind_int(stmt, 4, i);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {

            log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_inventory: module database.c", sql, sqlite3_errmsg(db));
            exit(EXIT_FAILURE);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    sqlite3_finalize(stmt);
}

void update_db_char_slot(int connection, int slot){

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE INVENTORY_TABLE SET IMAGE_ID=?, AMOUNT=? WHERE CHAR_ID=? AND SLOT=?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, clients.client[connection].client_inventory[slot].image_id);
    sqlite3_bind_int(stmt, 2, clients.client[connection].client_inventory[slot].amount);
    sqlite3_bind_int(stmt, 3, clients.client[connection].character_id);
    sqlite3_bind_int(stmt, 4, slot);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

            log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_char_inventory: module database.c", sql, sqlite3_errmsg(db));
            exit(EXIT_FAILURE);
        }

    sqlite3_finalize(stmt);
}

void update_db_race_count(int race_id){

    int rc;
    sqlite3_stmt *stmt;

    char sql[]="UPDATE RACE_TABLE SET CHAR_COUNT=? WHERE RACE_ID=?";

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, race[race_id].char_count);
    sqlite3_bind_int(stmt, 2, race_id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {

        log_event2(EVENT_ERROR, "Error %s executing '%s' in function update_db_race_count: module database.c", sql, sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }

    sqlite3_finalize(stmt);
}
