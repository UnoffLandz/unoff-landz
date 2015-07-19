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
#include <stdio.h> //support snprintf
#include <stdbool.h> // supports true/false
#include <stdlib.h> //support EXIT_FAILURE
#include <string.h> //support memcpy

#include "database_functions.h"
#include "../logging.h"
#include "../server_start_stop.h"
#include "../map_object.h"
#include "../maps.h"
#include "../file_functions.h"
#include "../string_functions.h"
#include "../numeric_functions.h"
#include "../e3d.h"

int load_db_map_objects(){

    /** public function - see header */

    int rc;
    sqlite3_stmt *stmt;

    //prepare the sql statement
    char sql[MAX_SQL_LEN]="";
    snprintf(sql, MAX_SQL_LEN, "SELECT * FROM MAP_OBJECT_TABLE");

    rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    log_event(EVENT_INITIALISATION, "loading map objects...");

    //read the sql query result into the map object array
    int i=0;

    while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        int id=sqlite3_column_int(stmt,0);
        int threedol_id=sqlite3_column_int(stmt, 1);
        int map_id=sqlite3_column_int(stmt, 2);

        map_object[threedol_id][map_id].tile=sqlite3_column_int(stmt, 3);
        map_object[threedol_id][map_id].e3d_id=sqlite3_column_int(stmt, 4);
        if(sqlite3_column_int(stmt, 5)==1) map_object[threedol_id][map_id].harvestable=true; else map_object[threedol_id][map_id].harvestable=false;
        map_object[threedol_id][map_id].reserve=sqlite3_column_int(stmt, 6);

        log_event(EVENT_INITIALISATION, "loaded [%i]", id);

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


void add_db_map_objects(char *elm_filename, int map_id){

    /** public function - see header */

    FILE *file;
    char sql[MAX_SQL_LEN]="";

    //read the elm file into a string
    unsigned char byte[MAX_ELM_FILE_SIZE]={0};

    //find the size of the elm file
    int file_size=get_file_size(elm_filename);

    if((file=fopen(elm_filename, "rb"))==NULL) {

        log_event(EVENT_ERROR, "unable to open file [%s] in %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    if(fread(&byte, file_size, 1, file)!=1){

        log_event(EVENT_ERROR, "unable to read file [%s] in function %s: module %s: line %i", elm_filename, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //close the file
    fclose(file);

    //load 3d object map
    int i=0;
    for(int j=maps.map[map_id].threed_object_offset; j<maps.map[map_id].twod_object_offset; j+=maps.map[map_id].threed_object_structure_len){

        //get object id
        char e3d_path_and_file_name[80]="";
        memcpy(e3d_path_and_file_name, &byte[j], 80);

        char e3d_filename[80]="";
        extract_file_name(e3d_path_and_file_name, e3d_filename);

        int e3d_id=get_e3d_id(e3d_filename);

        //get object position
        float x=Uint32_to_float(byte+j+80) * 2.00f;
        float y=Uint32_to_float(byte+j+84) * 2.00f;

        int tile=get_tile(x, y, map_id);

        int harvestable=0;
        int reserve=0;

        snprintf(sql, MAX_SQL_LEN, "INSERT INTO MAP_OBJECT_TABLE("  \
            "THREEDOL_ID," \
            "MAP_ID,"  \
            "TILE," \
            "E3D_ID," \
            "HARVESTABLE," \
            "RESERVE" \
            ") VALUES(%i, %i, %i, %i, %i, %i)", i, map_id, tile, e3d_id, harvestable, reserve);

        process_sql(sql);

        i++;

        printf("map_object [%i] added successfully\n", i);

        log_event(EVENT_SESSION, "Added map_object [%i] to MAP_OBJECT_TABLE", i);
    }
}
