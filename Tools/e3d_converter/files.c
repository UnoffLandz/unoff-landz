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
#include <openssl/md5.h>        //required for checking the md5 hash in the e3d file
//#include <math.h>               //required for sqrt function

#include "files.h"
#include "half.h"               // required for converting 16bit float values
#include "normal.h"             // required for converting compressed normal values
#include "dds.h"

int Uint16_to_dec(unsigned char *byte){

    /*** returns a 16bit integer value ***/

    union i
	{
        unsigned char buf[4];
        int16_t number;
	}i;

	i.buf[0] = byte[0];
	i.buf[1] = byte[1];

	return (int) i.number;
}

float Uint32_to_float(unsigned char *byte){

    /*** returns a 32bit floating point value ***/

    union i
	{
        unsigned char buf[4];
        float number;
	}i;

	i.buf[0] = byte[0];
	i.buf[1] = byte[1];
	i.buf[2] = byte[2];
	i.buf[3] = byte[3];

	return i.number;
}

int Uint32_to_dec(unsigned char *byte){

    /*** returns a 32bit integer value ***/

    union i
	{
        unsigned char buf[4];
        int number;
	}i;

	i.buf[0] = byte[0];
	i.buf[1] = byte[1];
	i.buf[2] = byte[2];
	i.buf[3] = byte[3];

	return i.number;
}

int file_exists(char *file_name){

    /*** returns TRUE if a file exists ***/

    FILE *file;

    if((file=fopen(file_name, "rb"))==NULL) return FALSE;

    fclose(file);

    return TRUE;
}

int get_file_size(char *file_name){

    /*** returns the number of bytes in a file ***/

    FILE *file;
    int count=0;

    //open the file
    if((file=fopen(file_name, "rb"))==NULL) return 0;

    //count the bytes until an EOF marker is found
    while(fgetc(file) != EOF) {
        count++;
    }

    //close the file and return the value
    fclose(file);

    return count;
}

void get_filename_prefix( char *filename, char *prefix){

    /*** split a filename from its prefix (prefix is returned through 'char *prefix') ***/

    strncpy(prefix, filename, strlen(filename)-(SUFFIX_LEN+1));
}

char *truefalse(int i){

    /*** returns a string representation of TRUE/FALSE ***/

    if(i==TRUE) return "TRUE"; else return "FALSE";
}

