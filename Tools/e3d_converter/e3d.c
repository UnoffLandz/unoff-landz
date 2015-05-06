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
//#include <openssl/md5.h>        //required for checking the md5 hash in the e3d file

#include "half.h"
#include "normal.h"
#include "files.h"
#include "e3d.h"

void read_e3d_header(char *filename, struct e3d_header_type *e3d_header){

    FILE *file;

    //create a struct to read the file into (can't read directly into the struct passed through the function prototype)
    struct e3d_header_type eh;

    //open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_header\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the file
    if(fread(&eh, sizeof(eh), 1, file)!=1){

        printf("unable to read data from file [%s] in function read_e3d_header\n", filename);
        exit(EXIT_FAILURE);;
    }

    //close the file
    fclose(file);

    //transfer data to struct passed through function prototype
    *e3d_header=eh;
}

void read_e3d_vertex_hash(char *filename, int offset, int vertex_count, struct vertex_hash_type2 *vertex_hash2){

    unsigned char byte[offset];
    float normal[3];
    int i=0;
    FILE *file;

    struct {
        Uint16 uvx;
        Uint16 uvy;
        Uint16 n;
        Uint16 t;
        Uint16 vz;
        Uint16 vx;
        Uint16 vy;
    }vh;

    // open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_vertex_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read data from file [%s] in function read_e3d_vertex_hash\n", filename);
        exit(EXIT_FAILURE);;
    }

    //read the hash
    for(i=0; i<vertex_count; i++){

        if(fread(&vh, sizeof(vh), 1, file)!=1){

            printf("unable to read data from file [%s] in function read_e3d_vertex_hash\n", filename);
            exit(EXIT_FAILURE);;
        }

        vertex_hash2[i].uvx=half_to_float(vh.uvx);
        vertex_hash2[i].uvy=half_to_float(vh.uvy);

        uncompress_normal(half_to_float(vh.n), normal);
        vertex_hash2[i].nx=normal[0];
        vertex_hash2[i].ny=normal[1];
        vertex_hash2[i].nz=normal[2];

        vertex_hash2[i].t=half_to_float(vh.t);

        vertex_hash2[i].vz=half_to_float(vh.vz);
        vertex_hash2[i].vx=half_to_float(vh.vx);
        vertex_hash2[i].vy=half_to_float(vh.vy);
    }

    fclose(file);
}

void read_e3d_index_hash(char *filename, int offset, int index_count, struct index_hash_type2 *index_hash2){

    unsigned char byte[offset];
    int i=0;
    FILE *file;

    struct {
        Uint16 idx;
    }ih;

    // open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_index_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read data from file [%s] in function read_e3d_index_hash\n", filename);
        exit(EXIT_FAILURE);;
    }

    //read the hash
    for(i=0; i<index_count; i++){

        if(fread(&ih, sizeof(ih), 1, file)!=1){

            printf("unable to read data from file [%s] in function file_analysis\n", filename);
            exit(EXIT_FAILURE);;
        }

        index_hash2[i].idx=ih.idx;
    }

    fclose(file);
}

void read_e3d_materials_hash(char *filename, int offset, int materials_count, struct materials_hash_type2 *materials_hash2){

    unsigned char byte[offset];
    int i=0;
    FILE *file;

    struct materials_hash_type2 mh;

    // open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_index_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read data from file [%s] in function read_e3d_materials_hash\n", filename);
        exit(EXIT_FAILURE);;
    }

    for(i=0; i<materials_count; i++){

        if(fread(&mh, sizeof(mh), 1, file)!=1){

            printf("unable to read data from file [%s] in function read_e3d_materials_hash\n", filename);
            exit(EXIT_FAILURE);;
        }

        materials_hash2[i].options=mh.options;
        strcpy(materials_hash2[i].name, mh.name);
        materials_hash2[i].minx=mh.minx;
        materials_hash2[i].miny=mh.miny;
        materials_hash2[i].minz=mh.minz;
        materials_hash2[i].maxx=mh.maxx;
        materials_hash2[i].maxy=mh.maxy;
        materials_hash2[i].maxz=mh.maxz;
        materials_hash2[i].minidx=mh.minidx;
        materials_hash2[i].maxidx=mh.maxidx;
        materials_hash2[i].start=mh.start;
        materials_hash2[i].count=mh.count;
    }

    fclose(file);
}

