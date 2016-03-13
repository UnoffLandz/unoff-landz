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
#include "db_sqlite_return_codes.h"
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


void open_database(const char *db_filename, const char *module, const char *func, const int line ){

    /** public function - see header **/

    //check database is closed
    check_db_closed(func, module, line);

    //open the database
    int rc = sqlite3_open(db_filename, &db);

    if( rc !=SQLITE_OK ){

        log_event(EVENT_ERROR, "sqlite3_open failed in function %s: module %s: line %i", db_filename, func, module, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "database file [%s] opened\n", db_filename);
    log_event(EVENT_INITIALISATION, "database file [%s] opened", db_filename);
}


void close_database(const char *module, const char *func, const int line){

    /** public function - see header **/

    //check database is open
    check_db_open(func, module, line);

    //close the database
    int rc=sqlite3_close(db);

    if(rc != SQLITE_OK ){

        log_event(EVENT_ERROR, "sqlite3_close failed in function %s: module %s: line %i", func, module, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "database file closed\n");
    log_event(EVENT_SESSION, "database file closed");
}


void prepare_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line){

     /** public function - see header **/

    int rc=sqlite3_prepare_v2(db, sql, -1, stmt, NULL);

    if(rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_prepare_v2 failed in function %s: module %s: line %i", module, func, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);
        log_text(EVENT_ERROR, "sql [%s]", sql);

        stop_server();
    }
}


void destroy_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line){

    /** public function - see header **/

    int rc=sqlite3_finalize(*stmt);

    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_finalize failed in function %s: module %s: line %i", module, func, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);
        log_text(EVENT_ERROR, "sql [%s]", sql);

        stop_server();
    }
}

void step_query(const char *sql, sqlite3_stmt **stmt, const char *module, const char *func, const int line){

    int rc=0;
    int i=0;

    while ( (rc = sqlite3_step(*stmt)) == SQLITE_ROW) {

        i++;
    }

    if(i>0){

        log_event(EVENT_ERROR, "multiple result from single sql execution in function %s: module %s: line %i", func, module, line);

        stop_server();
    }

    if (rc != SQLITE_DONE) {

        log_event(EVENT_ERROR, "sqlite3_step failed in function %s: module %s: line %i", func, module, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);
        log_text(EVENT_ERROR, "sql [%s]", sql);

        stop_server();
    }
}


