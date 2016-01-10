/******************************************************************************************************************
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
*******************************************************************************************************************/

#include <stdio.h> //support for snprintf
#include <string.h> //support for memset strcpy

#include "database_functions.h"
#include "../clients.h"
#include "../global.h"
#include "../characters.h"
#include "../logging.h"
#include "../game_data.h"
#include "../server_start_stop.h"
#include "../string_functions.h"


bool get_db_char_exists(int char_id){

    /** public function - see header */

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT count(*) FROM CHARACTER_TABLE WHERE CHAR_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_int(stmt, 1, char_id);

    int char_count=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

         char_count=sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, sql);
    }

    // if count of char_id is greater than 1 then we have a duplicate and need to close the server
    if(char_count>1){

        log_event(EVENT_ERROR, "duplicate char id [%i] in database", char_id);
        stop_server();
    }

    if(char_count==0) return false;

    return true;
}


bool get_db_char_data(const char *char_name, int char_id){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //create upper case copy of char name
    char char_name_uc[MAX_CHAR_NAME_LEN]="";
    strcpy(char_name_uc, char_name);
    str_conv_upper(char_name_uc);

    char char_tbl_sql[MAX_SQL_LEN]="";

    if(strcmp(char_name, "")!=0 || char_id==-1){

        snprintf(char_tbl_sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE UPPER(CHAR_NAME)='%s'", char_name_uc);
    }
    else {

        snprintf(char_tbl_sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_ID=%i", char_id);
    }

    int rc=sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //zero the struct (critical that we set character_id element to zero as we use this to flag if char has been found
    memset(&character, 0, sizeof(character));

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        character.character_id=sqlite3_column_int(stmt, 0);
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
        character.guild_rank=sqlite3_column_int(stmt, 13);
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
        //character.last_in_game=sqlite3_column_int(stmt,  28);
        character.char_created=sqlite3_column_int(stmt, 29);
        character.joined_guild=sqlite3_column_int(stmt, 30);
        character.physique_pp=sqlite3_column_int(stmt, 31);
        character.vitality_pp=sqlite3_column_int(stmt, 32);
        character.will_pp=sqlite3_column_int(stmt, 33);
        character.coordination_pp=sqlite3_column_int(stmt, 34);
        character.overall_exp=sqlite3_column_int(stmt, 35);
        character.harvest_exp=sqlite3_column_int(stmt, 36);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //get inventory
    char inventory_tbl_sql[MAX_SQL_LEN]="SELECT SLOT, IMAGE_ID, AMOUNT FROM INVENTORY_TABLE WHERE CHAR_ID=?";

    rc=sqlite3_prepare_v2(db, inventory_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    sqlite3_bind_int(stmt, 1, character.character_id);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int slot=sqlite3_column_int(stmt, 0);
        character.inventory[slot].object_id=sqlite3_column_int(stmt, 1);
        character.inventory[slot].amount=sqlite3_column_int(stmt, 2);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    if(character.character_id==0) return false;

    return true;
}


int get_db_char_count(){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT count(*) FROM CHARACTER_TABLE";

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    int max_id=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        max_id=sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_DONE) {

        log_event(EVENT_ERROR, "sqlite3_step failed in function %s:  module %s: line %i", rc, sql, __func__, __FILE__, __LINE__);
        stop_server();
    }

    sqlite3_finalize(stmt);
    if(rc!=SQLITE_DONE){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return max_id;
}


int add_db_char_data(struct client_node_type character){

    /** public function - see header **/

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char char_tbl_sql[MAX_SQL_LEN]="";
    snprintf(char_tbl_sql, MAX_SQL_LEN,
        "INSERT INTO CHARACTER_TABLE(" \
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
        "GUILD_ID," \
        "GUILD_RANK," \
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
        "CHAR_CREATED," \
        "JOINED_GUILD" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    int rc=sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    sqlite3_bind_text(stmt, 1, character.char_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, character.password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, CHAR_ALIVE);
    sqlite3_bind_int(stmt, 4, character.active_chan);
    sqlite3_bind_int(stmt, 5, character.chan[0]);
    sqlite3_bind_int(stmt, 6, character.chan[1]);
    sqlite3_bind_int(stmt, 7, character.chan[2]);
    sqlite3_bind_int(stmt, 8, 0); // gm permission
    sqlite3_bind_int(stmt, 9, 0); // ig permission
    sqlite3_bind_int(stmt, 10, character.map_id);
    sqlite3_bind_int(stmt, 11, character.map_tile);
    sqlite3_bind_int(stmt, 12, character.guild_id);
    sqlite3_bind_int(stmt, 13, character.guild_rank);
    sqlite3_bind_int(stmt, 14, character.char_type);
    sqlite3_bind_int(stmt, 15, character.skin_type);
    sqlite3_bind_int(stmt, 16, character.hair_type);
    sqlite3_bind_int(stmt, 17, character.shirt_type);
    sqlite3_bind_int(stmt, 18, character.pants_type);
    sqlite3_bind_int(stmt, 19, character.boots_type);
    sqlite3_bind_int(stmt, 20, character.head_type);
    sqlite3_bind_int(stmt, 21, SHIELD_NONE);
    sqlite3_bind_int(stmt, 22, WEAPON_NONE);
    sqlite3_bind_int(stmt, 23, CAPE_NONE);
    sqlite3_bind_int(stmt, 24, HELMET_NONE);
    sqlite3_bind_int(stmt, 25, character.frame);
    sqlite3_bind_int(stmt, 26, 0); // max health
    sqlite3_bind_int(stmt, 27, 0); // current health
    sqlite3_bind_int(stmt, 28, (int)character.char_created);
    sqlite3_bind_int(stmt, 29, (int)character.joined_guild);

    //process sql statement
    rc=sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //find the id of the new entry
    strcpy(char_tbl_sql, "SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE");

    //prepare the sql statement
    rc=sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //process the sql statement
    int id=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        id=sqlite3_column_int(stmt, 0);
    }

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    char inventory_tbl_sql[MAX_SQL_LEN]="";
    snprintf(inventory_tbl_sql, MAX_SQL_LEN, "INSERT INTO INVENTORY_TABLE(CHAR_ID, SLOT) VALUES(?, ?)");

    rc=sqlite3_prepare_v2(db, inventory_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    //wrap in a transaction to speed up insertion
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    //create the char inventory record on the database
    for(int i=0; i<MAX_INVENTORY_SLOTS; i++){

        sqlite3_bind_int(stmt, 1, id);
        sqlite3_bind_int(stmt, 2, i);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {

            log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
        }

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    log_event(EVENT_SESSION, "char [%s] added to database", character.char_name);

    return id;
}

void get_db_last_char_created(){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT CHAR_NAME, CHAR_CREATED FROM CHARACTER_TABLE ORDER BY CHAR_CREATED DESC LIMIT 1;");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        strcpy(game_data.name_last_char_created, (char*)sqlite3_column_text(stmt, 0));
        game_data.date_last_char_created=sqlite3_column_int(stmt,1);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }
}

void load_char_race_stats(){

    //TODO (themuntdregger#1#): add char race stats
}

void load_char_gender_stats(){

    //TODO (themuntdregger#1#): add char gender stats
}