void optimise_vertices(int vertex_count, int index_count, int *optimisation, struct vertex_hash_type2 *vertex_hash2, struct index_hash_type2 *index_hash2){

    int i=0, j=0, k=0, l=0;
    int dup=FALSE;

    for(i=0; i<vertex_count; i++){

        //check the preceding vertices to see if they duplicate the current one
        dup=FALSE;

        for(j=0; j<i; j++){

            if(
                (vertex_hash2[j].vz==vertex_hash2[i].vz) &&
                (vertex_hash2[j].vy==vertex_hash2[i].vy) &&
                (vertex_hash2[j].vx==vertex_hash2[i].vx) &&
                (vertex_hash2[j].uvx==vertex_hash2[i].uvx) &&
                (vertex_hash2[j].uvy==vertex_hash2[i].uvy)
               ){

                k++;

                for(l=0; l<index_count; l++){

                    if(index_hash2[l].idx==i) index_hash2[l].idx=j;
                }

                dup=TRUE;
                break;
            }
        }

        if(dup==FALSE){

            vertex_hash2[i-k].vz=vertex_hash2[i].vz;
            vertex_hash2[i-k].vy=vertex_hash2[i].vy;
            vertex_hash2[i-k].vx=vertex_hash2[i].vx;

            vertex_hash2[i-k].uvx=vertex_hash2[i].uvx;
            vertex_hash2[i-k].uvy=vertex_hash2[i].uvy;

            vertex_hash2[i-k].nx=vertex_hash2[i].nx;
            vertex_hash2[i-k].ny=vertex_hash2[i].ny;
            vertex_hash2[i-k].nz=vertex_hash2[i].nz;

            for(l=0; l<index_count; l++){

                if(index_hash2[l].idx==i) index_hash2[l].idx=i-k;

            }
        }
    }

    *optimisation=k;
/*
    switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("\nVertex optimisation - %i\n", k);
        printf("Post optimisation report...\n");
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function optimise_vertices\n", report_filename);
            exit(EXIT_FAILURE);
        }
        fprintf(file_out, "\nVertex optimisation - %i\n", k);
        fprintf(file_out, "Post optimisation report...\n");

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function optimise_vertices\n");
        exit(EXIT_FAILURE);
    }

    //print post optimisation reports
    report_vertex_data(report, REPORT_FILENAME, vertex_count, vertex_hash2);
    report_index_data(report, REPORT_FILENAME, index_count, index_hash2);
*/
}

void report_vertex_optimisation(int report, char *report_filename, int optimisation, int vertex_count){

   FILE *file_out;

   switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("\nvertex optimisation            [%i] [%i%%]\n", optimisation, optimisation*100/vertex_count);
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function report_vertex_optimisation\n", report_filename);
            exit(EXIT_FAILURE);
        }

        fprintf(file_out, "/nvertex optimisation          [%i]\n", optimisation);

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function read_e3d_header\n");
        exit(EXIT_FAILURE);
   }
}

