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

#include <stdio.h> //supports fopen function
#include <sys/stat.h> //supports fstat function
#include <stdbool.h> //supports true/false data type
#include <string.h> //supports strlen and strcpy functions

#include "logging.h"
#include "server_start_stop.h"

int get_file_size(char *file_name){

    /** public function - see header **/

    FILE *file;
    int count=0;

    if((file=fopen(file_name, "r"))==NULL){

        return -1;
    }

    while(fgetc(file) != EOF) {
        count++;
    }

    fclose(file);

    return count;
}


bool file_exists(const char *filename) {

    /** public function - see header **/

    struct stat st;

    if(stat(filename, &st)!=0) return false;

    return true;
}


int fcopy(const char *dbname, const char *newdbname) {

    FILE *fsrc = fopen(dbname,"r");
    if(!fsrc) {
        return -1;
    }

    FILE *ftgt = fopen(newdbname,"w");
    if(!ftgt) {
        fclose(fsrc);
        return -1;
    }

    int result = 0;
    size_t sz;
    char buf[1024];
    while((sz=fread(buf,1,1024,fsrc))==1024) {
        if(sz!=fwrite(buf,1,sz,ftgt)) {
            result = -1;
            goto FIN;
        }
        if(ferror(fsrc)||ferror(ftgt)) {
            result = -1;
            goto FIN;
        }
    }
    if(sz>0 && (sz!=fwrite(buf,1,sz,ftgt))) {
        result = -1;
        goto FIN;
    }
FIN:
    fclose(fsrc);
    fclose(ftgt);
    return result;
}


void extract_filename_suffix(const char *str_in, char *str_out){

    /** public function - see header */

    for(size_t i=0; i<strlen(str_in); i++){

        if(str_in[i]=='.'){

            strncpy(str_out, str_in+i, strlen(str_in)-i);
            return;
        }
    }

    strcpy(str_out, "");
}


void extract_filename_prefix(const char *str_in, char *str_out){

    /** public function - see header */

    for(size_t i=0; i<strlen(str_in); i++){

        if(str_in[i]=='.'){

            strncpy(str_out, str_in, i);
            return;
        }
    }

    strcpy(str_out, str_in);
}


void create_backup_file(const char *existing_filename) {

    /** public function - see header **/

    char prefix[80]="";
    int version=0;
    char filename_bck[80]="";

    extract_filename_prefix(existing_filename, prefix);
    sprintf(filename_bck, "%s.%d", prefix, version);

    while(file_exists(filename_bck)==true){

        version++;
        sprintf(filename_bck, "%s.%d", prefix, version);
    }

    fcopy(existing_filename, filename_bck);
}
