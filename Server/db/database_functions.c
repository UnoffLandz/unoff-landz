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

#include <stdio.h> //supports snprintf
#include <string.h> //supports strlen

#include "database_functions.h"
#include "../logging.h"
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
#include "../server_start_stop.h"
#include "../attributes.h"
#include "../chat.h"

sqlite3 *db;

int current_database_version();
static int prepare_query(const char *sql,sqlite3_stmt **stmt,const char *_func,int line)
{
    int rc=sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
    if(rc!=SQLITE_OK) {
        log_sqlite_error("sqlite3_prepare_v2 failed", _func, __FILE__, line, rc, sql);
        return -1;
    }
    return 0;
}
void open_database(char *database_name){

   /** public function - see header **/

    int rc = sqlite3_open(database_name, &db);

    if( rc !=SQLITE_OK ){

        log_sqlite_error("sqlite3_open", __func__ , __FILE__, __LINE__, rc, "");
    }
}
static int column_exists(const char *table,const char *column) {
    sqlite3_stmt *stmt;
    char sql[256];
    int rc;
    int name_column_idx=-1;

    snprintf(sql,256,"PRAGMA table_info(%s)",table);

    if(-1==prepare_query(sql,&stmt,__func__,__LINE__))
        return 0;

    for (int i=0; i<sqlite3_column_count(stmt); i++) {
        if(strcmp("name",sqlite3_column_name(stmt,i))==0) {
            name_column_idx = i;
            break;
    }
    }

    if(name_column_idx==-1) {
        sqlite3_finalize(stmt);
        return 0; //TODO: log information about strange table_info layout ?
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

int database_table_count(){

    /** public function - see header **/

    int rc;
    sqlite3_stmt *stmt;
    int table_count=0;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM sqlite_master WHERE type='table';");

    if(-1==prepare_query(sql,&stmt,__func__,__LINE__))
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
    **/

    int rc;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql,&stmt,__func__,__LINE__))
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
    int str_len=strlen(sql);

    int i=0;

    for(i=13; sql[i]!='('; i++){

        table_name[i-13]=sql[i];

        if(i>=str_len) {

            log_event(EVENT_ERROR, "unable to extract table name from sql string [%s] in function %s: module %s: line %i", sql, __func__, __FILE__, __LINE__);
            stop_server();
        }
    }

    log_event(EVENT_INITIALISATION, "Created table [%s]", table_name);
}

void process_sql(char *sql_str){

    /** public function - see header **/

    int rc=0;
    sqlite3_stmt *stmt;

    if(-1==prepare_query(sql_str,&stmt,__func__,__LINE__))
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
int current_database_version() {
    sqlite3_stmt *selectStmt;
    const char *sql_str = "SELECT db_version FROM GAME_DATA_TABLE";
    int rc = 0;

    if(0==column_exists("GAME_DATA_TABLE","db_version"))
        return 0;

    if(-1==prepare_query(sql_str,&selectStmt,__func__,__LINE__))
        return -1;

    rc = sqlite3_step(selectStmt);
    if (rc == SQLITE_DONE) {
        log_event(EVENT_ERROR,"Database is missing GAME_DATA_TABLE contents.");
        return -1;
    }
    else if (rc != SQLITE_ROW) {
        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql_str);
    }
    else {
        return sqlite3_column_int(selectStmt, 0);
    }
    rc = sqlite3_finalize(selectStmt);

    return 0;

}

void create_default_database(){

    /** public function - see header **/

    //create logical divider in log file
    log_text(EVENT_INITIALISATION, "\nCreating database tables...");

    //create database tables
    create_database_table(CHARACTER_TABLE_SQL);
    create_database_table(INVENTORY_TABLE_SQL);
    create_database_table(GENDER_TABLE_SQL);
    //create_database_table(ITEM_TABLE_SQL);
    //create_database_table(THREED_OBJECT_TABLE_SQL);
    create_database_table(MAP_TABLE_SQL);
    create_database_table(CHANNEL_TABLE_SQL);
    create_database_table(RACE_TABLE_SQL);
    //create_database_table(GUILD_TABLE_SQL);
    create_database_table(CHARACTER_TYPE_TABLE_SQL);
    create_database_table(ATTRIBUTE_TABLE_SQL);
    create_database_table(ATTRIBUTE_VALUE_TABLE_SQL);
    create_database_table(GAME_DATA_TABLE_SQL);
    create_database_table(SEASON_TABLE_SQL);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");

    add_db_game_data(1, 27225, 1, 27225, 360);

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

    int i=0, j=0, k=0, l=0;
    float attribute_value;

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "day vision", i, ATTR_DAY_VISION);

        attribute_value=10.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_DAY_VISION, k, attribute_value);
            attribute_value=attribute_value + 0.20f;
            l++;
        }
    }

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "night vision", i, ATTR_NIGHT_VISION);

        attribute_value=10.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_NIGHT_VISION, k, attribute_value);
            attribute_value=attribute_value + 0.20f;
            l++;
        }
    }

    for(i=1; i<=6; i++){

        j++;
        add_db_attribute(j, "carry capacity", i, ATTR_CARRY_CAPACITY);

        attribute_value=100.0f;

        for(k=1; k<=50; k++){

            add_db_attribute_value (l, j, ATTR_CARRY_CAPACITY, k, attribute_value);
            attribute_value=attribute_value + 18.0f;
            l++;
        }
    }


}
