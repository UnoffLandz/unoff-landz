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

#include <stdio.h>  //support for NULL
#include <string.h> //support for strcpy and memcpy
#include <stdint.h> //support for int32_t datatype

#include "database_functions.h"
#include "db_map_tbl.h"
#include "db_e3d_tbl.h"
//#include "db_map_object_tbl.h"
#include "db_object_tbl.h"

#include "../logging.h"
#include "../map_object.h"
#include "../maps.h"
#include "../server_start_stop.h"
#include "../file_functions.h"
#include "../string_functions.h"
#include "../numeric_functions.h"
#include "../e3d.h"


void load_db_maps(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading maps...");

    sqlite3_stmt *stmt;
    int i=0;

    char sql[MAX_SQL_LEN]="SELECT * FROM MAP_TABLE";

    //check database table exists
    char database_table[80];
    strcpy(database_table, strstr(sql, "FROM")+5);
    if(table_exists(database_table)==false){

        log_event(EVENT_ERROR, "table [%s] not found in database", database_table);
        stop_server();
    }

    //prepare sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //read the sql query result into the map array
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the map id and check that the value does not exceed the maximum permitted number of maps
        int map_id=sqlite3_column_int(stmt,0);

        if(map_id>MAX_MAPS) {

            log_event(EVENT_ERROR, "map id [%i] exceeds range [%i] in function load_maps: module database.c", map_id, MAX_MAPS);
            stop_server();
        }

        //get map name
        strcpy(maps.map[map_id].map_name, (char*)sqlite3_column_text(stmt, 1));
        log_event(EVENT_MAP_LOAD, "Loading map [%i] map_name [%s]", map_id,  maps.map[map_id].map_name);

        //get map description
        strcpy(maps.map[map_id].description, (char*)sqlite3_column_text(stmt, 2));
        log_text(EVENT_MAP_LOAD, "map description [%s]", maps.map[map_id].description);

        //get elm filename
        char elm_filename[80]="";
        strcpy(elm_filename, (char*)sqlite3_column_text(stmt, 3));
        sprintf(maps.map[map_id].elm_filename, "%s%s", CLIENT_MAP_PATH, elm_filename);
        log_text(EVENT_MAP_LOAD, "elm filename [%s]", elm_filename);

        //get map axis
        maps.map[map_id].map_axis=sqlite3_column_int(stmt, 4);

        //get tile map
        size_t tile_map_size=(size_t)sqlite3_column_bytes(stmt, 5);
        maps.map[map_id].tile_map_size=tile_map_size;
        memcpy(maps.map[map_id].tile_map, (unsigned char*)sqlite3_column_blob(stmt, 5), (size_t)tile_map_size);

        //This temporary code reads the tile map directly from the elm file
        //maps.map[map_id].le_map_size=(size_t)sqlite3_column_bytes(stmt, 6);
        //read_tile_map(elm_filename, maps.map[map_id].tile_map);

        //get height map
        size_t height_map_size=(size_t)sqlite3_column_bytes(stmt, 6);
        maps.map[map_id].height_map_size=height_map_size;
        memcpy(maps.map[map_id].height_map, (unsigned char*)sqlite3_column_blob(stmt, 6), (size_t)height_map_size);

        //This temporary code reads the height map directly from the elm file
        //maps.map[map_id].height_map_size=(size_t)sqlite3_column_bytes(stmt, 6);
        //read_height_map(elm_filename, maps.map[map_id].height_map);

        //this temporary code reads the 3d object list directly from the elm file
        //read_threed_object_list(elm_filename);

        //get map author
        strcpy(maps.map[map_id].author, (char*)sqlite3_column_text(stmt, 7));
        log_text(EVENT_MAP_LOAD, "map author [%s]", maps.map[map_id].author);

        //get map author email
        strcpy(maps.map[map_id].author_email, (char*)sqlite3_column_text(stmt, 8));
        log_text(EVENT_MAP_LOAD, "map author email[%s]", maps.map[map_id].author_email);

        //get map status
        maps.map[map_id].development_status=sqlite3_column_int(stmt, 9);
        log_text(EVENT_MAP_LOAD, "map status[%i]", maps.map[map_id].development_status);

        //get map upload date
        maps.map[map_id].upload_date=sqlite3_column_int(stmt, 10);
        log_text(EVENT_MAP_LOAD, "map upload date[%i]", maps.map[map_id].upload_date);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", map_id, maps.map[map_id].map_name);

        i++;
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

   if(i==0){

        log_event(EVENT_ERROR, "no maps found in database", i);
        stop_server();
    }
}

