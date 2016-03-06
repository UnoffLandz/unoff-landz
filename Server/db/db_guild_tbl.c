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
#include <stdio.h> // support printf
#include <stdlib.h> //support for NULL
#include <string.h>  //support for strcpy

#include "database_functions.h"
#include "../logging.h"
#include "../guilds.h"
#include "../server_start_stop.h"
#include "../string_functions.h"


void load_db_guilds(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading guilds...");

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GUILD_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="SELECT * FROM GUILD_TABLE";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //read the sql query result into the guild array
    int i=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id=sqlite3_column_int(stmt, 0);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(guilds.guild[id].guild_name, (char*)sqlite3_column_text(stmt, 1));

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(guilds.guild[id].guild_tag, (char*)sqlite3_column_text(stmt, 2));

        guilds.guild[id].guild_tag_colour=sqlite3_column_int(stmt, 3);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 4)) strcpy(guilds.guild[id].guild_description, (char*)sqlite3_column_text(stmt, 4));

        guilds.guild[id].date_guild_created=sqlite3_column_int(stmt, 5);
        guilds.guild[id].permission_level=sqlite3_column_int(stmt, 6);
        guilds.guild[id].status=sqlite3_column_int(stmt, 7);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", id, guilds.guild[id].guild_tag);

        i++;
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    if(i==0){

        log_event(EVENT_ERROR, "no guilds found in database", i);
        stop_server();
    }
}


int add_db_guild(char *guild_name, char *guild_tag, int guild_tag_colour, char *guild_description,
    int guild_permission_level,
    int guild_status){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GUILD_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="INSERT INTO GUILD_TABLE("  \
        "GUILD_NAME," \
        "GUILD_TAG," \
        "GUILD_TAG_COLOUR, " \
        "GUILD_DESCRIPTION," \
        "DATE_GUILD_CREATED," \
        "PERMISSION_LEVEL,"  \
        "STATUS" \
        ") VALUES(?, ?, ?, ?, ?, ?, ?)";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //bind the data
    int date_guild_created=(int)time(NULL);

    sqlite3_bind_text(stmt, 1, guild_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, guild_tag, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, guild_tag_colour);
    sqlite3_bind_text(stmt, 4, guild_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, date_guild_created);
    sqlite3_bind_int(stmt, 6, guild_permission_level);
    sqlite3_bind_int(stmt, 7, guild_status);

    //process the sql statement
    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", GET_CALL_INFO, rc, sql);
    }

    //find the id of the new entry
    strcpy(sql, "SELECT MAX(GUILD_ID) FROM GUILD_TABLE");

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

    //destroy the sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", GET_CALL_INFO, rc, sql);
    }

    fprintf(stderr, "Guild [%s] added successfully\n", guild_tag);

    log_event(EVENT_INITIALISATION, "Added guild [%s] to GUILD_TABLE", guild_tag);

    return id;
}


void get_db_guild_member_list(int guild_id, int order){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("GUILD_TABLE", GET_CALL_INFO);

    char sql[MAX_SQL_LEN]="";

    if(order==GUILD_ORDER_RANK){

        snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE GUILD_ID=%i ORDER BY GUILD_RANK", guild_id);
    }
    else if (order==GUILD_ORDER_TIME){

        snprintf(sql, MAX_SQL_LEN, "SELECT * FROM CHARACTER_TABLE WHERE GUILD_ID=%i ORDER BY JOINED_GUILD", guild_id);
    }
    else {

        log_event(EVENT_ERROR, "unknown order type [%i] function %s: module %s: line %i", order, GET_CALL_INFO);
        stop_server();
    }

    //prepare sql command
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", GET_CALL_INFO, rc, sql);
    }

    //zero the struct
    memset(&guild_member_list, 0, sizeof(guild_member_list));

    int i=0;

    //execute sql command
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(guild_member_list.guild_member[i].character_name, (char*) sqlite3_column_text(stmt, 1));

        guild_member_list.guild_member[i].date_joined_guild=sqlite3_column_int(stmt, 30);
        guild_member_list.guild_member[i].guild_rank=sqlite3_column_int(stmt, 13);

        i++;

        //if number of guild members exceeds maximum, log as an error but do not stop server
        if(i==MAX_GUILD_MEMBERS){

            log_event(EVENT_ERROR, "guild member count exceeds maximum guild members [%i]", MAX_GUILD_MEMBERS);
            break;
        }
    }

    guild_member_list.guild_member_count=i;

    //destroy sql command
    rc=sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize", GET_CALL_INFO, rc, sql);
    }
}


void batch_add_guilds(char *file_name){

    /**public function - see header**/

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "guild load file [%s] not found", file_name);
        exit(EXIT_FAILURE);
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding guilds specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding guilds specified in file [%s]\n", file_name);

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[6][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        add_db_guild(output[0], output[1], atoi(output[2]), output[3], atoi(output[4]), atoi(output[5]));
    }

    fclose(file);
}

