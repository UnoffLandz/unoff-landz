/***
	Copyright 2014 UnoffLandz

	This file is part of obj_conv.

	obj_conv is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	obj_conv is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with obj_conv.  If not, see <http://www.gnu.org/licenses/>.
***/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "files.h"
#include "create_e3d.h"

//#define DEBUG

int obj_conv(struct obj_type *obj, char *filename);

void print_usage() {

    printf("USAGE: obj_conv [options] [filename]\n");
    printf("\n");
	printf("[options]  - None\n");
	printf("\n");
	printf("[filename] - name of the e3d file to be converted\n");

	exit(EXIT_FAILURE);
}

int select_option(char option, struct obj_type *obj){

    switch(option){

		//case '-': break; //NOP

        default: printf("unknown command line option [%c]\n", option);
			return -1;
			break;
    }

	return 0;
}

int main(int argc, char *argv[]) {

   struct obj_type obj;

#ifdef DEBUG
    read_obj_data("house2.obj", &obj);
    create_e3d_file("house2.e3d", &obj);
    exit(1);
#endif

	printf("\nobj to e3d converter %s\n\n", VERSION);

	//parse the command line options
	int i;

    if(argc==1){

        print_usage();
		exit(EXIT_FAILURE);
    }

	for (i=1; i<argc; i++){

		// anything starting with a - is an option
		if (argv[i][0] == '-') {

			// check each character
			int j;
			int len = strlen(argv[i]);

			for(j=1; j<len; j++){

				if (select_option(argv[i][j], &obj) < 0) {
					print_usage();
					exit(EXIT_FAILURE);
				}
			}
		} else {

			// otherwise it's the name of a file to process
			obj_conv(&obj, argv[i]);
		}
	}

	return 0;
}

int obj_conv(struct obj_type *obj, char *filename){

	//check the file exists
	if (file_exists(filename)==FALSE){

		printf("can't find file [%s]\n", filename);
		exit(EXIT_FAILURE);
	}

    //the object data and create the e3d file
    read_obj_data(filename, obj);
    create_e3d_file(filename, obj);

	return 0;
}