void get_md5(char *filename, int offset, unsigned char *md5_hash){

    /*** calculates the md5 hash of a file (md5 hash is returned through 'unsigned char *md5_hash) ***/

    unsigned char byte[MAX_DATA];
    unsigned char md5_digest[E3D_MD5_HASH_LEN];

    int file_len=get_file_size(filename);
    FILE *file;

    //open file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function check_md5: module files.c", filename);
        exit(EXIT_FAILURE);
    }

    //read the file up to the offset at which the md5 is to be calculated
    if(offset>0) {

        if(fread(&byte, offset, 1, file)!=1){

            printf("unable to read file [%s] in function check_md5: module files.c", filename);
            exit (EXIT_FAILURE);
        }
    }

    //test to ensure data size does not exceed array
    if (file_len-offset>MAX_DATA){

        printf("data size [%i] in file [%s] exceeds array maximum of [%i]\n", file_len-offset, filename, MAX_DATA);
        exit(EXIT_FAILURE);
    }

    //read the file into a string
    if(fread(&byte, file_len-offset, 1, file)!=1){

        printf("unable to read file [%s] in function check_md5: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    //calculate md5 of the string
    MD5((unsigned char*)&byte, file_len-offset, (unsigned char*)&md5_digest);

    //copy the md5 hash to a new string (we could return a pointer or do some fancy casting but we hate that kind of thing)
    memcpy(md5_hash, md5_digest, E3D_MD5_HASH_LEN);
}

/*
void validate_file(char *filename, struct e3d_type *e3d){

    int i;
    FILE *file;
    unsigned char byte[MAX_DATA];
    unsigned char md5_hash[E3D_MD5_HASH_LEN];

    // open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function validate_file\n", filename);
        exit(EXIT_FAILURE);
    }

    //read the header offset
    if(fread(&byte, E3D_EXPECTED_HEADER_OFFSET, 1, file)!=1){

        printf("unable to read data from file [%s] in function validate_file\n", filename);
        exit(EXIT_FAILURE);;
    }

    //close file
    fclose(file);

    //check file type
    if(byte[0]=='e' && byte[1]=='3' && byte[2]=='d' && byte[3]=='x') {

        memcpy(e3d->file_type, byte, 4);
    }
    else {

        printf("unknown file type    [%c] [%c] [%c] [%c]\n", byte[0], byte[1], byte[2], byte[3]);
        exit(EXIT_FAILURE);
    }

    //check file version
    if( byte[4]==1 && byte[5]==1 && byte[6]==0 && byte[7]==0){

        memcpy(e3d->version, byte+4, 4);
     }
    else{

        printf("unsupported file version [%i] [%i] [%i] [%i]\n", byte[0], byte[1], byte[2], byte[3]);
        exit(EXIT_FAILURE);
    }

    //check md5 hash
    get_md5(filename, E3D_EXPECTED_HEADER_OFFSET, md5_hash);

    if(memcmp(md5_hash, byte+8, E3D_MD5_HASH_LEN)==0) {

        for(i=0; i<E3D_MD5_HASH_LEN; i++){
            e3d->md5_hash[i]=byte[8+i];
        }
    }
    else {

        printf("file md5 hash does not match...\n");

        printf("expected = ");
        for(i=0; i<E3D_MD5_HASH_LEN; i++) printf("%i ", md5_hash[i]);
        printf("\n");

        printf("found    = ");
        for(i=0; i<E3D_MD5_HASH_LEN; i++) printf("%i ", byte[i+8]);
        printf("\n");

        exit(EXIT_FAILURE);
    }

    //check file header size
    int header_offset=Uint32_to_dec(byte+24);

    if(header_offset==E3D_EXPECTED_HEADER_OFFSET){

        e3d->header_offset=header_offset;
    }
    else {

        printf("header offset [%i] does not equal %i bytes\n", header_offset, E3D_EXPECTED_HEADER_OFFSET);
        exit(EXIT_FAILURE);
    }
}
*/


/*
void calculate_normals(struct e3d_type *e3d){

    int i=0, j=0, pos=0;
    float vector_x1=0, vector_x2=0, vector_y1=0, vector_y2=0, vector_z1=0, vector_z2=0;
    float nx, ny, nz;
    float combined_squares, normalisation_factor;

    for(i=0; i<e3d->index_count; i++){

        j++;
        if(j==3) j=0;

        pos=e3d->index_hash[i].index;

        if(j==0){

            vector_x1=e3d->vertex_hash[pos].vx - e3d->vertex_hash[pos+1].vx;
            vector_x2=e3d->vertex_hash[pos+2].vx - e3d->vertex_hash[pos].vx;

            vector_y1=e3d->vertex_hash[pos].vy - e3d->vertex_hash[pos+1].vy;
            vector_y2=e3d->vertex_hash[pos+2].vy - e3d->vertex_hash[pos].vy;

            vector_z1=e3d->vertex_hash[pos].vz - e3d->vertex_hash[pos+1].vz;
            vector_z2=e3d->vertex_hash[pos+2].vz - e3d->vertex_hash[pos].vz;
        }

        if(j==1){

            vector_x1=e3d->vertex_hash[pos].vx - e3d->vertex_hash[pos+1].vx;
            vector_x2=e3d->vertex_hash[pos-1].vx - e3d->vertex_hash[pos].vx;

            vector_y1=e3d->vertex_hash[pos].vy - e3d->vertex_hash[pos+1].vy;
            vector_y2=e3d->vertex_hash[pos-1].vy - e3d->vertex_hash[pos].vy;

            vector_z1=e3d->vertex_hash[pos].vz - e3d->vertex_hash[pos+1].vz;
            vector_z2=e3d->vertex_hash[pos-1].vz - e3d->vertex_hash[pos].vz;
        }

        if(j==2){

            vector_x1=e3d->vertex_hash[pos].vx - e3d->vertex_hash[pos-2].vx;
            vector_x2=e3d->vertex_hash[pos-1].vx - e3d->vertex_hash[pos].vx;

            vector_y1=e3d->vertex_hash[pos].vy - e3d->vertex_hash[pos-2].vy;
            vector_y2=e3d->vertex_hash[pos-1].vy - e3d->vertex_hash[pos].vy;

            vector_z1=e3d->vertex_hash[pos].vz - e3d->vertex_hash[pos-2].vz;
            vector_z2=e3d->vertex_hash[pos-1].vz - e3d->vertex_hash[pos].vz;
        }

        //nx = (vector_y1 * vector_z2) - (vector_z1 * vector_y2);
        //ny = -((vector_z2 * vector_x1) - (vector_x2 * vector_z1));
        //nz = (vector_x1 * vector_x2) - (vector_y1 * vector_x2);

        nx = vector_y1 * vector_z2 - vector_z1 * vector_y2;
		ny = vector_z1 * vector_x2 - vector_x1 * vector_z2;
		nz = vector_x1 * vector_y2 - vector_y1 * vector_x2;

        //combined_squares = (nx * nx) + (ny * ny) + (nz * nz);

        normalisation_factor = sqrt((nx * nx) + (ny * ny) + (nz * nz));

        e3d->vertex_hash[i].nx= nx / normalisation_factor;
        e3d->vertex_hash[i].ny= ny / normalisation_factor;
        e3d->vertex_hash[i].nz= nz / normalisation_factor;
    }
}
*/

