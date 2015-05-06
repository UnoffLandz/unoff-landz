/***
    Copyright 2014 UnoffLandz

    This file is part of e3d_conv.

    e3d_conv is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    e3d_conv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with e3d_conv.  If not, see <http://www.gnu.org/licenses/>.
***/

#include <stdio.h>
//#include <errno.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include "files.h"
#include "e3d.h"
#include "obj.h"
#include "dds.h"

//#define DEBUG

int e3d_conv(struct e3d_header_type *e3d_header, struct p_options_type *p_options, char *filename);

void print_usage(){

	printf("USAGE: e3d_conv [options] [filename ...]\n");

	printf("[options]  - a = output diagnostics to screen (default)\n");
    printf("             A = output diagnostics to file\n");

    printf("             d = remove duplicate vertices\n");
	printf("             D = no not remove duplicate vertices (default)\n");

    printf("             e = include header data in diagnostics (default)\n");
	printf("             E = do not include header data in diagnostics\n");

    printf("             f = include vertex data in diagnostics\n");
	printf("             F = do not include vertex data in diagnostics (default)\n");

    printf("             g = include index data in diagnostics\n");
	printf("             G = do not include index data in diagnostics (default)\n");

    printf("             h = include materials data in diagnostics\n");
	printf("             H = do not include materials data in diagnostics (default)\n");

    printf("             i = include materials data in diagnostics\n");
	printf("             I = do not include materials data in diagnostics (default)\n");

	printf("             j = include duplicate vertex removal data in diagnostics\n");
	printf("             I = do not include duplicate vertex removal data in diagnostics (default)\n");

	printf("             m = create mtl file (default)\n");
	printf("             M = do not create mtl file\n");

	//printf("             n = calculate weighted face normals\n ");
	//printf("             N = do not calculate weighted face normals");

	printf("             r = convert file and output diagnostics \n");
	printf("             R = output diagnostics only (default)\n");

	printf("             s = output diagnostics (default)\n");
	printf("             S = no diagnostic output\n");

	printf("             v = verbose diagnostic output\n");
	printf("             V = summary diagnostic output (default)\n");

	printf("             y = reverse y axis of texture coordinates (default)\n");
	printf("             Y = do not reverse y axis of the texture coordinates\n");

	printf("[filename] - name of the e3d file to be converted\n");

	exit(EXIT_FAILURE);
}

int select_option(char option, struct p_options_type *p_options){

    switch(option){

		//case '-': break; //NOP

        case 'A': p_options->file_dump=FALSE; break;//done
        case 'a': p_options->file_dump=TRUE; break;//done

        case 'D': p_options->vertex_optimisation=FALSE; break;//done
        case 'd': p_options->vertex_optimisation=TRUE; break;//done

        case 'E': p_options->header_report=FALSE; break;//done
        case 'e': p_options->header_report=TRUE; break;//done

        case 'F': p_options->vertex_report=FALSE; break;//done
        case 'f': p_options->vertex_report=TRUE; break;//done

        case 'G': p_options->index_report=FALSE; break;//done
        case 'g': p_options->index_report=TRUE; break;//done

        case 'H': p_options->materials_report=FALSE; break;//done
        case 'h': p_options->materials_report=TRUE; break;//done

        case 'I': p_options->dds_report=FALSE; break;//done
        case 'i': p_options->dds_report=TRUE; break;//done

        case 'J': p_options->vertex_optimisation_report=FALSE; break;//done
        case 'j': p_options->vertex_optimisation_report=TRUE; break;//done

        case 'M': p_options->create_mtl=FALSE; break;//done
        case 'm': p_options->create_mtl=TRUE; break;//done

        //case 'N': p_options->calculate_normals=FALSE; break;
        //case 'n': p_options->calculate_normals=TRUE; break;

        case 'R': p_options->report_only=FALSE; break;//report
        case 'r': p_options->report_only=TRUE; break;

        case 'v': p_options->silent=FALSE; break;
        case 'V': p_options->silent=TRUE; break;
        case 'S': p_options->silent=FALSE; break;
        case 's': p_options->silent=TRUE; break;

        case 'Y': p_options->reverse_y_axis=FALSE; break;
        case 'y': p_options->reverse_y_axis=TRUE; break;

        default: printf("unknown command line option [%c]\n", option);
        return -1;
		break;
    }

	return 0;
}

