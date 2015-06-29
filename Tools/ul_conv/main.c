#include <stdio.h>

#include "e3d.h"
#include "obj.h"
#include "processing_options.h"
#include "common.h"

/** OPENING BRIEF FOR PROJECT

    A single application with functionality for:

    -converting e3d to obj format -
    -converting obj to e3d format -
    -e3d/obj diagnostics          - Done
    -refining e3d data            -

    CURRENT TASK - testing

    TESTING:
    command line with no options or filename    - prints usage
    command line with e3d filename              - prints usage
    command line with obj filename              - prints usage
    command line with -X                        - prints unknown command line option
    command line with -X and e3d filename       - prints unknown command line option
    command line with -X and obj filename       - prints unknown command line option
    command line with -h                        - prints usage
    command line with -H                        - prints usage
    command line with -D but no filename        - prints missing filename
    command line with -D sewer_wall1.e3d        - prints diagnostics (e3d full stats)
    command line with -D sewer_wall1.obj        - prints diagnostics (obj stats)
    command line with -C house2.e3d             - works
    command line with -C house2.obj             - works


    TO DO:
    add licensing to modules
    dds viewer

**/

void print_usage(){

	printf("USAGE: ul_conv [options] [filename ...]\n");

	printf("\n[options]  -c = convert [filename]\n");
	printf("           -d = diagnostics only\n");
	printf("           -h = help\n");

	printf("\nsupported file types:\n");
	printf("          .e3d - Eternal Lands\n");
	printf("          .obj - Wavefront\n");
}

int main(int argc, char *argv[]){

	printf("UL_CONV - Unofflandz e3d graphics tool - version %s\n", VERSION);

    parse_command_line(argc, argv);

    //extract the filename suffix so we can auto-detect the file type when processing files
    char suffix[4]; /** TO DO replace hardcoded value **/
    get_filename_suffix(p_options.filename, suffix);

    //process command line options
    if(p_options.help==true || (p_options.convert==false && p_options.diagnostics_only==false)){

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
    else {

        printf("unknown command line option\n");
        print_usage();
        exit(EXIT_FAILURE);
    }

	return EXIT_SUCCESS;
}