bool table_exists(const char *table_name) {

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char *sql="SELECT count(*) FROM sqlite_master WHERE tbl_name=?";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_text(stmt, 1, table_name, -1, SQLITE_STATIC);

    //process the sql statement - we use a while loop instead of testing the return
    //value using 'rc != SQLITE_DONE' as the latter returns strange errors
    int count=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        count=sqlite3_column_int(stmt, 0);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    //catch duplicated tables (only likely to happen if the entire database is corrupted)
    if(count>1){

        log_event(EVENT_ERROR, "more than 1 table named [%s] found in function %s: module %s: line %i", table_name, GET_CALL_INFO);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    if(count==0) return false;

    return true;
}


void check_table_exists(char *table_name, const char *module, const char *func, const int line){

    /** public function - see header **/

    if(table_exists(table_name)==false){

        log_event(EVENT_ERROR, "no table named [%s] found in database in function %s: module %s: line %i", table_name, func, module, line);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }
}


bool column_exists(const char *table_name, const char *column_name) {

    /** public function - see header **/

    sqlite3_stmt *stmt;
    int field_idx=-1;

    //check database is open
    check_db_open(GET_CALL_INFO);

    char *sql="PRAGMA table_info(?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_text(stmt, 1, table_name, -1, SQLITE_STATIC);

    //loop through fields in the sqlite_master table until we find one called 'name'
    for (int i=0; i<sqlite3_column_count(stmt); i++) {

        if(strcmp("name", sqlite3_column_name(stmt,i))==0) {

            field_idx = i;
            break;
        }
    }

    //if sqlite_master has no field called 'name' then report as error
    if(field_idx==-1) {

        destroy_query(sql, &stmt, GET_CALL_INFO);

        log_event(EVENT_ERROR, "sqlite_master has no 'name field in function %s: module %s: line %i", GET_CALL_INFO);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    int rc=0;
    int count=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //compare the field name to the target
        if(strcmp(column_name, (const char *)sqlite3_column_text(stmt, field_idx))==0) count++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(count==1) return true;

    else if(count>1){

        log_event(EVENT_ERROR, "database has more than 1 field called [%s] in function %s: module %s: line %i", column_name, GET_CALL_INFO);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    return false;
}


int database_table_count(){

    /** public function - see header **/

    sqlite3_stmt *stmt;

    //check database is open
    check_db_open(GET_CALL_INFO);

    //create the sql statement
    char *sql="SELECT count(*) FROM sqlite_master WHERE type='table'";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    step_query(sql, &stmt, GET_CALL_INFO);

    int table_count=sqlite3_column_int(stmt, 0);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    return table_count;
}


void create_database_table(char *sql){

    /** RESULT  : creates a database table

       RETURNS  : void

       PURPOSE  : used by function create_new_database

       NOTES    :
    **/

    //check database is open
    check_db_open(GET_CALL_INFO);

    //check that sql str contains a valid table creation instruction
    if(strncmp(sql, "CREATE TABLE", 12)!=0){

        log_event(EVENT_ERROR, "sql statement does not contain 'CREATE TABLE' instruction in function %s: module %s: line %i", GET_CALL_INFO);
        log_text(EVENT_ERROR, "[%s]", sql);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    process_sql(sql, GET_CALL_INFO);

    //extract table name for use in logging
    char table_name[80]="";

    for(size_t i=13; i<strlen(sql); i++){

        if(sql[i]=='('){

            strncpy(table_name, sql+13, i-13);
            break;
        }
    }

    if(strlen(table_name)==0){

        log_event(EVENT_ERROR, "table name not found in sql statement in function %s: module %s: line %i", GET_CALL_INFO);
        log_text(EVENT_ERROR, "[%s]", sql);

        fprintf(stderr, "Program failed - see error log\n");
        exit(EXIT_FAILURE);
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);
    fprintf(stderr, "created table [%s]\n", table_name);
}


void process_sql(const char *sql, const char *module, const char *func, const int line){

    /** public function - see header **/

    //check database is open
    check_db_open(GET_CALL_INFO);

    char *errMsg;

    //use sqlite3_exec in place of sqlite3_prepare as there are no binds or need to query output
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed, return code [%i], error message [%s] in module: %s, function %s: line: %i", rc, errMsg, func, module, line);
        log_text(EVENT_ERROR, "return code [%i] [%s]", rc, sqlite_return_code[rc].err_msg);
        log_text(EVENT_ERROR, "sql [%s]", sql);

        stop_server();
    }
}


void populate_database(const char *db_filename){

    /** public function - see header **/

    //check database is open
    check_db_open(GET_CALL_INFO);

    //create logical divider in log file and on console
    log_text(EVENT_INITIALISATION, "\nCreating database tables...\n");
    fprintf(stderr, "\nCreating database tables...\n");

    //create database tables
    create_database_table(CHARACTER_TABLE_SQL);
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

    //add and load race before char type
    batch_add_races(RACE_FILE);
    load_db_char_races();

    //add and load gender before char type
    batch_add_gender(GENDER_FILE);
    load_db_genders();

    batch_add_char_types(CHAR_TYPE_FILE);

    batch_add_seasons(SEASON_FILE);

    batch_add_objects(OBJECT_FILE); //add objects before e3ds and maps

    batch_add_e3ds(E3D_FILE);
    batch_add_maps(MAP_FILE);//also adds map objects

    batch_add_skills(HARVESTING_SKILL_FILE, HARVESTING_SKILL);

    batch_add_attributes(ATTR_DAY_VISION_FILE, ATTR_DAY_VISION);
    batch_add_attributes(ATTR_NIGHT_VISION_FILE, ATTR_NIGHT_VISION);
    batch_add_attributes(ATTR_CARRY_CAPACITY_FILE, ATTR_CARRY_CAPACITY);

    batch_add_guilds(GUILD_FILE);

    //add and load game data before chars
    batch_add_game_data(GAME_DATA_FILE);
    load_db_game_data();

    //add and load channels before chars
    batch_add_channels(CHANNELS_FILE);
    load_db_channels();

    batch_add_characters(CHARACTER_FILE);

    fprintf(stderr, "\nDatabase [%s] created\n", db_filename);
}
