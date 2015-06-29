#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "file.h"

void get_filename_prefix(char *filename, char *prefix){

    /*** split a filename from its prefix (prefix is returned through 'char *prefix') ***/

    int i=0;

    for(i=0; i<(int)strlen(filename); i++){

        if(filename[i]=='.')break;
    }

    strncpy(prefix, filename, i);
    prefix[i]=0;// add null terminator
}

void get_filename_suffix(char *filename, char *suffix){

    /*** split a suffix from its filename (suffix is returned through 'char *suffix') ***/

    int i=0;

    for(i=strlen(filename); i>=0; i--){

        if(filename[i]=='.')break;
    }

    strncpy(suffix, filename+i, FILENAME_SUFFIX_LENGTH);
}

int get_file_size(char *filename){

    /*** returns the size of the file ***/

    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file\n");
        exit(EXIT_FAILURE);
    }

    //count the bytes until an EOF marker is found
    int count=0;

    while(fgetc(file) != EOF) {
        count++;
    }

    fclose(file);

    return count;
}

void get_md5(char *filename, int offset, unsigned char *md5_hash){

    /*** calculates the md5 hash of a file (md5 hash is returned through 'unsigned char *md5_hash) ***/

    unsigned char md5_digest[MD5_HASH_LENGTH];

    //bounds check the data size
    int file_len=get_file_size(filename);

    if (file_len-offset>MD5_MAX_DATA){

        printf("data size [%i] in file [%s] exceeds array maximum of [%i]\n", file_len-offset, filename, MD5_MAX_DATA);
        exit(EXIT_FAILURE);
    }

    //open the file
    FILE *file;

    if((file=fopen(filename, "rb"))==NULL) {

        printf("unable to open file [%s] in function check_md5: module files.c", filename);
        exit(EXIT_FAILURE);
    }

    //read the offset
    unsigned char byte[MD5_MAX_DATA];

    if(fread(&byte, offset, 1, file)!=1){

        printf("unable to read file [%s] in function check_md5: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    //read the file data into an array
    if(fread(&byte, file_len-offset, 1, file)!=1){

        printf("unable to read file [%s] in function check_md5: module files.c", filename);
        exit (EXIT_FAILURE);
    }

    //calculate md5 of the array
    MD5((unsigned char*)&byte, file_len-offset, (unsigned char*)&md5_digest);

    //copy the md5 hash to a new string (we could return a pointer or do some fancy casting but we hate that shit)
    memcpy(md5_hash, md5_digest, MD5_HASH_LENGTH);
}
