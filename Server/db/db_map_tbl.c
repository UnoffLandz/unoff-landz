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

#include <stdio.h>  //support for NULL
#include <string.h> //support for strcpy and memcpy

#include "database_functions.h"
#include "../logging.h"
#include "../maps.h"
#include "../server_start_stop.h"
#include "../file_functions.h"
#include "../global.h"

int load_db_maps(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;
    int i=0, j=0, k=0;

    //the union converts an array containing the elm file contents into separate elements
    union {
        unsigned char byte[MAX_ELM_FILE_SIZE];

        struct{
            unsigned char magic_number[4];
            int h_tiles;
            int v_tiles;
            int tile_map_offset;
            int height_map_offset;

            int threed_object_hash_len;
            int threed_object_count;
            int threed_object_offset;

            int twod_object_hash_len;
            int twod_object_count;
            int twod_object_offset;

            int lights_object_hash_len;
            int lights_object_count;
            int lights_object_offset;

            unsigned char dungeon_flag;
            unsigned char version_flag;
            unsigned char reserved1;
            unsigned char reserved2;

            int ambient_red;
            int ambient_green;
            int ambient_blue;

            int particles_object_hash_len;
            int particles_object_count;
            int particles_object_offset;

            int clusters_offset;

            int reserved_9;
            int reserved_10;
            int reserved_11;
            int reserved_12;
            int reserved_13;
            int reserved_14;
            int reserved_15;
            int reserved_16;
            int reserved_17;

        }elm_header;
    }elm_file;

    //prepare the sql statement
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM MAP_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading maps...");

    //read the sql query result into the map array
    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        //get the map id and check that the value does not exceed the maximum permitted number of maps
        int map_id=sqlite3_column_int(stmt,0);

        if(map_id>MAX_MAPS){

            log_event(EVENT_ERROR, "map id [%i] exceeds range [%i] in function load_maps: module database.c", map_id, MAX_MAPS);
            stop_server();
        }

        //get map name
        strcpy(maps.map[map_id].map_name, (char*)sqlite3_column_text(stmt, 1));
        log_event(EVENT_MAP_LOAD, "Loading map [%i] map_name [%s]", map_id,  maps.map[map_id].map_name);

        //get map elm file and add client map path so that send_change_map protocol tells client where to find the file
        sprintf(maps.map[map_id].elm_filename, "%s%s", CLIENT_MAP_PATH, (char*)sqlite3_column_text(stmt, 2));
        log_text(EVENT_MAP_LOAD, "elm file name [%s]", maps.map[map_id].elm_filename);

        //check that we have elm file data
        int elm_file_size= sqlite3_column_bytes(stmt, 3);

        if(elm_file_size==0){

            log_event(EVENT_ERROR, "map id [%i] has no map data in function %s: module %s: line %i", map_id, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //read elm file data into union
        memcpy(elm_file.byte, (unsigned char*)sqlite3_column_blob(stmt, 3), elm_file_size);

        //check Magic Number
        log_text(EVENT_MAP_LOAD, "Magic Number [%c%c%c%c]", elm_file.elm_header.magic_number[0], elm_file.elm_header.magic_number[1], elm_file.elm_header.magic_number[2], elm_file.elm_header.magic_number[3]);

        if(elm_file.elm_header.magic_number[0]!='e' || elm_file.elm_header.magic_number[1]!='l' || elm_file.elm_header.magic_number[2]!='m' || elm_file.elm_header.magic_number[3]!='f'){

            log_event(EVENT_ERROR, "incorrect magic number for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //Horizontal and Vertical tile
        log_text(EVENT_MAP_LOAD, "Horizontal tiles [%i]", elm_file.elm_header.h_tiles);
        log_text(EVENT_MAP_LOAD, "Vertical tiles [%i]", elm_file.elm_header.v_tiles);

        if(elm_file.elm_header.h_tiles!=elm_file.elm_header.v_tiles){

            log_event(EVENT_ERROR, "horizontal and vertical tile counts are unequal for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //because there are 6 height map steps per tile, we calculate the map axis as 6x the number of
        //horizontal tiles. In the case of Isla Prima, the number of horizontal tiles is 32, so
        //our map axis is calculated as 6x32=192
        maps.map[map_id].map_axis=elm_file.elm_header.v_tiles*6;

        //get tile map size
        log_text(EVENT_MAP_LOAD, "Tile map offset [%i]", elm_file.elm_header.tile_map_offset);

        if(elm_file.elm_header.tile_map_offset!=ELM_FILE_HEADER_LENGTH){

            log_event(EVENT_ERROR, "tile map offset does not equal elm file header length for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //log height map offset
        log_text(EVENT_MAP_LOAD, "Height map offset [%i]", elm_file.elm_header.height_map_offset);

        //get 3d object hash length
        log_text(EVENT_MAP_LOAD, "3d object hash length [%i]", elm_file.elm_header.threed_object_hash_len);
        maps.map[map_id].threed_object_structure_len=elm_file.elm_header.threed_object_hash_len;

        //get 3d object count
        log_text(EVENT_MAP_LOAD, "3d object hash count [%i]", elm_file.elm_header.threed_object_count);
        maps.map[map_id].threed_object_count=elm_file.elm_header.threed_object_count;

        //log 3d object offset
        log_text(EVENT_MAP_LOAD, "3d object offset [%i]", elm_file.elm_header.threed_object_offset);

        //get 2d object hash length
        log_text(EVENT_MAP_LOAD, "2d object hash length [%i]", elm_file.elm_header.twod_object_hash_len);
        maps.map[map_id].twod_object_structure_len=elm_file.elm_header.twod_object_hash_len;

        //get 2d object count
        log_text(EVENT_MAP_LOAD, "2d object hash count [%i]", elm_file.elm_header.twod_object_count);
        maps.map[map_id].twod_object_count=elm_file.elm_header.twod_object_count;

        //log 2d object offset
        log_text(EVENT_MAP_LOAD, "2d object offset [%i]", elm_file.elm_header.twod_object_offset);

        //get lights object hash length
        log_text(EVENT_MAP_LOAD, "lights object hash length [%i]", elm_file.elm_header.lights_object_hash_len);
        maps.map[map_id].lights_object_structure_len=elm_file.elm_header.lights_object_hash_len;

        //get lights object count
        log_text(EVENT_MAP_LOAD, "lights object hash count [%i]", elm_file.elm_header.lights_object_count);
        maps.map[map_id].lights_object_count=elm_file.elm_header.lights_object_count;

        //log lights object offset
        log_text(EVENT_MAP_LOAD, "lights object offset [%i]", elm_file.elm_header.lights_object_offset);

        //log dungeon flag
        log_text(EVENT_MAP_LOAD, "dungeon flag [%i]", elm_file.elm_header.dungeon_flag);

        //log version flag
        log_text(EVENT_MAP_LOAD, "version flag [%i]", elm_file.elm_header.version_flag);

        if(elm_file.elm_header.version_flag!=ELM_FILE_VERSION){

            log_event(EVENT_ERROR, "file version flag is not correct for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        //log reserved bytes
        log_text(EVENT_MAP_LOAD, "reserved byte 1 [%i] reserved byte 2 [%i]", elm_file.elm_header.reserved1, elm_file.elm_header.reserved2);

        //log ambient colours
        log_text(EVENT_MAP_LOAD, "ambient red [%i]", elm_file.elm_header.ambient_red);
        log_text(EVENT_MAP_LOAD, "ambient green [%i]", elm_file.elm_header.ambient_green);
        log_text(EVENT_MAP_LOAD, "ambient blue [%i]", elm_file.elm_header.ambient_blue);

        //get particles object hash length
        log_text(EVENT_MAP_LOAD, "particles object hash length [%i]", elm_file.elm_header.particles_object_hash_len);

        //get particles object count
        log_text(EVENT_MAP_LOAD, "particles object hash count [%i]", elm_file.elm_header.particles_object_count);

        //log particles object offset
        log_text(EVENT_MAP_LOAD, "particles object offset [%i]", elm_file.elm_header.particles_object_offset);

        //log clusters offset
        log_text(EVENT_MAP_LOAD, "clusters object offset [%i]", elm_file.elm_header.clusters_offset);

        //log reserved integers
        log_text(EVENT_MAP_LOAD, "reserved 9 [%i]", elm_file.elm_header.reserved_9);
        log_text(EVENT_MAP_LOAD, "reserved 10 [%i]", elm_file.elm_header.reserved_10);
        log_text(EVENT_MAP_LOAD, "reserved 11 [%i]", elm_file.elm_header.reserved_11);
        log_text(EVENT_MAP_LOAD, "reserved 12 [%i]", elm_file.elm_header.reserved_12);
        log_text(EVENT_MAP_LOAD, "reserved 13 [%i]", elm_file.elm_header.reserved_13);
        log_text(EVENT_MAP_LOAD, "reserved 14 [%i]", elm_file.elm_header.reserved_14);
        log_text(EVENT_MAP_LOAD, "reserved 15 [%i]", elm_file.elm_header.reserved_15);
        log_text(EVENT_MAP_LOAD, "reserved 16 [%i]", elm_file.elm_header.reserved_16);
        log_text(EVENT_MAP_LOAD, "reserved 17 [%i]", elm_file.elm_header.reserved_17);

        //get tile map size
        maps.map[map_id].tile_map_size=elm_file.elm_header.height_map_offset-elm_file.elm_header.tile_map_offset;

        if(maps.map[map_id].height_map_size>TILE_MAP_MAX){

            log_event(EVENT_ERROR, "tile map exceeds maximum for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        log_text(EVENT_MAP_LOAD, "tile map size [%i]", maps.map[map_id].tile_map_size);

        //get height map size
        maps.map[map_id].height_map_size=elm_file.elm_header.threed_object_offset-elm_file.elm_header.height_map_offset;

        if(maps.map[map_id].height_map_size>HEIGHT_MAP_MAX){

            log_event(EVENT_ERROR, "height map exceeds maximum for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        log_text(EVENT_MAP_LOAD, "height map size [%i]", maps.map[map_id].height_map_size);

        //get 3d object map size
        maps.map[map_id].threed_object_map_size=elm_file.elm_header.twod_object_offset-elm_file.elm_header.threed_object_offset;

        if(maps.map[map_id].height_map_size>THREED_OBJECT_MAP_MAX){

            log_event(EVENT_ERROR, "3d object map exceeds maximum for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        log_text(EVENT_MAP_LOAD, "3d map size [%i]", maps.map[map_id].threed_object_map_size);

        //get 2d object map size
        maps.map[map_id].twod_object_map_size=elm_file.elm_header.lights_object_offset-elm_file.elm_header.twod_object_offset;

        if(maps.map[map_id].height_map_size>TWOD_OBJECT_MAP_MAX){

            log_event(EVENT_ERROR, "2d object map exceeds maximum for map [%i] [%s] in function %s: module %s: line %i", map_id, maps.map[map_id].elm_filename, __func__, __FILE__, __LINE__);
            stop_server();
        }

        log_text(EVENT_MAP_LOAD, "2d map size [%i]", maps.map[map_id].twod_object_map_size);

        //load tile map data
        k=0;
        for(j=elm_file.elm_header.tile_map_offset; j<elm_file.elm_header.height_map_offset; j++){

            maps.map[map_id].tile_map[k]=elm_file.byte[j];
            k++;
        }

        //load height map
        k=0;
        for(j=elm_file.elm_header.height_map_offset; j<elm_file.elm_header.threed_object_offset; j++){

            maps.map[map_id].height_map[k]=elm_file.byte[j];
            k++;
        }

        //load 3d object map
        k=0;
        for(j=elm_file.elm_header.threed_object_offset; j<elm_file.elm_header.twod_object_offset; j++){

            maps.map[map_id].threed_object_map[k]=elm_file.byte[j];
            k++;
        }

        //load 2d object map
        k=0;
        for(j=elm_file.elm_header.twod_object_offset; j<elm_file.elm_header.lights_object_offset; j++){

            maps.map[map_id].twod_object_map[k]=elm_file.byte[j];
            k++;
        }

        log_event(EVENT_INITIALISATION, "loaded [%i] [%s]", map_id, maps.map[map_id].map_name);

        i++;
    }

    //test that we were able to read all the rows in the query result
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //destroy the prepared sql statement
    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //return the number of query rows we were able to read
    return i;
}

int get_db_map_exists(int map_id){

    /** public function - see header */

    int rc=0;
    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT count(*) FROM MAP_TABLE WHERE MAP_ID=?");

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }
    rc = sqlite3_bind_int(stmt, 1, map_id);
    if(rc!=SQLITE_OK){
        log_sqlite_error("sqlite3_bind_int failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    while((rc = sqlite3_step(stmt))==SQLITE_ROW)
        ;
    if (rc != SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        stop_server();
    }

    int map_id_count=sqlite3_column_int(stmt, 0);
    if(map_id>1){

        log_event(EVENT_ERROR, "more than one entry in MAP_TABLE with id [%i]", map_id);
        stop_server();
    }

    sqlite3_finalize(stmt);
    if(rc!=SQLITE_DONE){

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    if(map_id_count==1) return TRUE;

    return FALSE;
}


void add_db_map(int map_id, char *map_name, char *elm_file_name){

    /** public function - see header */

    //doesn't use process_sql because map blob must be added

    int rc;
    sqlite3_stmt *stmt;
    FILE *file;
    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN,
    "INSERT INTO MAP_TABLE("  \
    "MAP_ID," \
    "MAP_NAME,"  \
    "ELM_FILE_NAME," \
    "ELM_FILE" \
    ") VALUES( ?, ?, ?, ?)");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //bind the data to be added to the map table
    sqlite3_bind_int(stmt, 1, map_id);
    sqlite3_bind_text(stmt, 2, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, elm_file_name, -1, SQLITE_STATIC);

    //find the size of the elm file
    int file_size=get_file_size(elm_file_name);

    if((file=fopen(elm_file_name, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //find the elm file size
    if(file_size>MAX_ELM_FILE_SIZE){

        log_event(EVENT_ERROR, "file [%s] exceeds max elm file size in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();

    }

    //read the elm file into a string
    unsigned byte[MAX_ELM_FILE_SIZE];

    if((file=fopen(elm_file_name, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(fread(byte, file_size, 1, file)!=1){

        log_event(EVENT_ERROR, "unable to read identification bytes for file [%s] in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    sqlite3_bind_blob(stmt, 4, byte, file_size, NULL);

    rc = sqlite3_step(stmt);

    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Map [%s] File Name [%s] added successfully\n", map_name, elm_file_name);

    log_event(EVENT_SESSION, "Added map [%s] to MAP_TABLE", map_name);
}

void update_db_map(int map_id, char *map_name, char *elm_file_name){

    /** public function - see header */

    //doesn't use process_sql because map blob must be added

    int rc;
    sqlite3_stmt *stmt;
    FILE *file;
    char sql[MAX_SQL_LEN]="";

    snprintf(sql, MAX_SQL_LEN,
    "UPDATE MAP_TABLE "  \
    "SET MAP_NAME=?,"  \
    "SET ELM_FILE_NAME=?," \
    "SET ELM_FILE=? " \
    "WHERE MAP_ID=?");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    //bind the data to be added to the map table
    sqlite3_bind_text(stmt, 1, map_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, elm_file_name, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, map_id);

    //find the size of the elm file
    int file_size=get_file_size(elm_file_name);

    if((file=fopen(elm_file_name, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //find the elm file size
    if(file_size>MAX_ELM_FILE_SIZE){

        log_event(EVENT_ERROR, "file [%s] exceeds max elm file size in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();

    }

    //read the elm file into a string
    unsigned byte[MAX_ELM_FILE_SIZE];

    if((file=fopen(elm_file_name, "r"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(fread(byte, file_size, 1, file)!=1){

        log_event(EVENT_ERROR, "unable to read identification bytes for file [%s] in function %s: module %s: line %i", elm_file_name, __func__, __FILE__, __LINE__);
        stop_server();
    }

    sqlite3_bind_blob(stmt, 3, byte, file_size, NULL);

    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_DONE) {

        log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK){

         log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    printf("Map [%s] File Name [%s] updated successfully\n", map_name, elm_file_name);

    log_event(EVENT_SESSION, "Updated map [%s] to MAP_TABLE", map_name);
}

