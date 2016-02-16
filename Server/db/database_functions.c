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
#include "db_skill_tbl.h"
#include "db_upgrade.h"
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
#include "../character_type.h"
#include "../season.h"
#include "../character_skill.h"

sqlite3 *db; // declare the database handle as global

void check_db_open(const char *module, const char *func, const int line){

    /** public function - see header **/

    if(!db){

        log_event(EVENT_ERROR, "database not open in function %s: module %s: line %i", func, module, line);
        fprintf(stderr, "database not open in function %s: module %s: line %i", func, module, line);

        exit(EXIT_FAILURE);
    }
}


void check_db_closed(const char *module, const char *func, const int line){

    /** public function - see header **/

    if(db){

        log_event(EVENT_ERROR, "database already open in function %s: module %s: line %i", func, module, line);
        fprintf(stderr, "database already open in function %s: module %s: line %i", func, module, line);

        exit(EXIT_FAILURE);
    }
}


void create_empty_database_file (const char *db_filename){

    /** public function - see header **/

    //check database is closed
    check_db_closed(GET_CALL_INFO);

    //statement creates an empty database file if none exists
    int rc = sqlite3_open(db_filename, &db);
    if(rc!=SQLITE_OK ){

        fprintf(stderr, "unable to create sqlite file [%s]\n", db_filename);
        log_event(EVENT_ERROR, "unable to create sqlite file [%s]", db_filename);
    }
}


void open_database(const char *db_filename){

    /** public function - see header **/

    //check database is closed
    check_db_closed(GET_CALL_INFO);

    //make sure we have an existing file to open, otherwise warn that no file exists
    if(file_exists(db_filename)==false){

        fprintf(stderr, "database file [%s] not found\n", db_filename);
        log_event(EVENT_ERROR, "database file [%s] not found", db_filename);

        return;
    }

    //open the database
    int rc = sqlite3_open(db_filename, &db);
    if( rc !=SQLITE_OK ){

        log_sqlite_error("sqlite3_open", __func__ , __FILE__, __LINE__, rc, "");
        //server stopped automatically
    }

    fprintf(stderr, "database file [%s] opened\n", db_filename);
    log_event(EVENT_INITIALISATION, "database file [%s] opened", db_filename);
}


void close_database(){

    /** public function - see header **/

    //check database is open
    check_db_open(GET_CALL_INFO);

    //close the database
    int rc=sqlite3_close(db);
    if(rc != SQLITE_OK ){

        //don't use log_sqlite_error function as this already contains a call to close_database
        //and therefore creates an infinite loop
        log_event(EVENT_ERROR, "sqlite3_close failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "error code %i", rc);
    }

    fprintf(stderr, "database file closed\n");
    log_event(EVENT_SESSION, "database file closed");
}


bool table_exists(const char *table_name) {

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE tbl_name='%s'", table_name);

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //process the sql statement - we use a while loop instead of testing the return
    //value using 'rc != SQLITE_DONE' as the latter returns strange errors
    int count=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        count=sqlite3_column_int(stmt, 0);
   }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //catch duplicated tables (only likely to happen if the entire database is corrupted)
    if(count>1){

        log_event(EVENT_ERROR, "more than 1 table named [%s] found in function %s: module %s: line %i", table_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(count==0) return false;

    return true;
}


void check_table_exists(char *table_name, const char *module, const char *func, const int line){

    /** public function - see header **/

    if(table_exists(table_name)==false){

        log_event(EVENT_ERROR, "no table named [%s] found in database in function %s: module %s: line %i", table_name, func, module, line);
        fprintf(stderr, "no table named [%s] found in database in function %s: module %s: line %i", table_name,func, module, line);

        exit(EXIT_FAILURE);
    }
}


bool column_exists(const char *table_name, const char *column_name) {

    /** public function - see header **/

    sqlite3_stmt *stmt;
    int field_idx=-1;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "PRAGMA table_info('%s')", table_name);

    //prepare sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //loop through fields in the sqlite_master table until we find one called 'name'
    for (int i=0; i<sqlite3_column_count(stmt); i++) {

        if(strcmp("name", sqlite3_column_name(stmt,i))==0) {

            field_idx = i;
            break;
        }
    }

    //if sqlite_master has no field called 'name' then report as error
    if(field_idx==-1) {

        log_event(EVENT_ERROR, "sqlite_master has no 'name field in function %s: module %s: line %i", __func__, __FILE__, __LINE__);

        rc = sqlite3_finalize(stmt);

        if(rc!=SQLITE_OK){

            log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
            //server stopped automatically
        }
    }

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //compare the field name to the target
        if(strcmp(column_name, (const char *)sqlite3_column_text(stmt, field_idx))==0) {

            return 1;
        }
    }

    sqlite3_finalize(stmt);

    return 0;
}


int database_table_count(){

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    //create the sql statement
    char sql[MAX_SQL_LEN]="SELECT count(*) FROM sqlite_master WHERE type='table'";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //process the sql statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    int table_count=sqlite3_column_int(stmt, 0);

    //destroy the sql statement
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

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    //check that sql str contains a valid table creation instruction
    if(strncmp(sql, "CREATE TABLE", 12)!=0){

        log_event(EVENT_ERROR, "sql statement does not contain 'CREATE TABLE' instruction in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "[%s]", sql);
        stop_server();
    }

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //process the sql statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite_step failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //destroy the sql statement
    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //extract table name for use in logging
    char table_name[80]="";

    for(size_t i=13; i<strlen(sql); i++){

        if(sql[i]=='('){

            strncpy(table_name, sql+13, i-13);
            break;
        }
    }

    if(strlen(table_name)==0){

        log_event(EVENT_ERROR, "table name not found in sql statement in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "[%s]", sql);
        stop_server();
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);
    fprintf(stderr, "created table [%s]\n", table_name);
}


void process_sql(const char *sql){

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //process the sql statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc != SQLITE_OK){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
        //server stopped automatically
    }
}


void populate_database(const char *db_filename){

    /** public function - see header **/

    //check database is open
    check_db_closed(GET_CALL_INFO);

    //create logical divider in log file and on console
    log_text(EVENT_INITIALISATION, "\nCreating database tables...\n");
    fprintf(stderr, "\nCreating database tables...\n");

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
    create_database_table(SKILL_TABLE_SQL);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");

    add_db_game_data(1, 27225, 1, 27225, 360, REQUIRED_DATABASE_VERSION);

    add_db_channel(1, 0, CHAN_PERMANENT, "", "Main Channel", "A channel for chatting", 1);

    batch_add_races(RACE_FILE); //add race before gender and char type
    batch_add_gender(GENDER_FILE);
    batch_add_char_types(CHAR_TYPE_FILE);
    batch_add_seasons(SEASON_FILE);
    batch_add_objects(OBJECT_FILE); //add objects before e3ds and maps
    batch_add_e3ds(E3D_FILE);
    batch_add_maps(MAP_FILE);//also adds map objects
    batch_add_skills(HARVESTING_SKILL, HARVESTING_SKILL_FILE);

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

        float i=50.0f;

        for(int pick_points=0; pick_points<=50; pick_points++){

            attribute_value[pick_points]=(int)i;
            i=i + 0.20f;
        }

        add_db_attribute(race_id, ATTR_CARRY_CAPACITY, attribute_value);
        attribute_id++;
    }

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
    fprintf(stderr, "Player [master_character][test] added successfully\n");

    fprintf(stderr, "Database [%s] created\n", db_filename);
}
