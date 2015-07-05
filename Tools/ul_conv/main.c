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
*******************************************************************************************************************
                                    COMPILER SETTINGS

To compile, set the following compiler flags :

    -std=c99                - target c99 compliance

                                    LINKING INFORMATION

To compile, link with the following libraries :

    libcrypto.a             - openssl library

*******************************************************************************************************************/

#include <stdio.h>
//#include <GL/glew.h>
//#include <GL/freeglut_std.h>

#include "e3d.h"
#include "obj.h"
#include "processing_options.h"
#include "common.h"
//#include "dds.h"

void print_usage(){

	printf("USAGE: ul_conv [options] [filename ...]\n");

	printf("\n[options]  -c = convert [filename]\n");
	printf("           -d = diagnostics only\n");
	printf("           -h = help\n");
	//printf("           -t = display texture\n");

	printf("\nsupported file types:\n");
	printf("          .e3d - Eternal Lands\n");
	printf("          .obj - Wavefront\n");
	//printf("          .dds - textures\n");
}

int main(int argc, char *argv[]){

	printf("UL_CONV - Unofflandz e3d graphics tool - version %s\n", VERSION);

    parse_command_line(argc, argv);

    //extract the filename suffix so we can auto-detect the file type when processing files
    char suffix[4]; /** TO DO replace hardcoded value **/
    get_filename_suffix(p_options.filename, suffix);

    //process command line options
    if(p_options.help==true){

        print_usage();
    }

    else if(p_options.convert==true){

        if(strcmp(suffix, ".e3d")==0){

            convert_e3d_to_obj_file();
        }
        else if(strcmp(suffix, ".obj")==0){

            convert_obj_to_e3d_file();
        }
        else if(strcmp(suffix, "")==0 && strcmp(p_options.filename, "")==0){

            printf("missing filename\n");
            exit(EXIT_FAILURE);
        }
        else {

            printf("conversion for file type [%s] is not supported\n", p_options.filename);
            exit(EXIT_FAILURE);
        }
    }

    else if(p_options.diagnostics_only==true && p_options.convert==false){

        if(strcmp(suffix, ".e3d")==0){

            //perform diagnostics on an e3d file
            read_e3d_header(p_options.filename);
            read_e3d_vertex_hash(p_options.filename, e3d_header.vertex_offset, e3d_header.vertex_count);
            read_e3d_index_hash(p_options.filename, e3d_header.index_offset, e3d_header.index_count);
            read_e3d_materials_hash(p_options.filename, e3d_header.material_offset, e3d_header.material_count);

            report_e3d_data();
        }
        else if(strcmp(suffix, ".obj")==0){

            //perform diagnostics on an obj file
            read_obj_data(p_options.filename);
            report_obj_stats();
        }
        else if(strcmp(suffix, "")==0){

            printf("missing filename\n");
            exit(EXIT_FAILURE);
        }
        else {

            printf("diagnostics for file type [%s] is not supported\n", suffix);
            exit(EXIT_FAILURE);
        }
    }

/*
    else if(p_options.dds==true){

        if(strcmp(suffix, ".dds")==0){

            display_texture();

        }

        else {

            printf("display of file type [%s] is not supported\n", p_options.filename);
            exit(EXIT_FAILURE);
        }
    }
*/
    else {

            printf("unknown command line option\n");
            print_usage();
            exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