bool get_db_map_exists(int map_id) {

    /** public function - see header */

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT count(*) FROM MAP_TABLE WHERE MAP_ID=?";

    //prepare the sql statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //bind the sql statement
    rc = sqlite3_bind_int(stmt, 1, map_id);
    if(rc!=SQLITE_OK) {
        log_sqlite_error("sqlite3_bind_int failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //execute the sql statement
    int map_id_count=0;
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        map_id_count=sqlite3_column_int(stmt, 0);
    }

    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        stop_server();
    }

    //check for duplicates
    if(map_id_count>1) {

        log_event(EVENT_ERROR, "there are [%i] entries in MAP_TABLE with id [%i]", map_id_count, map_id);
        stop_server();
    }

    //discard sql statement
    sqlite3_finalize(stmt);
    if(rc!=SQLITE_DONE) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //return false if map_id found otherwise true
    if(map_id_count==0) return false;

    return true;
}

void delete_map(int map_id){

    /** public function - see header */

    char sql[MAX_SQL_LEN]="DELETE FROM MAP_TABLE WHERE MAP_ID=?";

    sqlite3_stmt *stmt;

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //bind value to sql statement
    rc = sqlite3_bind_int(stmt, 1, map_id);
    if(rc!=SQLITE_OK) {
        log_sqlite_error("sqlite3_bind_int failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //process sql statement
    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }
}


void add_db_map_old(int map_id, char *elm_filename, char *map_name, char *map_description, char *map_author,
    char *map_author_email, int status){

    /** public function - see header */

    read_elm_header(elm_filename);
    read_tile_map(elm_filename, maps.map[map_id].tile_map);
    read_height_map(elm_filename, maps.map[map_id].height_map);

/*do this separately*/
    //read_threed_object_list(elm_filename, map_id);
    //add_db_map_objects(elm_filename, map_id);

    //insert map data (as we are inserting blobs, we need to bind values rather that adding values to the string
    char sql[MAX_SQL_LEN]="INSERT INTO MAP_TABLE("  \
                "MAP_ID," \
                "MAP_NAME,"  \
                "MAP_DESCRIPTION," \
                "ELM_FILE_NAME, " \
                "MAP_AXIS," \
                "TILE_MAP," \
                "HEIGHT_MAP, " \
                "MAP_AUTHOR, " \
                "MAP_AUTHOR_EMAIL, " \
                "MAP_UPLOAD_DATE, " \
                "MAP_STATUS"
                ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_int(stmt, 1, map_id);
    sqlite3_bind_text(stmt, 2, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, map_description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, elm_filename, -1, SQLITE_STATIC);

    int map_axis=elm_header.h_tiles * STEP_TILE_RATIO;
    sqlite3_bind_int(stmt, 5, map_axis);

    int tile_map_size=elm_header.height_map_offset - elm_header.tile_map_offset;
    sqlite3_bind_blob(stmt, 6, maps.map[map_id].tile_map, tile_map_size, NULL);

    int height_map_size=elm_header.threed_object_offset - elm_header.height_map_offset;
    sqlite3_bind_blob(stmt, 7, maps.map[map_id].height_map, height_map_size, NULL);

    sqlite3_bind_text(stmt, 8, map_author, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, map_author_email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 10, status);
    sqlite3_bind_int(stmt, 11, (int)time(NULL));

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Map [%s] added successfully\n", map_name);

    log_event(EVENT_SESSION, "Added map [%s] file name [%s] to MAP_TABLE", map_name, elm_filename);
}


void add_db_map(int map_id, char *elm_filename){

    /** public function - see header */

    read_elm_header(elm_filename);
    read_tile_map(elm_filename, maps.map[map_id].tile_map);
    read_height_map(elm_filename, maps.map[map_id].height_map);

    //insert map data (as we are inserting blobs, we need to bind values rather that adding values to the string
    char sql[MAX_SQL_LEN]="INSERT INTO MAP_TABLE("  \
                "MAP_ID," \
                "ELM_FILE_NAME, " \
                "MAP_AXIS," \
                "TILE_MAP," \
                "HEIGHT_MAP," \
                "MAP_UPLOAD_DATE"
                ") VALUES(?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //load map id
    sqlite3_bind_int(stmt, 1, map_id);
    sqlite3_bind_text(stmt, 2, elm_filename, -1, SQLITE_STATIC);

    //load map axis
    int map_axis=elm_header.h_tiles * STEP_TILE_RATIO;
    sqlite3_bind_int(stmt, 3, map_axis);

    //load tile map
    int tile_map_size=elm_header.height_map_offset - elm_header.tile_map_offset;
    sqlite3_bind_blob(stmt, 4, maps.map[map_id].tile_map, tile_map_size, NULL);

    //load height map
    int height_map_size=elm_header.threed_object_offset - elm_header.height_map_offset;
    sqlite3_bind_blob(stmt, 5, maps.map[map_id].height_map, height_map_size, NULL);

    //load map upload date
    sqlite3_bind_int(stmt, 6, (int)time(NULL));

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Map [%s] added successfully\n", elm_filename);

    log_event(EVENT_SESSION, "Added map [%i] file name [%s] to MAP_TABLE", map_id, elm_filename);
}


void list_db_maps(){

    /** public function - see header */

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM MAP_TABLE";

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("%6s %s %s\n", "[MAP ID]", "[MAP_NAME]", "[ELM FILE]");

    //read the sql query result into the map array
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the map id and check that the value does not exceed the maximum permitted number of maps
        int map_id=sqlite3_column_int(stmt,0);

        //get map name
        char map_name[80]="";
        strcpy(map_name, (char*)sqlite3_column_text(stmt, 1));

        //get map elm file and add client map path so that send_change_map protocol tells client where to find the file
        char map_file_name[80]="";
        strcpy(map_file_name, (char*)sqlite3_column_text(stmt, 2));

        printf("[%6i] [%s] [%s]\n", map_id, map_name, map_file_name);
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }
}


