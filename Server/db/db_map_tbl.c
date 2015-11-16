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
#include <stdlib.h> //testing only
#include <stdint.h> //support for int32_t datatype

#include "database_functions.h"
#include "db_map_tbl.h"
#include "db_e3d_tbl.h"
#include "../logging.h"
#include "../maps.h"
#include "../server_start_stop.h"
#include "../file_functions.h"
#include "../global.h"
#include "../string_functions.h"
#include "../objects.h"
#include "../numeric_functions.h"
#include "../e3d.h"

#define DEBUG_MAPS 1

void load_db_maps(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading maps...");

    sqlite3_stmt *stmt;
    int i=0;
    char raw_elm_filename[80]="";

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
        sprintf(maps.map[map_id].elm_filename, "%s%s", CLIENT_MAP_PATH, (char*)sqlite3_column_text(stmt, 3));
        strcpy(raw_elm_filename, (char*)sqlite3_column_text(stmt, 3));
        log_text(EVENT_MAP_LOAD, "elm filename [%s]", maps.map[map_id].elm_filename);

        //get map axis
        maps.map[map_id].map_axis=sqlite3_column_int(stmt, 4);

        //get tile map
        int tile_map_size=sqlite3_column_bytes(stmt, 5);
        maps.map[map_id].tile_map_size=tile_map_size;
        memcpy(maps.map[map_id].tile_map, (unsigned char*)sqlite3_column_blob(stmt, 5), (size_t)tile_map_size);

        //get height map
/*
        int height_map_size=sqlite3_column_bytes(stmt, 6);
        maps.map[map_id].height_map_size=height_map_size;
        memcpy(maps.map[map_id].height_map, (unsigned char*)sqlite3_column_blob(stmt, 6), (size_t)height_map_size);
*/

        read_height_map(raw_elm_filename, maps.map[map_id].height_map, &maps.map[map_id].height_map_size, &maps.map[map_id].map_axis);

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

int get_db_map_exists(int map_id) {

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


void add_db_map(int map_id, char *elm_filename, char *map_name, char *map_description, char *map_author,
    char *map_author_email, int status){

    /** public function - see header */

    //check map id is unused
    if(get_db_map_exists(map_id)==true){

        //load all maps into the map struct so that, when we can log the name of the map that
        //is being replaced
        load_db_maps();
        log_text(EVENT_INITIALISATION, "");//insert logical separator in log file

        log_event(EVENT_ERROR, "map [%i] [%s] replaced", map_id, maps.map[map_id].map_name);
    }

    sqlite3_stmt *stmt;

    struct __attribute__((__packed__)){

        unsigned char magic_number[4];
        int32_t h_tiles;
        int32_t v_tiles;
        int32_t tile_map_offset;
        int32_t height_map_offset;

        int32_t threed_object_hash_len;
        int32_t threed_object_count;
        int32_t threed_object_offset;

        int32_t twod_object_hash_len;
        int32_t twod_object_count;
        int32_t twod_object_offset;

        int32_t lights_object_hash_len;
        int32_t lights_object_count;
        int32_t lights_object_offset;

        unsigned char dungeon_flag;
        unsigned char version_flag;
        unsigned char reserved1;
        unsigned char reserved2;

        int32_t ambient_red;
        int32_t ambient_green;
        int32_t ambient_blue;

        int32_t particles_object_hash_len;
        int32_t particles_object_count;
        int32_t particles_object_offset;

        int32_t clusters_offset;

        int32_t reserved_9;
        int32_t reserved_10;
        int32_t reserved_11;
        int32_t reserved_12;
        int32_t reserved_13;
        int32_t reserved_14;
        int32_t reserved_15;
        int32_t reserved_16;
        int32_t reserved_17;

    } elm_header;

    //bounds check the map_id
    if(map_id>MAX_MAPS || map_id<1) {

        log_event(EVENT_ERROR, "map id [%i] exceeds range [%i] in function load_maps: module database.c", map_id, MAX_MAPS);
        stop_server();
    }

    //open elm file
    FILE *file;

    if((file=fopen(elm_filename, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //read the header
    unsigned char header_byte[ELM_FILE_HEADER]= {0};
    if(fread(header_byte, ELM_FILE_HEADER, 1, file)!=1) {

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }
    //copy bytes to header struct so we can extract data
    memcpy(&elm_header, header_byte, ELM_FILE_HEADER);

    //check the magic number
    if(elm_header.magic_number[0]!='e' ||
            elm_header.magic_number[1]!='l' ||
            elm_header.magic_number[2]!='m' ||
            elm_header.magic_number[3]!='f') {

        log_event(EVENT_ERROR, "elm file magic number [%c%c%c%] != [elmf] in function %s: module %s: line %i",
                  elm_header.magic_number[0],
                  elm_header.magic_number[1],
                  elm_header.magic_number[2],
                  elm_header.magic_number[3],
                  __func__, __FILE__, __LINE__);
        stop_server();
    }

    //check the header length
    if(elm_header.tile_map_offset!=ELM_FILE_HEADER) {

        log_event(EVENT_ERROR, "elm file header [%i] is not equal to [%i] in function %s: module %s: line %i", elm_header.tile_map_offset, ELM_FILE_HEADER, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //check the vertical and horizontal tile counts are equal
    if(elm_header.h_tiles!=elm_header.v_tiles) {

        log_event(EVENT_ERROR, "horizontal tile count [%i] and vertical tile count [%i] are unequal in function %s: module %s: line %i", elm_header.h_tiles, elm_header.v_tiles, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //calculate the map axis in steps (multiply tile axis by 6)
    int map_axis=elm_header.h_tiles * 6;

    //check the threed object hash structure length
    if(elm_header.threed_object_hash_len!=THREED_OBJECT_HASH_LENGTH) {

        log_event(EVENT_ERROR, "threed object hash length [%i] is not equal to [%i] in function %s: module %s: line %i", elm_header.threed_object_hash_len, THREED_OBJECT_HASH_LENGTH, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //extract the tile map from the elm file
    unsigned char tile_map_byte[TILE_MAP_MAX]={0};
    int tile_map_size=elm_header.height_map_offset - elm_header.tile_map_offset;

    if(tile_map_size>TILE_MAP_MAX){

        log_event(EVENT_ERROR, "tile map size [%i] exceeds maximum [%i] in function %s: module %s: line %i", tile_map_size, TILE_MAP_MAX, elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(fread(tile_map_byte, (size_t)tile_map_size, 1, file)!=1) {

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //extract the height map from the elm file
    unsigned char height_map_byte[HEIGHT_MAP_MAX]= {0};
    int height_map_size=elm_header.threed_object_offset - elm_header.height_map_offset;

    if(height_map_size>HEIGHT_MAP_MAX){

        log_event(EVENT_ERROR, "height map size [%i] exceeds maximum [%i] in function %s: module %s: line %i", height_map_size, HEIGHT_MAP_MAX, elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(fread(height_map_byte, (size_t)height_map_size, 1, file)!=1) {

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //extract the e3d object list and load into the MAP_OBJECT table
    sqlite3_stmt *stmt2;
    char *sErrMsg2 = 0;

    struct __attribute__((__packed__)) {

        char e3d_path_and_filename[80];
        float x_pos;
        float y_pos;
        float z_pos;
        float x_rot;
        float y_rot;
        float z_rot;
        unsigned char self_lit;
        unsigned char blended;
        unsigned char reserved[2];
        float r;
        float g;
        float b;
        unsigned char reserved2[24];
    } threed_object_entry;

    //load the e3d data from the database into memory as we'll need to know the e3d_id for each object
    load_db_e3ds();

    //as there's likely to be thousands of entries that take an age to load, create a sql statement to which
    //values can be replaced
    char sql2[MAX_SQL_LEN]="INSERT INTO MAP_OBJECT_TABLE("  \
         "THREEDOL_ID," \
         "MAP_ID,"  \
         "TILE," \
         "E3D_ID," \
         "HARVESTABLE," \
         "RESERVE" \
         ") VALUES(?, ?, ?, ?, ?, ?)";

    int rc2=sqlite3_prepare_v2(db, sql2, -1, &stmt2, NULL);
    if(rc2!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc2, sql2);
    }

    rc2=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg2);
    if(rc2!=SQLITE_OK){

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc2, sql2);
    }

    for(int i=0; i<elm_header.threed_object_count; i++) {

        //clear the struct
        memset(&threed_object_entry, 0, sizeof(threed_object_entry));

        //read the binary hash into the struct
        if(fread(&threed_object_entry, (size_t)elm_header.threed_object_hash_len, 1, file)!=1) {

            log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //adjust object coordinates to height map
        threed_object_entry.x_pos=threed_object_entry.x_pos * 2.00f;
        threed_object_entry.y_pos=threed_object_entry.y_pos * 2.00f;

        //extract the e3d file name from the path and file
        char e3d_filename[80]="";
        strcpy(e3d_filename, strrchr(threed_object_entry.e3d_path_and_filename, '/')+1 );

        //look up the id for the filename
        int e3d_id=get_e3d_id(e3d_filename);

        //usually we'd use the get_tile function to calculate the tile. However, this relies on the map struct
        //being fully loaded which only happens after a server start. We therefore have to calculate the tile
        //based on data from the elm header
        int tile=(int)threed_object_entry.x_pos + ((int)threed_object_entry.y_pos * map_axis);

        int harvestable=0;
        int reserve=0;

        sqlite3_bind_int(stmt2, 1, i);
        sqlite3_bind_int(stmt2, 2, map_id);
        sqlite3_bind_int(stmt2, 3, tile);
        sqlite3_bind_int(stmt2, 4, e3d_id);
        sqlite3_bind_int(stmt2, 5, harvestable);
        sqlite3_bind_int(stmt2, 6, reserve);

        rc2 = sqlite3_step(stmt2);
        if (rc2!= SQLITE_DONE) {

            log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc2, sql2);
        }

       sqlite3_clear_bindings(stmt2);
       sqlite3_reset(stmt2);
    }

    rc2=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg2);
    if (rc2!=SQLITE_OK) {

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc2, sql2);
    }

    rc2=sqlite3_finalize(stmt2);
    if(rc2!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc2, sql2);
    }

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

    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    sqlite3_bind_int(stmt, 1, map_id);
    sqlite3_bind_text(stmt, 2, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, map_description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, elm_filename, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, map_axis);
    sqlite3_bind_blob(stmt, 6, tile_map_byte, tile_map_size, NULL);
    sqlite3_bind_blob(stmt, 7, height_map_byte, height_map_size, NULL);
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

    fclose(file);

    printf("Map [%s] added successfully\n", map_name);

    log_event(EVENT_SESSION, "Added map [%s] file name [%s] to MAP_TABLE", map_name, elm_filename);
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
