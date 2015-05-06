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
#include <stdlib.h>
#include <string.h>

#include "files.h"
#include "e3d.h"
#include "dds.h"

void create_obj_file(char *filename,
                     struct p_options_type *p_options,
                     struct e3d_header_type *e3d_header,
                     struct vertex_hash_type2 *vertex_hash2,
                     struct index_hash_type2 *index_hash2,
                     struct materials_hash_type2 *materials_hash2){

    FILE *file;
    int i,j;
    int start, count;

    char prefix[FILENAME_LEN]="";
	char obj_filename[FILENAME_LEN]="";
	char mtl_filename[FILENAME_LEN]="";

    get_filename_prefix(filename, prefix);
    sprintf(obj_filename, "%s.obj", prefix);

    //create the obj file
    if((file=fopen(obj_filename, "w"))==NULL) {

            printf("unable to create file [%s] in function create_obj_file\n", obj_filename);
            exit(EXIT_FAILURE);
    }

    //mark the file with the app and version number
    fprintf(file, "# file created by Unofflandz e3d_conv %s\n", VERSION);

    //write the mtl file link
    if(p_options->create_mtl==TRUE) {

        sprintf(mtl_filename, "%s.mtl", prefix);
        fprintf(file, "mtllib %s\n", mtl_filename);
    }

    //write the vertices
    fprintf(file, "\n# %i vertices\n", e3d_header->vertex_count);

    for(i=0; i<e3d_header->vertex_count; i++){

        fprintf(file, "v %f %f %f\n", vertex_hash2[i].vx, vertex_hash2[i].vy, vertex_hash2[i].vz);
    }

    //write the textures
    fprintf(file, "\n# %i textures\n", e3d_header->vertex_count);

    for(i=0; i<e3d_header->vertex_count; i++){

        if(p_options->reverse_y_axis==FALSE){

            fprintf(file, "vt %f %f\n", vertex_hash2[i].uvx, vertex_hash2[i].uvy);
        }
        else {
            fprintf(file, "vt %f %f\n", vertex_hash2[i].uvx, 1-vertex_hash2[i].uvy);
        }
    }

    //write the normals
    fprintf(file, "\n# %i normals\n", e3d_header->vertex_count);

    for(i=0; i<e3d_header->vertex_count; i++){

       fprintf(file, "vn %f %f %f\n", vertex_hash2[i].nx, vertex_hash2[i].ny, vertex_hash2[i].nz);
    }

    //write the faces
    fprintf(file, "\n# %i faces\n", e3d_header->index_count/3);

    for(i=0; i<e3d_header->material_count; i++){

        //we only need usemtl if we have a corresponding mtl file
        if(p_options->create_mtl==TRUE) fprintf(file, "usemtl %s\n", materials_hash2[i].name);

        //groups faces so that Unity correctly applies textures
        fprintf(file, "g %s\n", materials_hash2[i].name);

        //add smoothing
        fprintf(file, "s 1\n");

        start=materials_hash2[i].start;
        count=materials_hash2[i].count;

        for(j=start; j<start+count; j+=3){

            fprintf(file, "f %i/%i/%i %i/%i/%i %i/%i/%i\n",
               index_hash2[j+0].idx+1, index_hash2[j+0].idx+1, index_hash2[j+0].idx+1,
               index_hash2[j+1].idx+1, index_hash2[j+1].idx+1, index_hash2[j+1].idx+1,
               index_hash2[j+2].idx+1, index_hash2[j+2].idx+1, index_hash2[j+2].idx+1);
        }
     }

    //close the file
    fclose(file);
}

void create_mtl_file(char *filename, struct e3d_header_type *e3d_header, struct materials_hash_type2 *materials_hash2){

    /*** creates a materials (mtl) file ***/

    FILE *file;
    int i=0;
    int alpha=FALSE;
    char pf_fourcc[4]="";
    char prefix[FILENAME_LEN]="";
    char mtl_filename[FILENAME_LEN]="";

    get_filename_prefix(filename, prefix);
    sprintf(mtl_filename, "%s.mtl", prefix);


    //create the file
    if((file=fopen(mtl_filename, "w"))==NULL) {

            printf("unable to create file [%s] in function create_mtl_file\n", mtl_filename);
            exit(EXIT_FAILURE);
    }

    //mark the file with the app and version number
    fprintf(file, "# file created by Unofflandz e3d_conv %s\n", VERSION);

    for(i=0; i<e3d_header->material_count; i++){

        fprintf(file, "newmtl %s\n", materials_hash2[i].name);
        fprintf(file, "Ka 1.000000 1.000000 1.000000\n");
        fprintf(file, "Kd 0.640000 0.640000 0.640000\n");
        fprintf(file, "Ks 0.250000 0.250000 0.250000\n");
        fprintf(file, "Ni 1.000000\n");
        read_dds(materials_hash2[i].name, pf_fourcc);

        if(strcmp(pf_fourcc, "DXT1")==0) alpha=FALSE;
        if(strcmp(pf_fourcc, "DXT3")==0) alpha=TRUE;
        if(strcmp(pf_fourcc, "DXT5")==0) alpha=TRUE;

        if(alpha==TRUE) fprintf(file, "d 0.98\n");

        fprintf(file, "illum 2\n");
        fprintf(file, "Ns 0.312500\n");
        fprintf(file, "map_Kd %s\n", materials_hash2[i].name);
    }

    fclose(file);
}
