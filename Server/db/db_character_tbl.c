/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

int get_db_char_data(char *char_name){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char char_tbl_sql[MAX_SQL_LEN]="";
    snprintf(char_tbl_sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_NAME=?");

    sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    sqlite3_bind_text(stmt, 1, char_name, -1, SQLITE_STATIC);

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
        //character.last_in_game=sqlite3_column_int(stmt,  27);
        character.char_created=sqlite3_column_int(stmt, 28);
        character.joined_guild=sqlite3_column_int(stmt, 29);
        character.physique_pp=sqlite3_column_int(stmt, 30);
        character.vitality_pp=sqlite3_column_int(stmt, 31);
        character.will_pp=sqlite3_column_int(stmt, 32);
        character.coordination_pp=sqlite3_column_int(stmt, 33);
        character.overall_exp=sqlite3_column_int(stmt, 34);
        character.harvest_exp=sqlite3_column_int(stmt, 35);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //get inventory
    char inventory_tbl_sql[MAX_SQL_LEN]="";
    snprintf(inventory_tbl_sql, MAX_SQL_LEN, "SELECT SLOT, IMAGE_ID, AMOUNT FROM INVENTORY_TABLE WHERE CHAR_ID=?");

    sqlite3_prepare_v2(db, inventory_tbl_sql, -1, &stmt, NULL);
    if(rc!=SQLITE_DONE){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    sqlite3_bind_int(stmt, 1, character.character_id);

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int slot=sqlite3_column_int(stmt, 0);
        character.client_inventory[slot].image_id=sqlite3_column_int(stmt, 1);
        character.client_inventory[slot].amount=sqlite3_column_int(stmt, 2);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    sqlite3_finalize(stmt);
    if(rc!=SQLITE_DONE){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, inventory_tbl_sql);
    }

    if(character.character_id==0) return NOT_FOUND;

    return FOUND;
}


int get_db_max_char_id(){

    /** RESULT  : Finds the highest char_id in the Character Table of the database

        RETURNS : the char_id if char_name is found. else 0.

        PURPOSE : Used by add_db_char_data
    **/

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE;");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    int max_id=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        max_id=sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return max_id;
}


int get_db_char_count(){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM CHARACTER_TABLE");

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

    //don't use process_sql as we have to add both the char and inventory in this function

    int rc=0;
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
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    rc=sqlite3_prepare_v2(db, char_tbl_sql, -1, &stmt, NULL);
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

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, char_tbl_sql);
    }

    //get id of the char we just created and add inventory slots to database
    int char_id=get_db_max_char_id();

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
    int i=0;

    for(i=0; i<MAX_INVENTORY_SLOTS; i++){

        sqlite3_bind_int(stmt, 1, char_id);
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

    //As the id of the new record is created automatically by sqllite, we need to find out what it is and
    //return it so that we can link an inventory table entry for the char.
    return get_db_max_char_id();
}

void get_db_last_char_created(){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT CHAR_NAME, CHAR_CREATED FROM CHARACTER_TABLE ORDER BY CHAR_CREATED LIMIT 1;");

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