void report_e3d_header_data(char *filename, int report, char *report_filename, struct e3d_header_type *e3d_header){

   FILE *file_out;
   int i=0;

   switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("E3D file header...\n");
        printf("file name                      %s\n", filename);

        printf("magic number                   %c %c %c %c\n", e3d_header->magic_number[0], e3d_header->magic_number[1],  e3d_header->magic_number[2],  e3d_header->magic_number[3]);

        printf("version info                   %i %i %i %i\n", e3d_header->version[0], e3d_header->version[1],  e3d_header->version[2],  e3d_header->version[3]);
        printf("MD5 hash        "); for(i=0; i<E3D_MD5_HASH_LEN; i++) printf("%i ", e3d_header->md5_hash[i]); printf("\n");
        printf("header offset                  %i\n", e3d_header->header_offset);

        printf("\n");

        printf("vertex count                   %i\n", e3d_header->vertex_count);
        printf("vertex length                  %i\n", e3d_header->vertex_size);
        printf("vertex offset                  %i\n", e3d_header->vertex_offset);

        printf("\n");

        printf("index count                    %i\n", e3d_header->index_count);
        printf("index length                   %i\n", e3d_header->index_size);
        printf("index offset                   %i\n", e3d_header->index_offset);

        printf("\n");

        printf("material count                 %i\n", e3d_header->material_count);
        printf("material length                %i\n", e3d_header->material_size);
        printf("material offset                %i\n", e3d_header->material_offset);

        printf("\n");

        printf("option flag normals            "); if(e3d_header->options_flag & 0x01) printf("X\n"); else printf("-\n");
        printf("option flag tangents           "); if(e3d_header->options_flag & 0x02) printf("X\n"); else printf("-\n");
        printf("option flag extra uv           "); if(e3d_header->options_flag & 0x04) printf("X\n"); else printf("-\n");
        printf("option flag colour             "); if(e3d_header->options_flag & 0x08) printf("X\n"); else printf("-\n");

        printf("\n");

        printf("format flag half position      "); if(e3d_header->format_flag & 0x01) printf("X\n"); else printf("-\n");
        printf("format flag half uv            "); if(e3d_header->format_flag & 0x02) printf("X\n"); else printf("-\n");
        printf("format flag half extra uv      "); if(e3d_header->format_flag & 0x04) printf("X\n"); else printf("-\n");
        printf("format flag compressed normals "); if(e3d_header->format_flag & 0x08) printf("X\n"); else printf("-\n");
        printf("format flag short index        "); if(e3d_header->format_flag & 0x10) printf("X\n"); else printf("-\n");
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function read_e3d_header\n", report_filename);
            exit(EXIT_FAILURE);
        }

        fprintf(file_out, "E3D file header...\n");
        fprintf(file_out, "file name                      %s\n", filename);

        fprintf(file_out, "magic number                   %c %c %c %c\n", e3d_header->magic_number[0], e3d_header->magic_number[1],  e3d_header->magic_number[2],  e3d_header->magic_number[3]);

        fprintf(file_out, "version info                   %i %i %i %i\n", e3d_header->version[0], e3d_header->version[1],  e3d_header->version[2],  e3d_header->version[3]);
        fprintf(file_out, "MD5 hash        "); for(i=0; i<E3D_MD5_HASH_LEN; i++) fprintf(file_out, "%i ", e3d_header->md5_hash[i]); fprintf(file_out, "\n");
        fprintf(file_out, "header offset                  %i\n", e3d_header->header_offset);

        fprintf(file_out, "\n");

        fprintf(file_out, "vertex count                   %i\n", e3d_header->vertex_count);
        fprintf(file_out, "vertex length                  %i\n", e3d_header->vertex_size);
        fprintf(file_out, "vertex offset                  %i\n", e3d_header->vertex_offset);

        fprintf(file_out, "\n");

        fprintf(file_out, "index count                    %i\n", e3d_header->index_count);
        fprintf(file_out, "index length                   %i\n", e3d_header->index_size);
        fprintf(file_out, "index offset                   %i\n", e3d_header->index_offset);

        fprintf(file_out, "\n");

        fprintf(file_out, "material count                 %i\n", e3d_header->material_count);
        fprintf(file_out, "material length                %i\n", e3d_header->material_size);
        fprintf(file_out, "material offset                %i\n", e3d_header->material_offset);

        fprintf(file_out, "\n");

        fprintf(file_out, "option flag normals            "); if(e3d_header->options_flag & 0x01) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "option flag tangents           "); if(e3d_header->options_flag & 0x02) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "option flag extra uv           "); if(e3d_header->options_flag & 0x04) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "option flag colour             "); if(e3d_header->options_flag & 0x08) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

        fprintf(file_out, "\n");

        fprintf(file_out, "format flag half position      "); if(e3d_header->format_flag & 0x01) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "format flag half uv            "); if(e3d_header->format_flag & 0x02) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "format flag half extra uv      "); if(e3d_header->format_flag & 0x04) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "format flag compressed normals "); if(e3d_header->format_flag & 0x08) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
        fprintf(file_out, "format flag short index        "); if(e3d_header->format_flag & 0x10) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function read_e3d_header\n");
        exit(EXIT_FAILURE);
    }
}

