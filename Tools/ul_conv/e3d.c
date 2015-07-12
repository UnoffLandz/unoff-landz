#include <stdio.h>

#include "e3d.h"
#include "obj.h"
#include "half.h"
#include "normal.h"
#include "processing_options.h"

void read_e3d_header(char *filename){

    /*** Reads the header data from an e3d file ***/

    //open the file
    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_header\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the file into the e3d header struct
    if(fread(&e3d_header, sizeof(e3d_header), 1, file)!=1){

        printf("unable to read e3d_header in function read_e3d_header\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void read_e3d_vertex_hash(char *filename, int vertex_hash_offset, int vertex_count){

    /*** Reads the vertex data from an e3d file ***/

    //bounds check the number of vertices
    if(vertex_count>MAX_E3D_VERTEX_COUNT){

        printf("e3d vertex count [%i] exceeds maximum [%i]\n", vertex_count, MAX_E3D_VERTEX_COUNT);
        exit(EXIT_FAILURE);
    }

    // open the file
    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_vertex_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset from the file
    unsigned char byte[vertex_hash_offset];

    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read offset for vertex hash in function read_e3d_vertex_hash\n");
        exit(EXIT_FAILURE);
    }

    //read the file into the vertex struct
    struct e3d_vertex_hash_type vh;

    for(int i=0; i<vertex_count; i++){

        if(fread(&vh, sizeof(vh), 1, file)!=1){

            printf("unable to read vertex hash [%i] in function read_e3d_vertex_hash\n", i);
            exit(EXIT_FAILURE);
        }

        e3d_vertex_hash[i].uvx=vh.uvx;
        e3d_vertex_hash[i].uvy=vh.uvy;

        e3d_vertex_hash[i].n=vh.n;

        e3d_vertex_hash[i].t=vh.t;

        e3d_vertex_hash[i].vx=vh.vx;
        e3d_vertex_hash[i].vy=vh.vy;
        e3d_vertex_hash[i].vz=vh.vz;
    }

    fclose(file);
}

void read_e3d_index_hash(char *filename, int index_hash_offset, int index_count){

    /*** Reads the index data from an e3d file ***/

    //bounds check the number of indexes
    if(index_count>MAX_E3D_INDEX_COUNT){

        printf("e3d index count [%i] exceeds maximum [%i]\n", index_count, MAX_E3D_INDEX_COUNT);
        exit(EXIT_FAILURE);
    }

    // open the file
    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_index_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    unsigned char byte[index_hash_offset];

    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read offset for index hash in function read_e3d_index_hash\n");
        exit(EXIT_FAILURE);
    }

    //read the file into the index hash
    struct e3d_vertex_index_type vi;

    for(int i=0; i<index_count; i++){

        if(fread(&vi, sizeof(vi), 1, file)!=1){

            printf("unable to read index hash [%i] in function read_e3d_index_hash\n", i);
            exit(EXIT_FAILURE);
        }

        e3d_vertex_index[i].idx=vi.idx;
    }

    fclose(file);
}

void read_e3d_materials_hash(char *filename, int materials_offset, int materials_count){

    /*** Reads the materials data from an e3d file ***/

    //bounds check the number of indexes
    if(materials_count>MAX_E3D_MATERIALS_COUNT){

        printf("e3d materials count exceeds maximum [%i]\n", MAX_E3D_MATERIALS_COUNT);
        exit(EXIT_FAILURE);
    }

    // open the file
    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function read_e3d_materials_hash\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    unsigned char byte[materials_offset];

    if(fread(&byte, sizeof(byte), 1, file)!=1){

        printf("unable to read offset for materials hash in function read_e3d_materials_hash\n");
        exit(EXIT_FAILURE);
    }

    struct e3d_materials_list_type ml;

    for(int i=0; i<materials_count; i++){

        if(fread(&ml, sizeof(ml), 1, file)!=1){

            printf("unable to read materials hash [%i] in function read_e3d_materials_hash\n", i);
            exit(EXIT_FAILURE);
        }

        e3d_materials_list[i].options=ml.options;
        strcpy(e3d_materials_list[i].name, ml.name);
        e3d_materials_list[i].min_x=ml.min_x;
        e3d_materials_list[i].min_y=ml.min_y;
        e3d_materials_list[i].min_z=ml.min_z;
        e3d_materials_list[i].max_x=ml.max_x;
        e3d_materials_list[i].max_y=ml.max_y;
        e3d_materials_list[i].max_z=ml.max_z;
        e3d_materials_list[i].min_idx=ml.min_idx;
        e3d_materials_list[i].max_idx=ml.max_idx;
        e3d_materials_list[i].idx_start=ml.idx_start;
        e3d_materials_list[i].idx_count=ml.idx_count;
    }

    fclose(file);
}

