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
#include <stdio.h> //support snprintf
#include <stdbool.h> // supports bool data type
#include <stdlib.h> //support EXIT_FAILURE
#include <string.h> //support memcpy

#include "database_functions.h"
#include "db_e3d_tbl.h"
#include "db_object_tbl.h"
#include "db_map_tbl.h"

#include "../logging.h"
#include "../server_start_stop.h"
#include "../maps.h"
#include "../file_functions.h"
#include "../string_functions.h"
#include "../numeric_functions.h"
#include "../e3d.h"
#include "../objects.h"

void load_db_map_objects(){

    /** public function - see header */

    log_event(EVENT_INITIALISATION, "loading map objects...");

    sqlite3_stmt *stmt;

    char sql[MAX_SQL_LEN]="SELECT * FROM MAP_OBJECT_TABLE";

    //check database table exists
    char database_table[80];
    strcpy(database_table, strstr(sql, "FROM")+5);
    if(table_exists(database_table)==false){

        log_event(EVENT_ERROR, "table [%s] not found in database", database_table);
        stop_server();
    }

    //prepare the sql statement
    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

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
        strcpy( map_object[threedol_id][map_id].e3d_filename, (char*)sqlite3_column_text(stmt, 7));

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

    if(i==0){

        log_event(EVENT_ERROR, "no map objects found in database", i);
        stop_server();
    }
}


void add_db_map_objects(int map_id, char *elm_filename){

    /** public function - see header */

    //load e3d and object data, otherwise we'll be unable to populate the map object entries
    //with links
    load_db_e3ds();

    //read the 3d object list into the array
    read_threed_object_list(elm_filename);

    //get the map axis
    read_elm_header(elm_filename);

    //add the array data to the database
    sqlite3_stmt *stmt;
    char *sErrMsg = 0;

    //as there's likely to be thousands of entries that take an age to load, create a sql statement to which
    //values can be replaced within TRANSACTION
    char sql[MAX_SQL_LEN]="INSERT INTO MAP_OBJECT_TABLE("  \
         "THREEDOL_ID," \
         "MAP_ID,"  \
         "TILE," \
         "E3D_ID," \
         "HARVESTABLE," \
         "RESERVE," \
         "E3D_FILENAME"
         ") VALUES(?, ?, ?, ?, ?, ?, ?)";


    int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
    if(rc!=SQLITE_OK){

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    int reserve=0;

    for(int i=0; i<elm_header.threed_object_count; i++) {

        sqlite3_bind_int(stmt, 1, i);
        sqlite3_bind_int(stmt, 2, map_id);

        //get map_tile from threed object list
        float x_pos=threed_object_list[i].x_pos * 2.00f;
        float y_pos=threed_object_list[i].y_pos * 2.00f;
        int tile=(int)x_pos + ((int)y_pos * elm_header.h_tiles * STEP_TILE_RATIO);
        sqlite3_bind_int(stmt, 3, tile);

        //get e3d id from threed object list
        char e3d_filename[80]="";
        strcpy(e3d_filename, strrchr(threed_object_list[i].e3d_path_and_filename, '/')+1 );
        int e3d_id=get_e3d_id(e3d_filename);
        sqlite3_bind_int(stmt, 4, e3d_id);

        //get whether object is harvestable from threed object list
        int object_id=e3d[e3d_id].object_id;
        bool harvestable=object[object_id].harvestable;
        sqlite3_bind_int(stmt, 5, harvestable);

        sqlite3_bind_int(stmt, 6, reserve);

        sqlite3_bind_text(stmt, 7, e3d_filename, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc!= SQLITE_DONE) {

            log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
        }

       sqlite3_clear_bindings(stmt);
       sqlite3_reset(stmt);
    }

    rc=sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &sErrMsg);
    if (rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_exec failed", __func__, __FILE__, __LINE__, rc, sql);
    }

    rc=sqlite3_finalize(stmt);
    if(rc!=SQLITE_OK) {

        log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
    }
}

void update_db_map_objects(int map_id){

    /** public function - see header */

    load_db_maps();//required for threed object count
    load_db_map_objects();
    load_db_e3ds();

    sqlite3_stmt *stmt;

    int threed_object_count=maps.map[map_id].threed_object_count;

    for(int i=0; i<threed_object_count; i++) {

        //get e3d id from threed object list
        int e3d_id=get_e3d_id(map_object[i][map_id].e3d_filename);

        if(e3d_id!=map_object[i][map_id].e3d_id){

            // TODO (themuntdregger#1#): add logging

            char sql[MAX_SQL_LEN]="";
            sprintf(sql, "UPDATE MAP_OBJECT_TABLE SET E3D_ID=%i WHERE MAP_ID=%i AND THREEDOL_ID=%i", e3d_id, map_id, i);

            int rc=sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if(rc!=SQLITE_OK) {

                log_sqlite_error("sqlite3_prepare_v2 failed", __func__, __FILE__, __LINE__, rc, sql);
            }

            rc = sqlite3_step(stmt);
            if (rc!= SQLITE_DONE) {

                log_sqlite_error("sqlite3_step failed", __func__, __FILE__, __LINE__, rc, sql);
            }

            rc=sqlite3_finalize(stmt);
            if(rc!=SQLITE_OK) {

                log_sqlite_error("sqlite3_finalize failed", __func__, __FILE__, __LINE__, rc, sql);
            }
        }
    }
}


void batch_update_map_objects(char *file_name){

    /** public function - see header */

    FILE* file;

    if((file=fopen(file_name, "r"))==NULL){

        log_event(EVENT_ERROR, "map load file [%s] not found", file_name);
        exit(EXIT_FAILURE);
    }

    char line[160]="";
    int line_counter=0;

    printf("\n");

    while (fgets(line, sizeof(line), file)) {

        line_counter++;

        sscanf(line, "%*s");

        char output[8][80];
        memset(&output, 0, sizeof(output));
        parse_line(line, output);

        int map_id=atoi(output[0]);

        update_db_map_objects(map_id);
      }

    fclose(file);
}
