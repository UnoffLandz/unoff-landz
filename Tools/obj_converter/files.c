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
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>        //required for checking the md5 hash in the e3d file
#include <SDL/SDL_types.h>      //required for converting Uint32 to Uint16

#include "files.h"
#include "half.h"               // required for converting 16bit float values
#include "normal.h"             // required for converting compressed normal values

void str_trim_right(char *str_in){

    int i=0;
    int len=strlen(str_in)-1;

    for(i=len; i>=0; i--) {
        if(!isspace(str_in[i])) break;
    }

    str_in[i+1]='\0';

    return;

}

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
    unsigned char md5_digest[MD5_HASH_LEN];

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
    memcpy(md5_hash, md5_digest, MD5_HASH_LEN);
}