void create_e3d_file(char *filename){

    /***  Creates an e3d file ***/

    //open the file
    FILE *file;

    if((file=fopen(filename, "wb"))==NULL) {

        printf("unable to open file [%s] in function create_e3d_file\n", filename);
        exit(EXIT_FAILURE);
    }

    //write the header
    fwrite(&e3d_header, sizeof(e3d_header), 1, file);

    //write the vertex hash
    for(int i=0; i<e3d_header.vertex_count; i++){

        fwrite(&e3d_vertex_hash[i], sizeof(e3d_vertex_hash[i]), 1, file);
    }

    //write the index hash
    for(int i=0; i<e3d_header.index_count; i++){

        fwrite(&e3d_vertex_index[i], sizeof(e3d_vertex_index[i]), 1, file);
    }

    //write the materials hash
    for(int i=0; i<e3d_header.material_count; i++){

        fwrite(&e3d_materials_list[i], sizeof(e3d_materials_list[i]), 1, file);
    }

    fclose(file);
}

void report_e3d_header(){

    /*** output the e3d header data to the console ***/

    printf("\nfile name                      %s\n", p_options.filename);

    printf("\nE3D file header...\n");

    printf("magic number                   %c %c %c %c\n", e3d_header.magic_number[0], e3d_header.magic_number[1],  e3d_header.magic_number[2],  e3d_header.magic_number[3]);
    printf("version info                   %i %i %i %i\n", e3d_header.version[0], e3d_header.version[1],  e3d_header.version[2],  e3d_header.version[3]);

    printf("MD5 hash                       ");
    for(int i=0; i<MD5_HASH_LENGTH; i++)printf("%i ", e3d_header.md5_hash[i]);
    printf("\n");

    printf("header offset                  %i\n\n", e3d_header.header_offset);

    printf("vertex count                   %i\n", e3d_header.vertex_count);
    printf("vertex length                  %i\n", e3d_header.vertex_size);
    printf("vertex offset                  %i\n\n", e3d_header.vertex_offset);

    printf("index count                    %i\n", e3d_header.index_count);
    printf("index length                   %i\n", e3d_header.index_size);
    printf("index offset                   %i\n\n", e3d_header.index_offset);

    printf("material count                 %i\n", e3d_header.material_count);
    printf("material length                %i\n", e3d_header.material_size);
    printf("material offset                %i\n\n", e3d_header.material_offset);

    printf("option flag normals            "); if(e3d_header.options_flag & 0x01) printf("X\n"); else printf("-\n");
    printf("option flag tangents           "); if(e3d_header.options_flag & 0x02) printf("X\n"); else printf("-\n");
    printf("option flag extra uv           "); if(e3d_header.options_flag & 0x04) printf("X\n"); else printf("-\n");
    printf("option flag colour             "); if(e3d_header.options_flag & 0x08) printf("X\n\n"); else printf("-\n\n");

    printf("format flag half position      "); if(e3d_header.format_flag & 0x01) printf("X\n"); else printf("-\n");
    printf("format flag half uv            "); if(e3d_header.format_flag & 0x02) printf("X\n"); else printf("-\n");
    printf("format flag half extra uv      "); if(e3d_header.format_flag & 0x04) printf("X\n"); else printf("-\n");
    printf("format flag compressed normals "); if(e3d_header.format_flag & 0x08) printf("X\n"); else printf("-\n");
    printf("format flag short index        "); if(e3d_header.format_flag & 0x10) printf("X\n\n"); else printf("-\n\n");
}

