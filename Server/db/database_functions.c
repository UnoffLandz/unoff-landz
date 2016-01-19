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

#include <stdio.h> //supports snprintf
#include <string.h> //supports strlen
#include <stdlib.h> // testing only

#include "database_functions.h"
#include "db_character_tbl.h"
#include "db_map_tbl.h"
#include "db_character_type_tbl.h"
#include "db_character_race_tbl.h"
#include "db_character_inventory_tbl.h"
#include "db_gender_tbl.h"
#include "db_chat_channel_tbl.h"
#include "db_attribute_tbl.h"
#include "db_game_data_tbl.h"
#include "db_season_tbl.h"
#include "db_object_tbl.h"
#include "db_e3d_tbl.h"
#include "db_map_object_tbl.h"
#include "db_guild_tbl.h"
#include "../server_start_stop.h"
#include "../attributes.h"
#include "../chat.h"
#include "../logging.h"
#include "../file_functions.h"
#include "../colour.h"
#include "../characters.h"
#include "../server_messaging.h"
#include "../game_data.h"
#include "../guilds.h"
#include "../string_functions.h"
#include "../gender.h"
#include "../maps.h"
#include "../objects.h"
#include "../e3d.h"

// declare the database handle as global
sqlite3 *db;


static int prepare_query(const char *sql, sqlite3_stmt **stmt, const char *_func, int line){

    int rc=sqlite3_prepare_v2(db, sql, -1, stmt, NULL);

    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", _func, __FILE__, line, rc, sql);
        return -1;
    }

    return 0;
}


void open_database(const char *db_filename){

   /** public function - see header **/

    if(file_exists(db_filename)==false){

        printf("database file [%s] not found\n", db_filename);
        log_event(EVENT_ERROR, "database file [%s] not found", db_filename);
        stop_server();
        return;
    }

    int rc = sqlite3_open(db_filename, &db);
    if( rc !=SQLITE_OK ){

        log_sqlite_error("sqlite3_open", __func__ , __FILE__, __LINE__, rc, "");
    }

    printf("database file [%s] opened\n", db_filename);
    log_event(EVENT_INITIALISATION, "database file [%s] opened", db_filename);
}


void close_database(){

    int rc=sqlite3_close(db);
    if(rc != SQLITE_OK ){

        //don't use log_sqlite_error function as this already contains a call to close_database
        //and therefore creates an infinite loop
        log_event(EVENT_ERROR, "sqlite3_close failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error code %i", rc);
    }

    printf("database file closed\n");
    log_event(EVENT_SESSION, "database file closed");
}


bool table_exists(const char *table_name) {

    sqlite3_stmt *stmt;
    int count=0;
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE tbl_name='%s'", table_name);

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        count=sqlite3_column_int(stmt, 0);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    if(count==0) return false;

    return true;
}

/*
int column_exists(const char *table, const char *column) {

    sqlite3_stmt *stmt;
    char sql[MAX_SQL_LEN]="";
    int name_column_idx=-1;

    snprintf(sql, MAX_SQL_LEN, "PRAGMA table_info('%s')", table);

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    for (int i=0; i<sqlite3_column_count(stmt); i++) {

        if(strcmp("name", sqlite3_column_name(stmt,i))==0) {
            name_column_idx = i;
            break;
        }
    }

    if(name_column_idx==-1) {
        sqlite3_finalize(stmt);
        return 0; //TODO(themuntdregger#1#): log information about strange table_info layout ?
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        if(0==strcmp(column,(const char *)sqlite3_column_text(stmt, name_column_idx))) {
            sqlite3_finalize(stmt);
            return 1;
        }
    }
    sqlite3_finalize(stmt);
    return 0;
}
*/

int database_table_count(){

    /** public function - see header **/

    int rc;
    sqlite3_stmt *stmt;
    int table_count=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE type='table';");

    if(-1==prepare_query(sql, &stmt, __func__, __LINE__))
        return 0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        table_count=sqlite3_column_int(stmt, 0);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    return table_count;
}


void create_database_table(char *sql){

    /** RESULT  : creates a database table

       RETURNS  : void

       PURPOSE  : used by function create_new_database

       NOTES    :
    **/

    int rc;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql, &stmt, __func__, __LINE__))
        return;

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize", __func__, __FILE__, __LINE__, rc, sql);
    }

    //extract table name from sql string
    char table_name[80]="";
    size_t str_len=strlen(sql);

    for(size_t i=13; sql[i]!='('; i++){

        table_name[i-13]=sql[i];

        if(i>=str_len) {

            log_event(EVENT_ERROR, "unable to extract table name from sql string [%s] in function %s: module %s: line %i", sql, __func__, __FILE__, __LINE__);
            stop_server();
        }
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);

    printf("created table [%s]\n", table_name);
}


void process_sql(const char *sql_str){

    /** public function - see header **/

    int rc=0;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql_str, &stmt, __func__, __LINE__))
        return;

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }

    rc=sqlite3_finalize(stmt);
    if(rc != SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }
}


