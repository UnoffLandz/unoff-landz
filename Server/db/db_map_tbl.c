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

#include <stdio.h>  //support for NULL
#include <string.h> //support for strcpy and memcpy
#include <stdint.h> //support for int32_t datatype
#include <stdlib.h> //support EXIT_FAILURE

#include "database_functions.h"
#include "db_map_tbl.h"
#include "db_e3d_tbl.h"
#include "db_object_tbl.h"
#include "db_map_object_tbl.h"
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

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("MAP_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM MAP_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    //read the sql query result into the map array
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the map id and check that the value does not exceed the maximum permitted number of maps
        int map_id=sqlite3_column_int(stmt,0);

        if(map_id>MAX_MAPS) {

            log_event(EVENT_ERROR, "map id [%i] exceeds range [%i] in function load_maps: module database.c", map_id, MAX_MAPS);
            stop_server();
        }

        //get map name
        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(maps.map[map_id].map_name, (char*)sqlite3_column_text(stmt, 1));
        log_event(EVENT_MAP_LOAD, "Loading map [%i] map_name [%s]", map_id,  maps.map[map_id].map_name);

        //get map description
        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(maps.map[map_id].description, (char*)sqlite3_column_text(stmt, 2));
        log_text(EVENT_MAP_LOAD, "map description [%s]", maps.map[map_id].description);

        char elm_filename[80]="";

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 3)) strcpy(elm_filename, (char*)sqlite3_column_text(stmt, 3));
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

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 7)) strcpy(maps.map[map_id].author, (char*)sqlite3_column_text(stmt, 7));
        log_text(EVENT_MAP_LOAD, "map author [%s]", maps.map[map_id].author);

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 8)) strcpy(maps.map[map_id].author_email, (char*)sqlite3_column_text(stmt, 8));
        log_text(EVENT_MAP_LOAD, "map author email[%s]", maps.map[map_id].author_email);

        //get map status
        maps.map[map_id].development_status=sqlite3_column_int(stmt, 9);
        log_text(EVENT_MAP_LOAD, "map status[%i]", maps.map[map_id].development_status);

        //get map upload date
        maps.map[map_id].upload_date=sqlite3_column_int(stmt, 10);
        log_text(EVENT_MAP_LOAD, "map upload date[%i]", maps.map[map_id].upload_date);

        maps.map[map_id].threed_object_count=sqlite3_column_int(stmt, 11);
        log_text(EVENT_MAP_LOAD, "threed object count[%i]", maps.map[map_id].threed_object_count);

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", map_id, maps.map[map_id].map_name);

        i++;
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    if(i==0){

        log_event(EVENT_ERROR, "no maps found in database", i);
        stop_server();
    }
}


bool get_db_map_exists(int map_id) {

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("MAP_TABLE", GET_CALL_INFO);

    char *sql="SELECT count(*) FROM MAP_TABLE WHERE MAP_ID=?";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, map_id);

    //execute the sql statement
    int map_id_count=0;
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        map_id_count=sqlite3_column_int(stmt, 0);
    }

    //check for duplicates
    if(map_id_count>1) {

        log_event(EVENT_ERROR, "there are [%i] entries in MAP_TABLE with id [%i]", map_id_count, map_id);
        stop_server();
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    //return false if map_id found otherwise true
    if(map_id_count==0) return false;

    return true;
}


void delete_map(int map_id){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);

    char *sql="DELETE FROM MAP_TABLE WHERE MAP_ID=?";

    sqlite3_stmt *stmt;

    prepare_query(sql, &stmt, GET_CALL_INFO);

    sqlite3_bind_int(stmt, 1, map_id);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);
}


void add_db_map(int map_id, char *elm_filename){

    /** public function - see header */

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("MAP_TABLE", GET_CALL_INFO);

    read_elm_header(elm_filename);
    read_tile_map(elm_filename, maps.map[map_id].tile_map);
    read_height_map(elm_filename, maps.map[map_id].height_map);

    //insert map data (as we are inserting blobs, we need to bind values rather that adding values to the string
    char *sql="INSERT INTO MAP_TABLE("  \
                "MAP_ID," \
                "ELM_FILE_NAME, " \
                "MAP_AXIS," \
                "TILE_MAP," \
                "HEIGHT_MAP," \
                "MAP_UPLOAD_DATE,"
                "THREED_OBJECT_COUNT"
                ") VALUES(?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;

    prepare_query(sql, &stmt, GET_CALL_INFO);

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

    //load threed object count
    sqlite3_bind_int(stmt, 7, elm_header.threed_object_count);

    step_query(sql, &stmt, GET_CALL_INFO);

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "Map [%s] added successfully\n", elm_filename);

    log_event(EVENT_SESSION, "Added map [%i] file name [%s] to MAP_TABLE", map_id, elm_filename);
}


