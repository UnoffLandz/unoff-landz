#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "processing_options.h"

void parse_command_line(int argc, char *argv[]){

    //set the default processing options
    p_options.convert=false;
    p_options.diagnostics_only=false;
    p_options.help=false;

	for (int i=1; i<argc; i++){

        if(argv[i][0] == '-') {

			//anything commencing with '-' is a processing option
			for(int j=1; j<(int)strlen(argv[i]); j++){

                switch(tolower(argv[i][j])){

                    case 'c':   p_options.convert=true; break;
                    case 'd':   p_options.diagnostics_only=true; break;
                    case 'h':   p_options.help=true; break;

                    default:
                    printf("\nunknown command line option [%c]\n\n", argv[i][j]);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else {

            //anything not commencing with '-' is a filename
            strcpy(p_options.filename, argv[i]);
 		}
	}
}
