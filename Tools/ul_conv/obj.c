#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "e3d.h"
#include "normal.h"
#include "half.h"
#include "processing_options.h"
#include "common.h"

void read_obj_data(char *filename){

    /*** Reads data from an obj file ***/

    //open the file
    FILE *file;

    if((file=fopen(filename, "r"))==NULL) {

        printf("unable to open file [%s] in function read_obj_data\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[OBJ_FILE_LINE_LENGTH];
    int v=0, vt=0, vn=0, lib=0, mtl=0;
    int face_mtl=0;
    int idx=0;
    char mtl_filename[80];

    // read the file
    while(fgets(line, OBJ_FILE_LINE_LENGTH, file)){

        //identify vertex data entries
        if(strncmp(line, "v ", 2)==0) {

            if(sscanf(line+2, "%f %f %f", &obj_vertex[v].x, &obj_vertex[v].y, &obj_vertex[v].z)!=3){

                printf("sscanf failed to read 'v' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            //bounds check the number of vertices
            if(v==MAX_OBJ_VERTEX_COUNT){

                printf("obj vertex count [%i] exceeds maximum [%i] in function read_obj_data\n", v, MAX_OBJ_VERTEX_COUNT);
                exit(EXIT_FAILURE);
            }

            v++;
        }

        //identify texture data entries
        if(strncmp(line, "vt ", 3)==0) {

            if(sscanf(line+3, "%f %f", &obj_texture[vt].x, &obj_texture[vt].y)!=2){

                printf("sscanf failed to read 'vt' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            if(vt==MAX_OBJ_TEXTURE_COUNT){

                printf("texture count [%i] exceeds maximum [%i] in function read_obj_data\n", vt, MAX_OBJ_TEXTURE_COUNT);
                exit(EXIT_FAILURE);
            }

            vt++;
        }

        //identify normals data entries
        if(strncmp(line, "vn ", 3)==0) {


            if(sscanf(line+3, "%f %f %f", &obj_normal[vn].x, &obj_normal[vn].y, &obj_normal[vn].z)!=3){

                printf("sscanf failed to read 'vn' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            if(vn==MAX_OBJ_NORMAL_COUNT){

                printf("normal count [%i] exceeds maximum [%i] in function read_obj_data\n", vn, MAX_OBJ_NORMAL_COUNT);
                exit(EXIT_FAILURE);
            }

            vn++;
        }

        //identify materials lib entries
        if(strncmp(line, "mtllib ", 7)==0){

            if(sscanf(line+7, "%s", obj_material[lib].mtl_name)!=1){

                printf("sscanf failed to read 'mtllib' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            //remove the newline char from the end of the string
            strncpy(mtl_filename, line+7, strlen(line)-8);

            if(lib==MAX_OBJ_MTLLIB_COUNT){

                printf("mtllib count [%i] exceeds maximum [%i] in function read_obj_data\n", lib, MAX_OBJ_MTLLIB_COUNT);
                exit(EXIT_FAILURE);
            }

            lib++;
        }

        //identify usemtl entries
        if(strncmp(line, "usemtl ", 7)==0) {

            if(lib==0) {

                printf("obj file usemtl tag has no matching mtllib\n");
                exit(EXIT_FAILURE);
            }

            if(sscanf(line+7, "%s", obj_material[mtl].mtl_name)!=1){

                printf("sscanf failed to read 'usemtl' entry in function read_obj_data\n");
                exit(EXIT_FAILURE);
            }

            face_mtl=mtl;

            if(lib==MAX_OBJ_MATERIAL_COUNT){

                printf("material count [%i] exceeds maximum [%i] in function read_obj_data\n", lib, MAX_OBJ_MATERIAL_COUNT);
                exit(EXIT_FAILURE);
            }

            mtl++;
        }

        //identify face entries
        if(strncmp(line, "f ", 2)==0) {

            if(sscanf(line+2, "%i/%i/%i %i/%i/%i %i/%i/%i",
                    &obj_face[idx].v[0],
                    &obj_face[idx].t[0],
                    &obj_face[idx].n[0],

                    &obj_face[idx].v[1],
                    &obj_face[idx].t[1],
                    &obj_face[idx].n[1],

                    &obj_face[idx].v[2],
                    &obj_face[idx].t[2],
                    &obj_face[idx].n[2]
                    )!=9){

                printf("sscanf failed to read 'f' entry in function read_obj_face_data\n");
                exit(EXIT_FAILURE);
            }

            obj_face[idx].mtl=face_mtl;

            if(idx==MAX_OBJ_FACE_COUNT){

                printf("face count [%i] exceeds maximum [%i] in function read_obj_data\n", lib, MAX_OBJ_FACE_COUNT);
                exit(EXIT_FAILURE);
            }

            idx++;
        }
    }

    obj_stats.vertex_count=v;
    obj_stats.texture_count=vt;
    obj_stats.normals_count=vn;
    obj_stats.material_count=mtl;
    obj_stats.face_count=idx;

    fclose(file);
 }

void extract_obj_data_from_e3d(){

    /*** get obj data from the e3d struct ***/

    //get the vertices
    obj_stats.vertex_count=e3d_header.vertex_count;

    for(int i=0; i<obj_stats.vertex_count; i++){

        obj_vertex[i].x=half_to_float(e3d_vertex_hash[i].vx);
        obj_vertex[i].y=half_to_float(e3d_vertex_hash[i].vy);
        obj_vertex[i].z=half_to_float(e3d_vertex_hash[i].vz);
    }
    //get the texture/uv data - (texture/uv count will equal vertex count)
    obj_stats.texture_count=e3d_header.vertex_count;

    for(int i=0; i<obj_stats.texture_count; i++){

        obj_texture[i].x=half_to_float(e3d_vertex_hash[i].uvx);
        obj_texture[i].y=1-half_to_float(e3d_vertex_hash[i].uvy);
    }

    //get the normals data (normals count will equal vertex count)
    obj_stats.normals_count=e3d_header.vertex_count;

    for(int i=0; i<obj_stats.normals_count; i++){

        float normal[3]={0.0f, 0.0f, 0.0f};
        uncompress_normal(e3d_vertex_hash[i].n, normal);
        obj_normal[i].x=normal[0];
        obj_normal[i].y=normal[1];
        obj_normal[i].z=normal[2];
    }

    //get the face and materials data
    obj_stats.material_count=e3d_header.material_count;

    int l=0;

    for(int i=0; i<e3d_header.material_count; i++){

        strcpy(obj_material[i].mtl_name, e3d_materials_list[i].name);

        int start=e3d_materials_list[i].idx_start;

        for(int j=0; j<e3d_materials_list[i].idx_count; j+=3){

            obj_face[l].v[0]=e3d_vertex_index[start+j+0].idx+1;
            obj_face[l].t[0]=e3d_vertex_index[start+j+0].idx+1;
            obj_face[l].n[0]=e3d_vertex_index[start+j+0].idx+1;

            obj_face[l].v[1]=e3d_vertex_index[start+j+1].idx+1;
            obj_face[l].t[1]=e3d_vertex_index[start+j+1].idx+1;
            obj_face[l].n[1]=e3d_vertex_index[start+j+1].idx+1;

            obj_face[l].v[2]=e3d_vertex_index[start+j+2].idx+1;
            obj_face[l].t[2]=e3d_vertex_index[start+j+2].idx+1;
            obj_face[l].n[2]=e3d_vertex_index[start+j+2].idx+1;

            obj_face[l].mtl=i;

            l++;
        }
    }

    obj_stats.face_count=l;
}

void create_obj_file(char *obj_filename, char *mtl_filename){

    /*** creates an obj file ***/

    FILE *file;

    //create the obj file
    if((file=fopen(obj_filename, "w"))==NULL) {

        printf("unable to create file [%s] in function create_obj_file\n", obj_filename);
        exit(EXIT_FAILURE);
    }

    //mark the file with the app and version number
    fprintf(file, "# file created by Unofflandz ul_conv %s\n", VERSION);

    //write the mtl file link
    fprintf(file, "mtllib %s\n", mtl_filename);

    //write the vertices
    fprintf(file, "\n# %i vertices\n", obj_stats.vertex_count);

    for(int i=0; i<obj_stats.vertex_count; i++){

        fprintf(file, "v %f %f %f\n", obj_vertex[i].x, obj_vertex[i].y, obj_vertex[i].z);
    }

    //write the textures
    fprintf(file, "\n# %i textures\n", obj_stats.texture_count);

    for(int i=0; i<obj_stats.texture_count; i++){

        fprintf(file, "vt %f %f\n", obj_texture[i].x, obj_texture[i].y);
    }

    //write the normals
    fprintf(file, "\n# %i normals\n", obj_stats.normals_count);

    for(int i=0; i<obj_stats.normals_count; i++){

       fprintf(file, "vn %f %f %f\n", obj_normal[i].x, obj_normal[i].y, obj_normal[i].z);
    }

    //write the faces
    fprintf(file, "\n# %i faces\n", obj_stats.face_count);

    for(int i=0; i<obj_stats.material_count; i++){

        fprintf(file, "usemtl %s\n", obj_material[i].mtl_name);

        //groups faces so that Unity correctly applies textures
        fprintf(file, "g %s\n", obj_material[i].mtl_name);

        //add smoothing
        fprintf(file, "s 1\n");

        for(int j=0; j<obj_stats.face_count; j++){

            if(obj_face[j].mtl==i){

               fprintf(file, "f %i/%i/%i %i/%i/%i %i/%i/%i\n",
                    obj_face[j].v[0], obj_face[j].t[0], obj_face[j].n[0],
                    obj_face[j].v[1], obj_face[j].t[1], obj_face[j].n[1],
                    obj_face[j].v[2], obj_face[j].t[2], obj_face[j].n[2]);
            }
        }
    }

    fclose(file);
}

void convert_e3d_to_obj_file(){

    read_e3d_header(p_options.filename);
    read_e3d_vertex_hash(p_options.filename, e3d_header.vertex_offset, e3d_header.vertex_count);
    read_e3d_index_hash(p_options.filename, e3d_header.index_offset, e3d_header.index_count);
    read_e3d_materials_hash(p_options.filename, e3d_header.material_offset, e3d_header.material_count);

    extract_obj_data_from_e3d();

    //extract the prefix from the conversion filename
    char prefix[80]; /** TODO replace hardcoded amount **/
    get_filename_prefix(p_options.filename, prefix);

    //create the obj filename
    char obj_filename[80]; /** TODO replace hardcoded amount **/
    sprintf(obj_filename, "%s.obj", prefix);

    //create the mtl filename
    char mtl_filename[80]; /** TODO replace hardcoded amount **/
    sprintf(mtl_filename, "%s.mtl", prefix);

    create_obj_file(obj_filename, mtl_filename);
    create_mtl_file(mtl_filename);

    report_e3d_data();
    printf("\ncreated files [%s] and [%s]\n", obj_filename, mtl_filename);
}

void create_mtl_file(char *mtl_filename){

    /*** creates the mtl file ***/

    FILE *file;

    //create the file
    if((file=fopen(mtl_filename, "w"))==NULL) {

        printf("unable to create file [%s] in function create_mtl_file\n", mtl_filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "# file created by Unofflandz ul_conv %s\n", VERSION);

    for(int i=0; i<obj_stats.material_count; i++){

        fprintf(file, "newmtl %s\n", obj_material[i].mtl_name);
        fprintf(file, "Ka 1.000000 1.000000 1.000000\n");
        fprintf(file, "Kd 0.640000 0.640000 0.640000\n");
        fprintf(file, "Ks 0.250000 0.250000 0.250000\n");
        fprintf(file, "Ni 1.000000\n");

        fprintf(file, "d 0.98\n");

        fprintf(file, "illum 2\n");
        fprintf(file, "Ns 0.312500\n");
        fprintf(file, "map_Kd %s\n", obj_material[i].mtl_name);
    }

    fclose(file);
}

void report_obj_stats(){

    /*** output the e3d header data to the console ***/
    printf("\nfile name                      %s\n", p_options.filename);

    printf("\nOBJ stats...\n");

    printf("vertex count                   %i\n", obj_stats.vertex_count);
    printf("texture count (uv)             %i\n", obj_stats.texture_count);
    printf("normals count                  %i\n", obj_stats.normals_count);
    printf("face count                     %i\n", obj_stats.face_count);
    printf("material count                 %i\n", obj_stats.material_count);
}
