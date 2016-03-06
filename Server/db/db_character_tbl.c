/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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
#include <stdlib.h> //support for EXIT_FAILURE macro and exit function

#include "database_functions.h"
#include "../clients.h"
#include "../characters.h"
#include "../logging.h"
#include "../game_data.h"
#include "../server_start_stop.h"
#include "../string_functions.h"
#include "../server_protocol_functions.h"

bool get_db_char_exists(int char_id){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT count(*) FROM CHARACTER_TABLE WHERE CHAR_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    sqlite3_bind_int(stmt, 1, char_id);

    int char_count=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

         char_count=sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize", GET_CALL_INFO, rc, sql);
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

    //check database is open
    check_db_open(GET_CALL_INFO);

    sqlite3_stmt *stmt;

    //create upper case copy of char name
    char char_name_uc[MAX_CHAR_NAME_LEN]="";
    strcpy(char_name_uc, char_name);
    str_conv_upper(char_name_uc);

    char sql[MAX_SQL_LEN]="";

    if(strcmp(char_name, "")!=0 || char_id==-1){

        snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE UPPER(CHAR_NAME)='%s'", char_name_uc);
    }
    else {

        snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE CHAR_ID=%i", char_id);
    }

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //zero the struct (critical that we set character_id element to zero as we use this to flag if char has been found
    memset(&character, 0, sizeof(character));

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        character.character_id=sqlite3_column_int(stmt, 0);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(character.char_name, (char*) sqlite3_column_text(stmt, 1));
        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(character.password, (char*) sqlite3_column_text(stmt,2));

        character.char_status=sqlite3_column_int(stmt, 3);
        character.active_chan=sqlite3_column_int(stmt, 4);
        character.chan[0]=sqlite3_column_int(stmt, 5);
        character.chan[1]=sqlite3_column_int(stmt, 6);
        character.chan[2]=sqlite3_column_int(stmt, 7);
        character.player_type=sqlite3_column_int(stmt, 8);
        character.unused=sqlite3_column_int(stmt, 9);
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
        character.harvest_lvl=sqlite3_column_int(stmt, 37);
        memcpy(character.inventory, (unsigned char*)sqlite3_column_blob(stmt, 38), sizeof(character.inventory));
    }

    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize", GET_CALL_INFO, rc, sql);
    }

    if(character.character_id==0) return false;

    return true;
}


int get_db_char_count(){

    /** public function - see header */

    //check database is open
    check_db_open(GET_CALL_INFO);

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT count(*) FROM CHARACTER_TABLE";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    int max_id=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        max_id=sqlite3_column_int(stmt, 0);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    return max_id;
}


int add_db_char_data(struct client_node_type character){

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="INSERT INTO CHARACTER_TABLE(" \
        "CHAR_NAME," \
        "PASSWORD," \
        "CHAR_STATUS," \
        "ACTIVE_CHAN," \
        "CHAN_0," \
        "CHAN_1," \
        "CHAN_2," \
        "PLAYER_TYPE," \
        "UNUSED," \
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
        "JOINED_GUILD," \
        "INVENTORY" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    sqlite3_bind_text(stmt, 1, character.char_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, character.password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, CHAR_ALIVE);
    sqlite3_bind_int(stmt, 4, character.active_chan);
    sqlite3_bind_int(stmt, 5, character.chan[0]);
    sqlite3_bind_int(stmt, 6, character.chan[1]);
    sqlite3_bind_int(stmt, 7, character.chan[2]);
    sqlite3_bind_int(stmt, 8, character.player_type);
    sqlite3_bind_int(stmt, 9, character.unused);
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
    sqlite3_bind_blob(stmt, 30, character.inventory, sizeof(character.inventory), NULL);

    //process sql statement
    rc=sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    //find the id of the new entry
    strcpy(sql, "SELECT MAX(CHAR_ID) FROM CHARACTER_TABLE");

    //prepare the sql statement
    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //process the sql statement
    int id=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        id=sqlite3_column_int(stmt, 0);
    }

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    log_event(EVENT_SESSION, "char [%s] added to database", character.char_name);

    return id;
}


void get_db_last_char_created(){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="SELECT CHAR_NAME, CHAR_CREATED FROM CHARACTER_TABLE ORDER BY CHAR_CREATED DESC LIMIT 1";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        strcpy(game_data.name_last_char_created, (char*)sqlite3_column_text(stmt, 0));
        game_data.date_last_char_created=sqlite3_column_int(stmt,1);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }
}