void change_db_map_name(int map_id, char *map_name){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //use parameters rather than inserting values as this enables apostrophe characters
    //to be handled
    char sql[MAX_SQL_LEN]="UPDATE MAP_TABLE SET MAP_NAME=? WHERE MAP_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_text(stmt, 1, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, map_id);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "Map [%i] name changed to [%s] on MAP_TABLE", map_id, map_name);
}


void change_db_map_description(int map_id, char *map_description){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //use parameters rather than inserting values as this enables apostrophe characters
    //to be handled
    char sql[MAX_SQL_LEN]="UPDATE MAP_TABLE SET MAP_DESCRIPTION=? WHERE MAP_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_text(stmt, 1, map_description, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, map_id);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "Map [%i] description changed to [%s] on MAP_TABLE", map_id, map_description);
}


void change_db_map_author(int map_id, char *map_author){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //use parameters rather than inserting values as this enables apostrophe characters
    //to be handled
    char sql[MAX_SQL_LEN]="UPDATE MAP_TABLE SET MAP_AUTHOR=? WHERE MAP_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_text(stmt, 1, map_author, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, map_id);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "Map [%i] author changed to [%s] on MAP_TABLE", map_id, map_author);
}


void change_db_map_author_email(int map_id, char *map_author_email){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //use parameters rather than inserting values as this enables apostrophe characters
    //to be handled
    char sql[MAX_SQL_LEN]="UPDATE MAP_TABLE SET MAP_AUTHOR_EMAIL=? WHERE MAP_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_text(stmt, 1, map_author_email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, map_id);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "Map [%i] author email changed to [%s] on MAP_TABLE", map_id, map_author_email);
}

void change_db_map_development_status(int map_id, int map_development_status){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //use parameters rather than inserting values as this enables apostrophe characters
    //to be handled
    char sql[MAX_SQL_LEN]="UPDATE MAP_TABLE SET MAP_STATUS=? WHERE MAP_ID=?";

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_int(stmt, 1, map_development_status);
    sqlite3_bind_int(stmt, 2, map_id);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_SESSION, "Map [%i] development status changed to [%i] on MAP_TABLE", map_id, map_development_status);
}