void report_e3d_vertex_data(){

    /*** ouput the e3d vertex data to the console ***/

    printf("\nVertex data...\n");
    printf("[ count]  uvx       uvy       nx        ny        nz        t         vx        vy        vz\n");

    for(int i=0; i<e3d_header.vertex_count; i++){

        float uvx=half_to_float(e3d_vertex_hash[i].uvx);
        float uvy=half_to_float(e3d_vertex_hash[i].uvy);

        float normal[3];
        uncompress_normal(e3d_vertex_hash[i].n, normal);

        float t=half_to_float(e3d_vertex_hash[i].t);

        float vx=half_to_float(e3d_vertex_hash[i].vx);
        float vy=half_to_float(e3d_vertex_hash[i].vy);
        float vz=half_to_float(e3d_vertex_hash[i].vz);

         printf("[%6i] %9f %9f %9f %9f %9f %9f %9f %9f %9f\n",
                i,
                uvx,
                uvy,
                normal[0],
                normal[1],
                normal[2],
                t,
                vx,
                vy,
                vz
                );
    }

    //Doesn't work
    //int duplicate_vertex_count=check_e3d_duplicate_vertices();
    //printf("/nduplicate vertices [%i]\n", duplicate_vertex_count);
}

void report_e3d_index_data(){

    /*** output the e3d index data to the console ***/

    printf("\nIndex hash data...\n");
    printf("Count     Entry\n");

    for(int i=0; i<e3d_header.index_count; i++){

        int idx=(int)e3d_vertex_index[i].idx;

        printf("%6i %6i\n", i, idx);
    }
}

void report_e3d_materials_data() {

    /*** output the e3d materials data to the console ***/

    printf("\nMaterial hash data...\n");
    printf("Options name                    minx          miny          minz          maxx          maxy          maxz      minidx  maxidx   start   count\n");

    for(int i=0; i<e3d_header.material_count; i++){

        printf("%7i %18s %13f %13f %13f %13f %13f %13f %7i %7i %7i %7i\n",
                e3d_materials_list[i].options,
                e3d_materials_list[i].name,
                e3d_materials_list[i].min_x,
                e3d_materials_list[i].min_y,
                e3d_materials_list[i].min_z,
                e3d_materials_list[i].max_x,
                e3d_materials_list[i].max_y,
                e3d_materials_list[i].max_z,
                e3d_materials_list[i].min_idx,
                e3d_materials_list[i].max_idx,
                e3d_materials_list[i].idx_start,
                e3d_materials_list[i].idx_count
                );
    }
}