void create_database(const char *db_filename){

    /** public function - see header **/

    //create logical divider in log file
    log_text(EVENT_INITIALISATION, "\nCreating database tables...\n");

    //create database tables
    create_database_table(CHARACTER_TABLE_SQL);
    create_database_table(INVENTORY_TABLE_SQL);
    create_database_table(GENDER_TABLE_SQL);
    create_database_table(E3D_TABLE_SQL);
    create_database_table(OBJECT_TABLE_SQL);
    create_database_table(MAP_TABLE_SQL);
    create_database_table(CHANNEL_TABLE_SQL);
    create_database_table(RACE_TABLE_SQL);
    create_database_table(CHARACTER_TYPE_TABLE_SQL);
    create_database_table(ATTRIBUTE_TABLE_SQL);
    create_database_table(GAME_DATA_TABLE_SQL);
    create_database_table(SEASON_TABLE_SQL);
    create_database_table(MAP_OBJECT_TABLE_SQL);
    create_database_table(GUILD_TABLE_SQL);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");

    add_db_game_data(1, 27225, 1, 27225, 360, REQUIRED_DATABASE_VERSION);

    add_db_channel(1, 0, CHAN_PERMANENT, "", "Main Channel", "A channel for chatting", 1);

    add_db_race(1, "Human", "tall");
    add_db_race(2, "Dwarf", "short");
    add_db_race(3, "Elf", "short");
    add_db_race(4, "Gnome", "short");
    add_db_race(5, "Orchan", "tall");
    add_db_race(6, "Dragoni", "tall");

    add_db_gender(1, "Male");
    add_db_gender(2, "Female");

    add_db_char_type(0, 1, 2);
    add_db_char_type(1, 1, 1);
    add_db_char_type(2, 3, 2);
    add_db_char_type(3, 3, 1);
    add_db_char_type(4, 2, 2);
    add_db_char_type(5, 2, 1);
    add_db_char_type(37, 4, 2);
    add_db_char_type(38, 4, 1);
    add_db_char_type(39, 5, 2);
    add_db_char_type(40, 5, 1);
    add_db_char_type(41, 6, 2);
    add_db_char_type(42, 6, 1);

    add_db_season(0, "Winter", "A Winter season", 0, 90);
    add_db_season(1, "Alddra", "A Spring season", 90, 180);
    add_db_season(2, "Kiwi", "An Summer season", 180, 270);
    add_db_season(3, "Butler", "An Autumn season", 270, 360);

    int attribute_id=0;
    int attribute_value[50];

    // create day vision attribute
    for(int race_id=1; race_id<=6; race_id++){

        float i=10.0f;

        for(int pick_points=0; pick_points<=50; pick_points++){

            attribute_value[pick_points]=(int)i;
            i=i + 0.20f;
         }

        add_db_attribute(race_id, ATTR_DAY_VISION, attribute_value);
        attribute_id++;
    }

    //create night vision attribute
    for(int race_id=1; race_id<=6; race_id++){

        float i=10.0f;

        for(int pick_points=0; pick_points<=50; pick_points++){

            attribute_value[pick_points]=(int)i;
            i=i+18.0f;
        }

        add_db_attribute(race_id, ATTR_NIGHT_VISION, attribute_value);
        attribute_id++;
    }

    //create carry capacity attribute
    for(int race_id=1; race_id<=MAX_RACES; race_id++){

        float i=10.0f;

        for(int pick_points=0; pick_points<=50; pick_points++){

            attribute_value[pick_points]=(int)i;
            i=i + 0.20f;
        }

        add_db_attribute(race_id, ATTR_CARRY_CAPACITY, attribute_value);
        attribute_id++;
    }

    //add items in this order otherwise map object table will be corrupted
    batch_add_objects(OBJECT_FILE);
    batch_add_e3ds(E3D_FILE);
    batch_add_maps(MAP_FILE);//also adds map objects

    // TODO (themuntdregger#1#): change add_db_guild so that it works in a similar way to add_db_map
    add_db_guild("Operators", "OPS", c_grey3, "The server operators guild", PERMISSION_3, GUILD_ACTIVE);

    //load game data so that starting map and tile can be found
    load_db_game_data();

    //clear the character struct
    memset(&character, 0, sizeof(character));

    //create the master character
    strcpy(character.char_name, "Player");
    strcpy(character.password, "test");
    character.char_created=time(NULL);
    character.character_id=1;
    character.char_status=CHAR_ALIVE;
    character.skin_type=1;
    character.hair_type=1;
    character.shirt_type=1;
    character.pants_type=1;
    character.boots_type=1;
    character.char_type=1;
    character.head_type=1;
    character.frame=frame_stand;
    character.char_created=time(NULL);
    character.map_id=game_data.beam_map_id;
    character.map_tile=game_data.beam_map_tile;
    character.guild_id=1;
    character.joined_guild=time(NULL);
    character.guild_rank=20;

    //set starting channels
    int j=0;
    for(int i=0; i<MAX_CHANNELS; i++){

        if(channel[i].new_chars==1){

            if(j<MAX_CHAN_SLOTS){

                if(j==0) character.active_chan=i-CHAT_CHANNEL_0;
                character.chan[j]=i;
            }
        }
    }

    add_db_char_data(character);
    printf("Player [master_character][test] added successfully\n");

    printf("Database [%s] created\n", db_filename);
}