void list_db_maps(){

    /** public function - see header */

    sqlite3_stmt *stmt;

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("MAP_TABLE", GET_CALL_INFO);

    char *sql="SELECT * FROM MAP_TABLE";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    fprintf(stderr, "%6s %s %s\n", "[MAP ID]", "[MAP_NAME]", "[ELM FILE]");

    //read the sql query result into the map array
    int rc=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the map id and check that the value does not exceed the maximum permitted number of maps
        int map_id=sqlite3_column_int(stmt,0);

        //get map name
        char map_name[80]="";

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 1)) strcpy(map_name, (char*)sqlite3_column_text(stmt, 1));

        //get map elm file and add client map path so that send_change_map protocol tells client where to find the file
        char map_file_name[80]="";

        //handle null string which would crash strcpy
        if(sqlite3_column_text(stmt, 2)) strcpy(map_file_name, (char*)sqlite3_column_text(stmt, 2));

        fprintf(stderr, "[%6i] [%s] [%s]\n", map_id, map_name, map_file_name);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);
}


void batch_add_maps(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "file [%s] not found", file_name);
        stop_server();
    }

    char line[160]="";
    int line_counter=0;

    log_event(EVENT_INITIALISATION, "\nAdding maps specified in file [%s]", file_name);
    fprintf(stderr, "\nAdding maps specified in file [%s]\n", file_name);

    //check database is open and table exists
    check_db_open(GET_CALL_INFO);
    check_table_exists("MAP_TABLE", GET_CALL_INFO);

    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    char *sql="INSERT INTO MAP_TABLE("  \
                "MAP_ID," \
                "MAP_NAME, " \
                "MAP_DESCRIPTION," \
                "ELM_FILE_NAME, " \
                "MAP_AUTHOR, " \
                "MAP_AUTHOR_EMAIL, " \
                "MAP_STATUS, " \
                "MAP_AXIS, " \
                "TILE_MAP, " \
                "HEIGHT_MAP, " \
                "MAP_UPLOAD_DATE, " \
                "THREED_OBJECT_COUNT" \
                ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    prepare_query(sql, &stmt, GET_CALL_INFO);

    int rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *&sErrMsg, sql);
    }

    int count=0;

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[8][MAX_LST_LINE_LEN];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        int map_id=atoi(output[0]);

        sqlite3_bind_int(stmt, 1, map_id);                       //map id
        sqlite3_bind_text(stmt, 2, output[1], -1, SQLITE_STATIC);   //map name
        sqlite3_bind_text(stmt, 3, output[2], -1, SQLITE_STATIC);   //map description
        sqlite3_bind_text(stmt, 4, output[3], -1, SQLITE_STATIC);   //elm filename
        sqlite3_bind_text(stmt, 5, output[4], -1, SQLITE_STATIC);   //author
        sqlite3_bind_text(stmt, 6, output[5], -1, SQLITE_STATIC);   //author email
        sqlite3_bind_int(stmt, 7, atoi(output[6]));                 //map status

        read_elm_header(output[3]);
        read_tile_map(output[3], maps.map[map_id].tile_map);
        read_height_map(output[3], maps.map[map_id].height_map);

        int map_axis=elm_header.h_tiles * STEP_TILE_RATIO;
        sqlite3_bind_int(stmt, 8, map_axis);

        int tile_map_size=elm_header.height_map_offset - elm_header.tile_map_offset;
        sqlite3_bind_blob(stmt, 9, maps.map[map_id].tile_map, tile_map_size, NULL);

        int height_map_size=elm_header.threed_object_offset - elm_header.height_map_offset;
        sqlite3_bind_blob(stmt, 10, maps.map[map_id].height_map, height_map_size, NULL);

        sqlite3_bind_int(stmt, 11, (int)time(NULL));

        sqlite3_bind_int(stmt, 12, elm_header.threed_object_count);

        step_query(sql, &stmt, GET_CALL_INFO);

        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);

        fprintf(stderr, "Map [%s] added successfully\n", output[1]);
        log_event(EVENT_SESSION, "Added map [%i] file name [%s] to MAP_TABLE", map_id, output[1]);

        count++;
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_event(EVENT_ERROR, "sqlite3_exec failed", GET_CALL_INFO);
        log_text(EVENT_ERROR, "return code [%i] message [%s] sql [%s]", rc, *sErrMsg, sql);
    }

    destroy_query(sql, &stmt, GET_CALL_INFO);

    fclose(file);

    //load map data to memory so this can be used by other functions
    load_db_maps();

    maps.count=count;

    //mark data as loaded
    maps.data_loaded=true;
}