void report_vertex_data(int report, char *report_filename, int vertex_count, struct vertex_hash_type2 *vertex_hash2){

    int i=0;
    FILE *file_out;

     switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("\nVertex hash data...\n");
        printf("[ count]  uvx       uvy       nx        ny        nz        t         vz        vx        vy\n");

        for(i=0; i<vertex_count; i++){

             printf("[%6i] %9f %9f %9f %9f %9f %9f %9f %9f %9f\n",
                    i,
                    vertex_hash2[i].uvx,
                    vertex_hash2[i].uvy,
                    vertex_hash2[i].nx,
                    vertex_hash2[i].ny,
                    vertex_hash2[i].nz,
                    vertex_hash2[i].t,
                    vertex_hash2[i].vz,
                    vertex_hash2[i].vx,
                    vertex_hash2[i].vy
                    );
        }
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function report_vertex_data\n", report_filename);
            exit(EXIT_FAILURE);
        }

        fprintf(file_out, "\nVertex hash data...\n");
        fprintf(file_out, "[ count]  uvx       uvy       nx        ny        nz        t         vz        vx        vy\n");

        for(i=0; i<vertex_count; i++){

             fprintf(file_out, "[%6i] %9f %9f %9f %9f %9f %9f %9f %9f %9f\n",
                    i,
                    vertex_hash2[i].uvx,
                    vertex_hash2[i].uvy,
                    vertex_hash2[i].nx,
                    vertex_hash2[i].ny,
                    vertex_hash2[i].nz,
                    vertex_hash2[i].t,
                    vertex_hash2[i].vz,
                    vertex_hash2[i].vx,
                    vertex_hash2[i].vy
                    );
        }

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function report_vertex_data\n");
        exit(EXIT_FAILURE);
    }
}

void report_index_data(int report, char *report_filename, int index_count, struct index_hash_type2 *index_hash2){

    int i=0;
    FILE *file_out;

    //print report
    switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("\nIndex hash data...\n");
        printf("Count     Entry\n");

        for(i=0; i<index_count; i++){

            printf("[%6i] %6i\n", i, index_hash2[i].idx);
        }
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function report_index_data\n", report_filename);
            exit(EXIT_FAILURE);
        }

        fprintf(file_out, "\nIndex hash data...\n");
        fprintf(file_out, "Count     Entry\n");

        for(i=0; i<index_count; i++){

            fprintf(file_out, "[%6i] %6i\n", i, index_hash2[i].idx);
        }

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function report_index_data\n");
        exit(EXIT_FAILURE);
    }
}

void report_materials_data(int report, char *report_filename, int material_count, struct materials_hash_type2 *materials_hash2){

    int i=0;
    FILE *file_out;

    //print report
    switch(report){

        case 0:
        //do nothing
        break;

        case 1:
        printf("\nMaterial hash data...\n");
        printf("Options name                minx      miny      minz      maxx      maxy      maxz      minidx  maxidx   start   count\n");

        for(i=0; i<material_count; i++){

            printf("%7i %18s %9f %9f %9f %9f %9f %9f %7i %7i %7i %7i\n",
                    materials_hash2[i].options,
                    materials_hash2[i].name,
                    materials_hash2[i].minx,
                    materials_hash2[i].miny,
                    materials_hash2[i].minz,
                    materials_hash2[i].maxx,
                    materials_hash2[i].maxy,
                    materials_hash2[i].maxz,
                    materials_hash2[i].minidx,
                    materials_hash2[i].maxidx,
                    materials_hash2[i].start,
                    materials_hash2[i].count
                    );
        }
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found in function report_materials_data\n", report_filename);
            exit(EXIT_FAILURE);
        }

        fprintf(file_out, "\nMaterial hash data...\n");
        fprintf(file_out, "Options name                minx      miny      minz      maxx      maxy      maxz      minidx  maxidx   start   count\n");

        for(i=0; i<material_count; i++){

            fprintf(file_out, "%7i %18s %9f %9f %9f %9f %9f %9f %7i %7i %7i %7i\n",
                    materials_hash2[i].options,
                    materials_hash2[i].name,
                    materials_hash2[i].minx,
                    materials_hash2[i].miny,
                    materials_hash2[i].minz,
                    materials_hash2[i].maxx,
                    materials_hash2[i].maxy,
                    materials_hash2[i].maxz,
                    materials_hash2[i].minidx,
                    materials_hash2[i].maxidx,
                    materials_hash2[i].start,
                    materials_hash2[i].count
                    );
        }

        fclose(file_out);
        break;

        default:
        printf("unknown value for variable 'report' in function report_materials_data\n");
        exit(EXIT_FAILURE);
    }
}