void load_char_race_stats(){

    //TODO (themuntdregger#1#): add char race stats
}


void load_char_gender_stats(){

    //TODO (themuntdregger#1#): add char gender stats
}


void load_npc_characters(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading npc_characters...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="SELECT * FROM CHARACTER_TABLE WHERE PLAYER_TYPE=?";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    sqlite3_bind_int(stmt, 0, NPC);

    //read the sql query result into the client array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        if(sqlite3_column_int(stmt, 8)==NPC){

            int actor_node=get_next_free_actor_node();

            clients.client[actor_node].character_id=sqlite3_column_int(stmt, 0);
            strcpy(clients.client[actor_node].char_name, (char*) sqlite3_column_text(stmt, 1));
            clients.client[actor_node].char_status=sqlite3_column_int(stmt, 3);
            clients.client[actor_node].active_chan=sqlite3_column_int(stmt, 4);
            clients.client[actor_node].player_type=sqlite3_column_int(stmt, 8);
            clients.client[actor_node].unused=sqlite3_column_int(stmt, 9);
            clients.client[actor_node].map_id=sqlite3_column_int(stmt, 10);
            clients.client[actor_node].map_tile=sqlite3_column_int(stmt, 11);
            clients.client[actor_node].char_type=sqlite3_column_int(stmt, 14);
            clients.client[actor_node].skin_type=sqlite3_column_int(stmt, 15);
            clients.client[actor_node].hair_type=sqlite3_column_int(stmt, 16);
            clients.client[actor_node].shirt_type=sqlite3_column_int(stmt, 17);
            clients.client[actor_node].pants_type=sqlite3_column_int(stmt, 18);
            clients.client[actor_node].boots_type=sqlite3_column_int(stmt, 19);
            clients.client[actor_node].head_type=sqlite3_column_int(stmt, 20);
            clients.client[actor_node].shield_type=sqlite3_column_int(stmt, 21);
            clients.client[actor_node].weapon_type=sqlite3_column_int(stmt, 22);
            clients.client[actor_node].cape_type=sqlite3_column_int(stmt, 23);
            clients.client[actor_node].helmet_type=sqlite3_column_int(stmt, 24);
            clients.client[actor_node].frame=sqlite3_column_int(stmt, 25);
            clients.client[actor_node].char_created=sqlite3_column_int(stmt, 29);

            log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", i, clients.client[actor_node].char_name);

            i++;
        }
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }
}


void db_update_inventory(int actor_node){

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("CHARACTER_TABLE", GET_CALL_INFO);

    //prepare sql statement
    char sql[MAX_SQL_LEN]="UPDATE CHARACTER_TABLE SET INVENTORY=? WHERE CHAR_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    sqlite3_bind_blob(stmt, 1, clients.client[actor_node].inventory, sizeof(clients.client[actor_node].inventory), NULL);
    sqlite3_bind_int(stmt, 2, clients.client[actor_node].character_id);

    //process the sql statement
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //do nothing
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }
}


void batch_add_characters(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "character load file [%s] not found", file_name);
        exit(EXIT_FAILURE);
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding characters specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding characters specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[13][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        //clear struct
        memset(&character, 0, sizeof(character));

        //add data
        strcpy(character.char_name, output[0]);
        strcpy(character.password, output[1]);
        character.char_status=atoi(output[2]);
        character.player_type=atoi(output[3]);
        character.guild_id=atoi(output[4]);
        character.guild_rank=atoi(output[5]);
        character.char_type=atoi(output[6]);
        character.skin_type=atoi(output[7]);
        character.hair_type=atoi(output[8]);
        character.shirt_type=atoi(output[9]);
        character.pants_type=atoi(output[10]);
        character.boots_type=atoi(output[11]);
        character.head_type=atoi(output[12]);
        character.shield_type=SHIELD_NONE;
        character.weapon_type=WEAPON_NONE;
        character.cape_type=CAPE_NONE;
        character.helmet_type=HELMET_NONE;
        character.frame=frame_stand;
        character.max_health=100;
        character.current_health=100;
        character.char_created=time(NULL);
        character.joined_guild=time(NULL);
        character.map_id=game_data.beam_map_id;
        character.map_tile=game_data.beam_map_tile;

        add_new_char_chat_channels();

        add_db_char_data(character);
    }

    fclose(file);
}