int main(int argc, char *argv[]) {

    struct p_options_type p_options;
    struct e3d_header_type e3d_header;

	printf("\nE3D_CONV - Eternal Lands e3d to Wavefront obj file converter %s\n\n", VERSION);

#ifdef DEBUG

    //debugging code

    //int report=1;
    //read_mipmap_data("plants01.dds", report, REPORT_FILENAME);
    //exit(1);

    p_options.vertex_optimisation=TRUE;
    p_options.vertex_optimisation_report=TRUE;
    p_options.header_report=TRUE;
    p_options.vertex_report=FALSE;
    p_options.index_report=FALSE;
    p_options.materials_report=FALSE;
    p_options.dds_report=TRUE;
    p_options.report_only=FALSE;
    p_options.reverse_y_axis=TRUE;
    p_options.create_mtl=TRUE;
    p_options.silent=FALSE;
    p_options.file_dump=FALSE;

    e3d_conv(&e3d_header, &p_options, "house2.e3d");

#endif

#ifndef DEBUG

	//display the command line usage details
	if (argc<2){
		print_usage();
		exit(EXIT_FAILURE);
	}

	//set the default processing options
    p_options.vertex_optimisation=FALSE;
    p_options.vertex_optimisation_report=FALSE;
    p_options.header_report=TRUE;
    p_options.vertex_report=FALSE;
    p_options.index_report=FALSE;
    p_options.materials_report=FALSE;
    p_options.dds_report=FALSE;
    p_options.report_only=FALSE;
    p_options.reverse_y_axis=TRUE;
    p_options.create_mtl=TRUE;
    p_options.silent=FALSE;
    p_options.file_dump=FALSE;

	//parse the command line options (courtesy of Learner)
	int i=0;
	for (i=1; i<argc; i++){

		// anything starting with a - is an option
		if (argv[i][0] == '-') {

			// check each character
			int j;
			int len = strlen(argv[i]);
			for(j=1; j<len; j++){

				if (select_option(argv[i][j], &p_options) < 0) {

					print_usage();

					exit(EXIT_FAILURE);
				}
			}
		} else {

			// otherwise it's the name of a file to process
			e3d_conv(&e3d_header, &p_options, argv[i]);
		}
	}
#endif

	return 0;
}

int e3d_conv(struct e3d_header_type *e3d_header, struct p_options_type *p_options, char *filename){

    int vertex_optimisation=0;
    FILE *file_out;

    struct vertex_hash_type2    vertex_hash2[MAX_VERTEX];
    struct index_hash_type2     index_hash2[MAX_INDEX];
    struct materials_hash_type2 materials_hash2[MAX_MATERIAL];

	//check the file exists
	if (file_exists(filename)==FALSE){

		printf("can't find file [%s] in function e3d_conv\n", filename);
		exit(EXIT_FAILURE);
	}


    //read the e3d data
	read_e3d_header(filename, e3d_header);
    read_e3d_vertex_hash(filename, e3d_header->vertex_offset, e3d_header->vertex_count, vertex_hash2);
    read_e3d_index_hash(filename, e3d_header->index_offset, e3d_header->index_count, index_hash2);
    read_e3d_materials_hash(filename, e3d_header->material_offset, e3d_header->material_count, materials_hash2);

    //optimise the vertex data
    if (p_options->vertex_optimisation==TRUE){

        optimise_vertices(e3d_header->vertex_count, e3d_header->index_count, &vertex_optimisation, vertex_hash2, index_hash2);
        e3d_header->vertex_count-=vertex_optimisation;
    }

    //create the obj and mtl files
    if(p_options->report_only==FALSE) {

        create_obj_file(filename, p_options, e3d_header, vertex_hash2, index_hash2, materials_hash2);

        if(p_options->create_mtl==TRUE) create_mtl_file(filename, e3d_header, materials_hash2);
    }

    //set the reporting options
    if(p_options->silent==TRUE) {

        //do nothing
    }
    else {

        switch(p_options->file_dump){

            case FALSE://screen dump

            if(p_options->header_report==TRUE) report_e3d_header_data(filename, 1, "", e3d_header);
            if(p_options->vertex_optimisation_report==TRUE) report_vertex_optimisation(1, "", vertex_optimisation, e3d_header->vertex_count);
            if(p_options->vertex_report==TRUE) report_vertex_data(1, "", e3d_header->vertex_count, vertex_hash2);
            if(p_options->index_report==TRUE) report_index_data(1, "", e3d_header->index_count, index_hash2);
            if(p_options->materials_report==TRUE) report_materials_data(1, "", e3d_header->material_count, materials_hash2);
            if(p_options->dds_report==TRUE) report_dds_data(1, "", e3d_header->material_count, materials_hash2);
            break;

            case TRUE://file dump

            //clears any existing report file
            if((file_out=fopen(REPORT_FILENAME, "w"))==NULL) {

                printf("can't create file [%s] in function e3d_conv\n", REPORT_FILENAME);
                exit(EXIT_FAILURE);
            }

            fclose(file_out);

            if(p_options->header_report==TRUE) report_e3d_header_data(filename, 2, REPORT_FILENAME, e3d_header);
            if(p_options->vertex_optimisation_report==TRUE) report_vertex_optimisation(2, REPORT_FILENAME, vertex_optimisation, e3d_header->vertex_count);
            if(p_options->vertex_report==TRUE) report_vertex_data(2, REPORT_FILENAME, e3d_header->vertex_count, vertex_hash2);
            if(p_options->index_report==TRUE) report_index_data(2, REPORT_FILENAME, e3d_header->index_count, index_hash2);
            if(p_options->materials_report==TRUE) report_materials_data(2, REPORT_FILENAME, e3d_header->material_count, materials_hash2);
            if(p_options->dds_report==TRUE) report_dds_data(2, REPORT_FILENAME, e3d_header->material_count, materials_hash2);
            break;

            default://unknown
            printf("unknown option in function e3d_conv\n");
            exit(EXIT_FAILURE);
        }
    }

	return 0;
}

