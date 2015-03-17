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

#include <stdio.h> //supports fopen
#include <sys/stat.h> //supports fstat

#include "global.h"


int get_file_size(char *file_name){

    /** public function - see header **/

    FILE *file;
    int count=0;

    if((file=fopen(file_name, "r"))==NULL) return NOT_FOUND;

    while(fgetc(file) != EOF) {
        count++;
    }

    fclose(file);

    return count;
}


int file_exists(const char *filename) {

    /** public function - see header **/

    struct stat st;

    if(stat(filename, &st)!=0) return FALSE;

    return TRUE;
}
