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
#include <string.h>
#include <openssl/md5.h> //required for checking the md5 hash in the e3d file

#include "files.h"
#include "create_e3d.h"
#include "half.h"
#include "normal.h"

void get_texture_file(char *mtl_filename, char *obj_tag, char *texture_filename){

    FILE *file;
    char line[FILELINE_LEN]="";
    char mtl_tag[80]="";
    int get_next_kd=FALSE;

    sprintf(mtl_tag, "newmtl %s\n", obj_tag);

    // open the file
    if((file=fopen(mtl_filename, "r"))==NULL) {

        printf("unable to open file [%s] in function get_texture_file\n", mtl_filename);
        exit(EXIT_FAILURE);
    }

    while(fgets(line, FILELINE_LEN, file)){

        //remove right hand spaces
        str_trim_right(line);

        // check for material tag
        if(strcmp(line, mtl_tag)==0) get_next_kd=TRUE;

        if(strncmp(line, "Kd ", 3)==0 && get_next_kd==TRUE){

            strcpy(texture_filename, line+4);
            return;
        }
    }

    strcpy(texture_filename, "");
}

void read_obj_data(char *filename, struct obj_type *obj){

/***    FUNCTION    - read data from wavefront obj format file
        PURPOSE     - prepares data needed to create eternal lands e3d format
        INPUT       - filename = name of e3d file to be created
                    - struct obj_type *obj = struct holding the wavefront obj data
        OUTPUT      - void
***/

    FILE *file;
    char line[FILELINE_LEN];
    int v=0, vt=0, vn=0, lib=0, mtl=0;
    int face_mtl=0;
    int idx=0;
    char mtl_filename[80];

    // open the file
    if((file=fopen(filename, "r"))==NULL) {

        printf("unable to open file [%s] in function read_obj_vertex_data\n", filename);
        exit(EXIT_FAILURE);
    }

    // parse the wavefront obj file
    while(fgets(line, FILELINE_LEN, file)){

        //identify vertex data entries
        if(strncmp(line, "v ", 2)==0) {

            if(sscanf(line+2, "%f %f %f", &obj->vertex[v].x, &obj->vertex[v].y, &obj->vertex[v].z)!=3){

                printf("sscanf failed to read 'v' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            v++;
        }

        //identify texture data entries
        if(strncmp(line, "vt ", 3)==0) {

            if(sscanf(line+3, "%f %f", &obj->texture[vt].x, &obj->texture[vt].y)!=2){

                printf("sscanf failed to read 'vt' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            vt++;
        }

        //identify normals data entries
        if(strncmp(line, "vn ", 3)==0) {


            if(sscanf(line+3, "%f %f %f", &obj->normal[vn].x, &obj->normal[vn].y, &obj->normal[vn].z)!=3){

                printf("sscanf failed to read 'vn' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            vn++;
        }

        //identify materials lib entries
        if(strncmp(line, "mtllib ", 7)==0){

            if(sscanf(line+7, "%s", obj->material_lib)!=1){

                printf("sscanf failed to read 'mtllib' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            //remove the newline char from the end of the string
            strncpy(mtl_filename, line+7, strlen(line)-8);

            //check the file exists
            if(file_exists(mtl_filename)==FALSE){

                printf("materials file [%s] not found\n", mtl_filename);
                exit(EXIT_FAILURE);
            }

            lib++;

            if(lib>1){

                printf("obj file has more than 1 mtllib\n");
                exit(EXIT_FAILURE);
            }
        }

        //identify usemtl entries
        if(strncmp(line, "usemtl ", 7)==0) {

            if(lib==0) {

                printf("obj file usemtl tag has no matching mtllib\n");
                exit(EXIT_FAILURE);
            }

            if(sscanf(line+7, "%s", obj->material[mtl].material_name)!=1){

                printf("sscanf failed to read 'usemtl' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            face_mtl=mtl;
            mtl++;
        }

        //identify face entries
        if(strncmp(line, "f ", 2)==0) {

            if(sscanf(line+2, "%i/%i/%i %i/%i/%i %i/%i/%i",
                    &obj->index[idx].v,
                    &obj->index[idx].t,
                    &obj->index[idx].n,
                    &obj->index[idx+1].v,
                    &obj->index[idx+1].t,
                    &obj->index[idx+1].n,
                    &obj->index[idx+2].v,
                    &obj->index[idx+2].t,
                    &obj->index[idx+2].n
                    )!=9){

                printf("sscanf failed to read 'f' entry in function read_obj_face_data\n");
                exit(EXIT_FAILURE);
            }

            obj->index[idx].mtl=face_mtl;
            obj->index[idx+1].mtl=face_mtl;
            obj->index[idx+2].mtl=face_mtl;

            idx+=3;
        }
    }

    obj->vertex_count=v;
    obj->texture_count=vt;
    obj->normals_count=vn;
    obj->material_count=mtl;
    obj->index_count=idx;

    fclose(file);
}

void create_e3d_file(char *filename, struct obj_type *obj){


/***    FUNCTION    - create_e3d_file
        PURPOSE     - creates an e3d file based on supplied wavefront obj data
        INPUT       - filename = name of e3d file to be created
                    - struct obj_type *obj = struct holding the wavefront obj data
        OUTPUT      - void
***/


//                                          DECLARE OUR VARIABLES


    int i=0, j=0;
    int e3d_idx=0, t_idx, n_idx, v_idx;
    float normal[3];
    FILE *file;
    char e3d_filename[80]="";
    char filename_prefix[80]="";


//                      DECLARE THE HASH STRUCTURES THAT WE WILL WRITE DIRECTLY TO THE E3D FILE


    struct {// the header hash

        char magic_number[4];
        unsigned char version[4];
        unsigned char md5_hash[16];
        int header_offset;

        int vertex_count;
        int vertex_size;
        int vertex_offset;

        int index_count;
        int index_size;
        int index_offset;

        int material_count;
        int material_size;
        int material_offset;

        unsigned char options_flag;
        unsigned char format_flag;
        unsigned char reserved1;
        unsigned char reserved2;
    }e3d_file;

    struct {//the vertex hash

        Uint16 uvx;
        Uint16 uvy;
        Uint16 n;
        Uint16 t;
        Uint16 vz;
        Uint16 vx;
        Uint16 vy;
    }vertex_hash[obj->vertex_count];

    struct {// the index hash

        Uint16 idx;
    }index_hash[obj->index_count];

    struct {// the material hash

        int options;
        char name[128];
        float minx;
        float miny;
        float minz;
        float maxx;
        float maxy;
        float maxz;
        int minidx;
        int maxidx;
        int start;
        int count;
    }material_hash[obj->material_count];


//                                    CREATE THE HEADER HASH


    //set the magic number (e3dx)
    e3d_file.magic_number[0]='e';
    e3d_file.magic_number[1]='3';
    e3d_file.magic_number[2]='d';
    e3d_file.magic_number[3]='x';

    //set version 11 of the e3d format (we only support this version even though there are others)
    e3d_file.version[0]=1;
    e3d_file.version[1]=1;
    e3d_file.version[2]=0;
    e3d_file.version[3]=0;

    //set the header offset
    e3d_file.header_offset=HEADER_OFFSET;

    //set the vertex data
    e3d_file.vertex_count=obj->vertex_count;
    e3d_file.vertex_size=VERTEX_HASH_LENGTH;
    e3d_file.vertex_offset=HEADER_OFFSET + HEADER_LENGTH;

    //set the index data
    e3d_file.index_count=obj->index_count;
    e3d_file.index_size=INDEX_HASH_LENGTH;
    e3d_file.index_offset=e3d_file.vertex_offset + (e3d_file.vertex_count * VERTEX_HASH_LENGTH);

    //set the material data
    e3d_file.material_count=obj->material_count;
    e3d_file.material_size=MATERIAL_HASH_LENGTH;
    e3d_file.material_offset=e3d_file.index_offset + (e3d_file.index_count * INDEX_HASH_LENGTH);

    //set flags
    e3d_file.options_flag=3;
    e3d_file.format_flag=31;
    e3d_file.reserved1=0;
    e3d_file.reserved2=0;

    if(obj->vertex_count != obj->index_count) printf("warning! unequal number of vertices and indices in obj file\n");


//                                CREATE THE VERTEX AND INDEX HASHES


    //loop through the index values
    for(i=0; i<e3d_file.index_count; i++){

        //use the vertex index in the obj face data as the e3d index value
        e3d_idx=obj->index[i].v-1;//e3d index values run from zero, hence, we need to subtract 1 from the obj face data (which runs from 1)

        //set the vertex hash texture values
        t_idx=obj->index[i].t-1;//get the obj texture index value
        vertex_hash[e3d_idx].uvx=float_to_half(obj->texture[t_idx].x);
        vertex_hash[e3d_idx].uvy=float_to_half(1-obj->texture[t_idx].y);

        //set the vertex hash normal values
        n_idx=obj->index[i].n-1;//get the obj normal index value
        normal[0]=obj->normal[n_idx].x;
        normal[1]=obj->normal[n_idx].y;
        normal[2]=obj->normal[n_idx].z;
        vertex_hash[e3d_idx].n=compress_normal(normal);

        //set the vertex hash tangent values
        vertex_hash[e3d_idx].t=0;/*** need to calculate tangents ***/

        //set the the vertex hash vertex values
        v_idx=obj->index[i].v-1;//get the obj vertex index value
        vertex_hash[e3d_idx].vz=float_to_half(obj->vertex[v_idx].z);
        vertex_hash[e3d_idx].vx=float_to_half(obj->vertex[v_idx].x);
        vertex_hash[e3d_idx].vy=float_to_half(obj->vertex[v_idx].y);

        //set the index hash
        index_hash[i].idx = (Uint16) e3d_idx;
    }


//                                CREATE THE MARTERIAL HASH


    //loop through each material
    for(i=0; i<e3d_file.material_count; i++){

        int idx=0;
        int start=0;
        int count=0;
        int max_idx=0;
        int min_idx=99999999;
        float max_x=0, max_y=0, max_z=0;
        float min_x=99999999, min_y=99999999, min_z=99999999;

        //loop through the obj face data for material and establish max/min values
        for(j=0; j<e3d_file.index_count; j++){

            if(obj->index[j].mtl==i){

                idx=obj->index[j].v;

                if(obj->vertex[idx].x < min_x) min_x=obj->vertex[idx].x;
                if(obj->vertex[idx].y < min_y) min_y=obj->vertex[idx].y;
                if(obj->vertex[idx].z < min_z) min_z=obj->vertex[idx].z;

                if(obj->vertex[idx].x > max_x) max_x=obj->vertex[idx].x;
                if(obj->vertex[idx].x > max_y) max_y=obj->vertex[idx].y;
                if(obj->vertex[idx].x > max_z) max_z=obj->vertex[idx].z;

                if(idx>max_idx) max_idx=idx;
                if(idx<min_idx) min_idx=idx;

                if(count==0) {
                    start=j;
                    count=1;
                }
                else count++;
            }
        }

        //set options
        material_hash[i].options=0;

        //set material file name
        strncpy(material_hash[i].name, obj->material[i].material_name, 128);

        //set min and max vertex values
        material_hash[i].minx=obj->material[i].min_x;
        material_hash[i].miny=obj->material[i].min_y;
        material_hash[i].minz=obj->material[i].min_z;

        material_hash[i].maxx=obj->material[i].max_x;
        material_hash[i].maxy=obj->material[i].max_y;
        material_hash[i].maxz=obj->material[i].max_z;

        //set min and max index values
        material_hash[i].minidx=min_idx;
        material_hash[i].maxidx=max_idx;

        //set index start and count values
        material_hash[i].start=start;
        material_hash[i].count=count;
    }


//                                  CALCULATE THE MD5 HASH


    //write the hashes to the file so we can calculate the md5
    get_filename_prefix(filename, filename_prefix);
    sprintf(e3d_filename, "%s.e3d", filename_prefix);

    if((file=fopen(e3d_filename, "wb"))==NULL) {

        printf("unable to open file [%s] in function create_e3d_file\n", filename);
        exit(EXIT_FAILURE);
    }

    fwrite(&e3d_file, sizeof(e3d_file), 1, file);           //header hash
    fwrite(&vertex_hash, sizeof(vertex_hash), 1, file);     //vertex hash
    fwrite(&index_hash, sizeof(index_hash), 1, file);       //index hash
    fwrite(&material_hash, sizeof(material_hash), 1, file); //material hash

    fclose(file);

    //calculate the MD5 hash and insert into the header struct
    get_md5(filename, EXPECTED_HEADER_OFFSET, e3d_file.md5_hash);


//                                      CREATE THE E3D FILE


    //rewrite the file to include the md5 hash
    if((file=fopen(e3d_filename, "wb"))==NULL) {

        printf("unable to open file [%s] in function create_e3d_file\n", filename);
        exit(EXIT_FAILURE);
    }

    fwrite(&e3d_file, sizeof(e3d_file), 1, file);           //header hash
    fwrite(&vertex_hash, sizeof(vertex_hash), 1, file);     //vertex hash
    fwrite(&index_hash, sizeof(index_hash), 1, file);       //index hash
    fwrite(&material_hash, sizeof(material_hash), 1, file); //material hash

    fclose(file);

    printf("filename [%s] created in EL e3d format\n", e3d_filename);
}