void extract_e3d_from_obj_data(){

    /*** get e3d data from the obj struct ***/

    //set the magic number (e3dx)
    e3d_header.magic_number[0]='e';
    e3d_header.magic_number[1]='3';
    e3d_header.magic_number[2]='d';
    e3d_header.magic_number[3]='x';

    //set version 11 of the e3d format (we only support this version even though there are others)
    e3d_header.version[0]=1;
    e3d_header.version[1]=1;
    e3d_header.version[2]=0;
    e3d_header.version[3]=0;

    //set the header offset
    e3d_header.header_offset=E3D_HEADER_OFFSET;

    //set the vertex data
    e3d_header.vertex_count=obj_stats.vertex_count;

    if(e3d_header.vertex_count>MAX_E3D_VERTEX_COUNT){

        printf("e3d vertex count [%i] exceeds maximum [%i]\n", e3d_header.vertex_count, MAX_E3D_VERTEX_COUNT);
        exit(EXIT_FAILURE);
    }

    e3d_header.vertex_size=E3D_VERTEX_HASH_LENGTH;
    e3d_header.vertex_offset=E3D_VERTEX_HASH_OFFSET;

    //set the index data
    e3d_header.index_count=obj_stats.face_count * 3;

    if(e3d_header.index_count>MAX_E3D_INDEX_COUNT){

        printf("e3d index count [%i] exceeds maximum [%i]\n", e3d_header.index_count, MAX_E3D_INDEX_COUNT);
        exit(EXIT_FAILURE);
    }

    e3d_header.index_size=E3D_INDEX_HASH_LENGTH;
    e3d_header.index_offset=e3d_header.vertex_offset + (e3d_header.vertex_count * E3D_VERTEX_HASH_LENGTH);

    //set the material data
    e3d_header.material_count=obj_stats.material_count;

    if(e3d_header.material_count>MAX_E3D_MATERIALS_COUNT){

        printf("e3d index count [%i] exceeds maximum [%i]\n", e3d_header.material_count, MAX_E3D_MATERIALS_COUNT);
        exit(EXIT_FAILURE);
    }

    e3d_header.material_size=E3D_MATERIAL_HASH_LENGTH;
    e3d_header.material_offset=e3d_header.index_offset + (e3d_header.index_count * E3D_INDEX_HASH_LENGTH);

    //set flags
    e3d_header.options_flag=3;
    e3d_header.format_flag=31;
    e3d_header.reserved1=0;
    e3d_header.reserved2=0;

    //initialise materials list values
    for(int i=0; i<obj_stats.material_count; i++){

        e3d_materials_list[i].options=1;

        strncpy(e3d_materials_list[i].name, obj_material[i].mtl_name, 128);

        e3d_materials_list[i].min_x=9999.0f;
        e3d_materials_list[i].min_y=9999.0f;
        e3d_materials_list[i].min_z=9999.0f;

        e3d_materials_list[i].max_x=-9999.0f;
        e3d_materials_list[i].max_y=-9999.0f;
        e3d_materials_list[i].max_z=-9999.0f;

        e3d_materials_list[i].min_idx=9999;
        e3d_materials_list[i].max_idx=-9999;

        e3d_materials_list[i].idx_count=0;
        e3d_materials_list[i].idx_start=-1;
    }

    //insert data into e3d struct
    int e3d_idx=0;

    for(int i=0; i<obj_stats.face_count; i++){

        for(int j=0; j<3; j++){

            //get vertex data
            int idx_v=obj_face[i].v[j]-1;

            float x_vertex=obj_vertex[idx_v].x;
            float y_vertex=obj_vertex[idx_v].y;
            float z_vertex=obj_vertex[idx_v].z;

            e3d_vertex_hash[idx_v].vx=float_to_half(x_vertex);
            e3d_vertex_hash[idx_v].vy=float_to_half(y_vertex);
            e3d_vertex_hash[idx_v].vz=float_to_half(z_vertex);

            //get uv texture data
            int idx_t=obj_face[i].t[j]-1;

            e3d_vertex_hash[idx_t].uvx=float_to_half(obj_texture[idx_t].x);
            e3d_vertex_hash[idx_t].uvy=float_to_half(1-obj_texture[idx_t].y);

            //get normals data
            int idx_n=obj_face[i].n[j]-1;

            float normal[3];
            normal[0]=obj_normal[idx_n].x;
            normal[1]=obj_normal[idx_n].y;
            normal[2]=obj_normal[idx_n].z;
            e3d_vertex_hash[idx_n].n=compress_normal(normal);

            //get index data
            e3d_vertex_index[e3d_idx].idx=idx_v;

            //get materials list data
            int mtl=obj_face[i].mtl;

            if(x_vertex < e3d_materials_list[mtl].min_x) e3d_materials_list[mtl].min_x=x_vertex;
            if(y_vertex < e3d_materials_list[mtl].min_y) e3d_materials_list[mtl].min_y=y_vertex;
            if(z_vertex < e3d_materials_list[mtl].min_z) e3d_materials_list[mtl].min_z=z_vertex;

            if(x_vertex > e3d_materials_list[mtl].max_x) e3d_materials_list[mtl].max_x=x_vertex;
            if(y_vertex > e3d_materials_list[mtl].max_y) e3d_materials_list[mtl].max_y=y_vertex;
            if(z_vertex > e3d_materials_list[mtl].max_z) e3d_materials_list[mtl].max_z=z_vertex;

            if(idx_v < e3d_materials_list[mtl].min_idx) e3d_materials_list[mtl].min_idx=idx_v;
            if(idx_v > e3d_materials_list[mtl].max_idx) e3d_materials_list[mtl].max_idx=idx_v;

            e3d_materials_list[mtl].idx_count++;
            if(e3d_materials_list[mtl].idx_start<0) e3d_materials_list[mtl].idx_start=e3d_idx;

            e3d_idx++;
        }
    }
}

void report_e3d_data(){

    report_e3d_header();
    report_e3d_vertex_data();
    report_e3d_index_data();
    report_e3d_materials_data();
}

void convert_obj_to_e3d_file(){

    read_obj_data(p_options.filename);

    extract_e3d_from_obj_data();

    create_e3d_file(MD5_TEMP_FILENAME);
    get_md5(MD5_TEMP_FILENAME, E3D_HEADER_OFFSET, e3d_header.md5_hash);

    //extract the prefix from the conversion filename
    char prefix[80]; /** TODO replace hardcoded amount **/
    get_filename_prefix(p_options.filename, prefix);

    //create the e3d filename
    char e3d_filename[80]; /** TODO replace hardcoded amount **/
    sprintf(e3d_filename, "%s.e3d", prefix);

    create_e3d_file(e3d_filename);

    report_e3d_data();

    printf("\ncreated file [%s]\n", e3d_filename);
}

/*
int check_e3d_duplicate_vertices(){

    int k=0;

    for(int i=0; i<e3d_header.vertex_count; i++){

        //check the preceding vertices to see if they duplicate the current one
        for(int j=0; j<i; j++){

            if(
                (e3d_vertex_hash[j].vz==e3d_vertex_hash[i].vz) &&
                (e3d_vertex_hash[j].vy==e3d_vertex_hash[i].vy) &&
                (e3d_vertex_hash[j].vx==e3d_vertex_hash[i].vx) &&
                (e3d_vertex_hash[j].uvx==e3d_vertex_hash[i].uvx) &&
                (e3d_vertex_hash[j].uvy==e3d_vertex_hash[i].uvy) &&
                (e3d_vertex_hash[j].t==e3d_vertex_hash[i].t) &&
                (e3d_vertex_hash[j].n==e3d_vertex_hash[i].n)
               ){

                k++;
            }
        }
    }

    return k;
}

void remove_duplicate_e3d_vertices(){

    int k=0;

    for(int i=0; i<e3d_header.vertex_count; i++){

        //check the preceding vertices to see if they duplicate the current one
        bool dup=false;

        for(int j=0; j<i; j++){

            if(
                (e3d_vertex_hash[j].vz==e3d_vertex_hash[i].vz) &&
                (e3d_vertex_hash[j].vy==e3d_vertex_hash[i].vy) &&
                (e3d_vertex_hash[j].vx==e3d_vertex_hash[i].vx) &&
                (e3d_vertex_hash[j].uvx==e3d_vertex_hash[i].uvx) &&
                (e3d_vertex_hash[j].uvy==e3d_vertex_hash[i].uvy) &&
                (e3d_vertex_hash[j].t==e3d_vertex_hash[i].t) &&
                (e3d_vertex_hash[j].n==e3d_vertex_hash[i].n)
               ){

                k++;

                for(int l=0; l<e3d_header.index_count; l++){

                    if(e3d_vertex_index[l].idx==i) e3d_vertex_index[l].idx=j;
                }

                dup=true;
                break;
            }
        }

        if(dup==false){

            e3d_vertex_hash[i-k].vz=e3d_vertex_hash[i].vz;
            e3d_vertex_hash[i-k].vy=e3d_vertex_hash[i].vy;
            e3d_vertex_hash[i-k].vx=e3d_vertex_hash[i].vx;

            e3d_vertex_hash[i-k].uvx=e3d_vertex_hash[i].uvx;
            e3d_vertex_hash[i-k].uvy=e3d_vertex_hash[i].uvy;

            e3d_vertex_hash[i-k].n=e3d_vertex_hash[i].n;

            e3d_vertex_hash[i-k].t=e3d_vertex_hash[i].t;

            for(int l=0; l<e3d_header.index_count; l++){

                if(e3d_vertex_index[l].idx==i) e3d_vertex_index[l].idx=i-k;
            }
        }
    }
}
*/
