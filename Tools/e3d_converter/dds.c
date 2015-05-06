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
#include <assert.h>

#include "files.h" // required for get_file_size function
#include "dds.h"

void read_dds(char *filename, char *pf_fourcc){

    FILE *file;

    struct {

        char dw_fourcc[4];
        int header_length;
        int dw_flags;
        int height;
        int width;
        int pitchorlinearsize;
        int depth;
        int mipmapcount;

        unsigned char reserved1[44];

        int pf_header_length;
        int pf_flags;
        char pf_fourcc[4];
        int pf_rgb_bitcount;
        int pf_r_bitmask;
        int pf_g_bitmask;
        int pf_b_bitmask;
        int pf_a_bitmask;

        int caps_flags;
        int caps2_flags;
        int caps3_flags;
        int caps4_flags;

        int reserved2;
    }dds;

    //open the file
    if((file=fopen(filename, "rb"))==NULL) {

        printf("file [%s] not found in function read_dds\n", filename);
        exit(EXIT_FAILURE);
    }

    // read the header into the buffer
    if(fread(&dds, sizeof(dds), 1, file)!=1){

        printf("unable to read header for file [%s] in function read_dds\n", filename);
        exit (EXIT_FAILURE);
    }

    //return the magic number so as other functions are able to act on this
    strncpy(pf_fourcc, dds.pf_fourcc, 4);
}

void report_dds_data(int report, char *report_filename, int materials_count, struct materials_hash_type2 *materials_hash){

    FILE *file;
    FILE *file_out;
    char filename[80]="";
    int curr, end, buffer_size, file_size;
    int n_width, n_height, n_size, n_offset;
    int alpha_count=0;
    int block_count=0;
    int block_size=0;
    int i;

    struct {

        char dw_fourcc[4];
        int header_length;
        int dw_flags;
        int height;
        int width;
        int pitchorlinearsize;
        int depth;
        int mipmapcount;

        unsigned char reserved1[44];

        int pf_header_length;
        int pf_flags;
        char pf_fourcc[4];
        int pf_rgb_bitcount;
        int pf_r_bitmask;
        int pf_g_bitmask;
        int pf_b_bitmask;
        int pf_a_bitmask;

        int caps_flags;
        int caps2_flags;
        int caps3_flags;
        int caps4_flags;

        int reserved2;
    }dds;


    for(i=0; i<materials_count; i++){

        strcpy(filename, materials_hash->name);

        //open the file
        if((file=fopen(filename, "rb"))==NULL) {

            printf("file [%s] not found in function report_dds_data\n", filename);
            exit(EXIT_FAILURE);
        }

        // read the header into the buffer
        if(fread(&dds, sizeof(dds), 1, file)!=1){

            printf("unable to read header for file [%s] in function report_dds_data\n", filename);
            exit (EXIT_FAILURE);
        }

        //calculate the size of the buffer that we need to hold the texture data
        curr = ftell (file);
        fseek (file, 0, SEEK_END);
        end = ftell (file);
        fseek (file, curr, SEEK_SET);
        buffer_size = end - curr;

        //create the buffer to hold the texture data and zero the memory
        unsigned char image_data[buffer_size];
        memset(image_data, 0, buffer_size);

        int factor=0;

        if(strncmp("DXT1 ", dds.pf_fourcc,4)==0) {
            factor=2;
            block_size=8;
        }
        else if(strncmp("DXT3 ", dds.pf_fourcc,4)==0) {
            factor=4;
            block_size=16;
        }
        else if(strncmp("DXT5 ", dds.pf_fourcc,4)==0) {
            factor=4;
            block_size=16;
        }
        else {
            printf("unknown pixel format for file [%s] in function report_dds_data\n", filename);
            exit(EXIT_FAILURE);
        }

        file_size=get_file_size(filename);
        block_count=dds.pitchorlinearsize/block_size;

        if(fread(image_data, sizeof(image_data), 1, file)!=1){

            printf("unable to read data for file [%s] in function report_dds_data\n", filename);
            exit (EXIT_FAILURE);
        }

        //close the file
        fclose(file);

        //calculate the size and offset of each mipmap
        struct {
            int size;
            int offset;
        }mipmap[dds.mipmapcount];

        n_width=dds.width;
        n_height=dds.height;
        n_size=0;
        n_offset=0;

        for(i=0; i<dds.mipmapcount; ++i ){

            if( n_width  == 0 ) n_width  = 1;
            if( n_height == 0 ) n_height = 1;

            n_size = ((n_width+3)/4) * ((n_height+3)/4) * factor;

            n_offset += n_size;
            n_width  /= 2;
            n_height /= 2;

            mipmap[i].size=n_size*4;
            mipmap[i].offset=n_offset;
        }

        //make sure the combined size of the mipmaps plus the header equals the filesize
        assert(n_offset*4+EXPECTED_DDS_HEADER_SIZE==file_size);

        //calculate alpha content in DXT3 file
        if(strncmp("DXT3 ", dds.pf_fourcc,4)==0) {

            alpha_count=0;

            for(i=0; i<dds.pitchorlinearsize; i+=block_size){

                int alpha_0=image_data[i];
                int alpha_1=image_data[i+1];

                if(alpha_0<alpha_1) alpha_count++;
            }
        }

        //report the dds data
        switch(report){

            case 0:
            //do nothing
            break;

            case 1:
            printf("\n\nFile                          [%s]\n", filename);

            printf("magic number                  [%c] [%c] [%c] [%c]\n", dds.dw_fourcc[0], dds.dw_fourcc[1], dds.dw_fourcc[2], dds.dw_fourcc[3]);

            printf("header length                 [%i]\n", dds.header_length);

            printf("\n");

            printf("dw flag caps                  "); if (dds.dw_flags & 0x1) printf("X\n"); else printf("-\n");
            printf("dw flag height                "); if (dds.dw_flags & 0x2) printf("X\n"); else printf("-\n");
            printf("dw flag width                 "); if (dds.dw_flags & 0x4) printf("X\n"); else printf("-\n");
            printf("dw flag pitch                 "); if (dds.dw_flags & 0x8) printf("X\n"); else printf("-\n");
            printf("dw flag pixel format          "); if (dds.dw_flags & 0x1000) printf("X\n"); else printf("-\n");
            printf("dw flag mip map count         "); if (dds.dw_flags & 0x20000) printf("X\n"); else printf("-\n");
            printf("dw flag linear size           "); if (dds.dw_flags & 0x80000) printf("X\n"); else printf("-\n");
            printf("dw flag depth                 "); if (dds.dw_flags & 0x800000) printf("X\n"); else printf("-\n");

            printf("\n");

            if(dds.height % 4 ==0) {
                printf("height                        [%i] pixels\n", dds.height);
            }
            else {
                printf("height                        [%i] pixels  ...WARNING height is not a multiple of 4\n", dds.height);

            }

            if(dds.width % 4==0){
                printf("width                         [%i] pixels\n", dds.width);
            }
            else {
                printf("width                         [%i] pixels  ...WARNING width is not a multiple of 4\n", dds.width);
            }

            printf("pitch or linear size          [%i] bytes\n", dds.pitchorlinearsize);
            printf("depth                         [%i]\n", dds.depth);
            printf("mipmap count                  [%i]\n", dds.mipmapcount);
            printf("pixel format header length    [%i]\n", dds.pf_header_length);

            printf("\n");

            printf("pf flag alpha pixels          "); if (dds.pf_flags & 0x1) printf("X\n"); else printf("-\n");
            printf("pf flag legacy alpha pixels   "); if (dds.pf_flags & 0x2) printf("X\n"); else printf("-\n");
            printf("pf flag compressed rgb data   "); if (dds.pf_flags & 0x4) printf("X\n"); else printf("-\n");
            printf("pf flag uncompressed rgb data "); if (dds.pf_flags & 0x40) printf("X\n"); else printf("-\n");
            printf("pf flag legacy yuv data       "); if (dds.pf_flags & 0x200) printf("X\n"); else printf("-\n");
            printf("pf flag legacy luminance data "); if (dds.pf_flags & 0x20000) printf("X\n"); else printf("-\n");

            printf("\n");

            printf("pixel format magic number     [%c] [%c] [%c] [%c]\n", dds.pf_fourcc[0], dds.pf_fourcc[1], dds.pf_fourcc[2], dds.pf_fourcc[3]);
            printf("rgb bit count                 [%i]\n", dds.pf_rgb_bitcount);
            printf("red or y bit mask             [%i]\n", dds.pf_r_bitmask);
            printf("green or u bit mask           [%i]\n", dds.pf_g_bitmask);
            printf("blue or v bit mask            [%i]\n", dds.pf_b_bitmask);
            printf("alpha bit mask                [%i]\n", dds.pf_a_bitmask);

            printf("\n");

            printf("caps 1 flag complex           "); if (dds.caps_flags & 0x8) printf("X\n"); else printf("-\n");
            printf("caps 1 flag mipmap present    "); if (dds.caps_flags & 0x4000000) printf("X\n"); else printf("-\n");
            printf("caps 1 flag texture present   "); if (dds.caps_flags & 0x1000) printf("X\n"); else printf("-\n");

            printf("\n");

            printf("caps 2 flag cube map          "); if (dds.caps2_flags & 0x200) printf("X\n"); else printf("-\n");
            printf("caps 2 flag positive x        "); if (dds.caps2_flags & 0x400) printf("X\n"); else printf("-\n");
            printf("caps 2 flag negative x        "); if (dds.caps2_flags & 0x800) printf("X\n"); else printf("-\n");
            printf("caps 2 flag positive y        "); if (dds.caps2_flags & 0x1000) printf("X\n"); else printf("-\n");
            printf("caps 2 flag negative y        "); if (dds.caps2_flags & 0x2000) printf("X\n"); else printf("-\n");
            printf("caps 2 flag positive z        "); if (dds.caps2_flags & 0x4000) printf("X\n"); else printf("-\n");
            printf("caps 2 flag negative z        "); if (dds.caps2_flags & 0x8000) printf("X\n"); else printf("-\n");
            printf("caps 2 flag volume texture    "); if (dds.caps2_flags & 0x200000) printf("X\n"); else printf("-\n");

            printf("\n");

            printf("caps 3 flags                  [%i] ... should be unused\n", dds.caps3_flags);
            printf("caps 4 flags                  [%i] ... should be unused\n", dds.caps4_flags);
            printf("reserved 2                    [%i] ... should be unused\n", dds.reserved2);

            printf("\n");

            printf("file size                       [%i]\n", file_size);
            printf("texture data size               [%i]\n", buffer_size);
            printf("total blocks                    [%i]\n", block_count);

            if(strncmp("DXT3 ", dds.pf_fourcc,4)==0) printf( "alpha blocks                    [%i] [%i%%]\n", alpha_count, alpha_count*100/block_count);

            printf("\nmip maps...\n");

            for(i=0; i<dds.mipmapcount; ++i ){

                printf("mipmap [%i] size [%i] offset [%i]\n", i, mipmap[i].size, mipmap[i].offset);
            }
            break;

            case 2:
            if((file_out=fopen(report_filename, "a"))==NULL) {

                printf("file [%s] not found in function read_dds\n", report_filename);
                exit(EXIT_FAILURE);
            }

            fprintf(file_out, "\n\nFile                          [%s]\n", filename);

            fprintf(file_out, "magic number                  [%c] [%c] [%c] [%c]\n", dds.dw_fourcc[0], dds.dw_fourcc[1], dds.dw_fourcc[2], dds.dw_fourcc[3]);

            fprintf(file_out, "header length                 [%i]\n", dds.header_length);

            fprintf(file_out, "\n");

            fprintf(file_out, "dw flag caps                  "); if (dds.dw_flags & 0x1) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag height                "); if (dds.dw_flags & 0x2) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag width                 "); if (dds.dw_flags & 0x4) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag pitch                 "); if (dds.dw_flags & 0x8) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag pixel format          "); if (dds.dw_flags & 0x1000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag mip map count         "); if (dds.dw_flags & 0x20000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag linear size           "); if (dds.dw_flags & 0x80000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "dw flag depth                 "); if (dds.dw_flags & 0x800000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

            fprintf(file_out, "\n");

            if(dds.height % 4 ==0) {
                fprintf(file_out, "height                        [%i] pixels\n", dds.height);
            }
            else {
                fprintf(file_out, "height                        [%i] pixels  ...WARNING height is not a multiple of 4\n", dds.height);

            }

            if(dds.width % 4==0){
                fprintf(file_out, "width                         [%i] pixels\n", dds.width);
            }
            else {
                fprintf(file_out, "width                         [%i] pixels  ...WARNING width is not a multiple of 4\n", dds.width);
            }

            fprintf(file_out, "pitch or linear size          [%i] bytes\n", dds.pitchorlinearsize);
            fprintf(file_out, "depth                         [%i]\n", dds.depth);
            fprintf(file_out, "mipmap count                  [%i]\n", dds.mipmapcount);
            fprintf(file_out, "pixel format header length    [%i]\n", dds.pf_header_length);

            fprintf(file_out, "\n");

            fprintf(file_out, "pf flag alpha pixels          "); if (dds.pf_flags & 0x1) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "pf flag legacy alpha pixels   "); if (dds.pf_flags & 0x2) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "pf flag compressed rgb data   "); if (dds.pf_flags & 0x4) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "pf flag uncompressed rgb data "); if (dds.pf_flags & 0x40) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "pf flag legacy yuv data       "); if (dds.pf_flags & 0x200) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "pf flag legacy luminance data "); if (dds.pf_flags & 0x20000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

            fprintf(file_out, "\n");

            fprintf(file_out, "pixel format magic number     [%c] [%c] [%c] [%c]\n", dds.pf_fourcc[0], dds.pf_fourcc[1], dds.pf_fourcc[2], dds.pf_fourcc[3]);
            fprintf(file_out, "rgb bit count                 [%i]\n", dds.pf_rgb_bitcount);
            fprintf(file_out, "red or y bit mask             [%i]\n", dds.pf_r_bitmask);
            fprintf(file_out, "green or u bit mask           [%i]\n", dds.pf_g_bitmask);
            fprintf(file_out, "blue or v bit mask            [%i]\n", dds.pf_b_bitmask);
            fprintf(file_out, "alpha bit mask                [%i]\n", dds.pf_a_bitmask);

            fprintf(file_out, "\n");

            fprintf(file_out, "caps 1 flag complex           "); if (dds.caps_flags & 0x8) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 1 flag mipmap present    "); if (dds.caps_flags & 0x4000000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 1 flag texture present   "); if (dds.caps_flags & 0x1000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

            fprintf(file_out, "\n");

            fprintf(file_out, "caps 2 flag cube map          "); if (dds.caps2_flags & 0x200) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag positive x        "); if (dds.caps2_flags & 0x400) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag negative x        "); if (dds.caps2_flags & 0x800) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag positive y        "); if (dds.caps2_flags & 0x1000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag negative y        "); if (dds.caps2_flags & 0x2000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag positive z        "); if (dds.caps2_flags & 0x4000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag negative z        "); if (dds.caps2_flags & 0x8000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");
            fprintf(file_out, "caps 2 flag volume texture    "); if (dds.caps2_flags & 0x200000) fprintf(file_out, "X\n"); else fprintf(file_out, "-\n");

            fprintf(file_out, "\n");

            fprintf(file_out, "caps 3 flags                  [%i] ... should be unused\n", dds.caps3_flags);
            fprintf(file_out, "caps 4 flags                  [%i] ... should be unused\n", dds.caps4_flags);
            fprintf(file_out, "reserved 2                    [%i] ... should be unused\n", dds.reserved2);

            fprintf(file_out, "\n");

            fprintf(file_out, "file size                       [%i]\n", file_size);
            fprintf(file_out, "texture data size               [%i]\n", buffer_size);
            fprintf(file_out, "total blocks                    [%i]\n", block_count);
            if(strncmp("DXT3 ", dds.pf_fourcc,4)==0) fprintf(file_out, "alpha blocks                    [%i] [%i%%]\n", alpha_count, alpha_count*100/block_count);
            fprintf(file_out, "\nmip maps...\n");

            for(i=0; i<dds.mipmapcount; ++i ){

                fprintf(file_out, "mipmap [%i] size [%i] offset [%i]\n", i, mipmap[i].size, mipmap[i].offset);
            }

            fclose(file_out);
            break;

            default:
            printf("unknown value for variable 'report' in function report_dds_data\n");
            exit(EXIT_FAILURE);
        }
    }
}


/*
int read_mipmap_data(char *filename, int report, char *report_filename){

    FILE *file;
    FILE *file_out;
    int i=0;

    struct {
        int size;
        int offset;
    }mipmap[20];

    //report values
    //0 - silent
    //1 - std out
    //2 - named file

    struct {

        char dw_fourcc[4];
        int header_length;
        int dw_flags;
        int height;
        int width;
        int pitchorlinearsize;
        int depth;
        int mipmapcount;

        unsigned char reserved1[44];

        int pf_header_length;
        int pf_flags;
        char pf_fourcc[4];
        int pf_rgb_bitcount;
        int pf_r_bitmask;
        int pf_g_bitmask;
        int pf_b_bitmask;
        int pf_a_bitmask;

        int caps_flags;
        int caps2_flags;
        int caps3_flags;
        int caps4_flags;

        int reserved2;
    }dds;

    switch(report){

        case 0:
        file_out=NULL;
        break;

        case 1:
        file_out=stdout;
        break;

        case 2:
        if((file_out=fopen(report_filename, "a"))==NULL) {

            printf("file [%s] not found\n", report_filename);
            exit(EXIT_FAILURE);
        }
        break;

        default:
        printf("unknown value for variable 'report' in function read_dds_header\n");
        exit(EXIT_FAILURE);
    }

    //open the file
    if((file=fopen(filename, "rb"))==NULL) {

        fprintf(file_out, "file [%s] not found\n", filename);
        exit(EXIT_FAILURE);
    }

    // read the header into the buffer
    if(fread(&dds, sizeof(dds), 1, file)!=1){


        fprintf(file_out, "unable to read header for file [%s]\n", filename);
        exit (EXIT_FAILURE);
    }

    int curr = ftell (file);
    fseek (file, 0, SEEK_END);
    int end = ftell (file);
    fseek (file, curr, SEEK_SET);
    int buffersize = end - curr;

    int filesize=get_file_size(filename);

    printf("buffer size %i\n", buffersize);
    printf("file size %i\n", filesize);
    printf("offset %i\n", get_file_size(filename)- buffersize);

    int factor=0;
    int blocksize=0;

    if(strncmp("DXT1 ", dds.pf_fourcc,4)==0) {
        factor=2;
        blocksize=8;
        printf("DXT1\n");
    }
    else if(strncmp("DXT3 ", dds.pf_fourcc,4)==0) {
        factor=4;
        blocksize=16;
        printf("DXT3\n");
    }
    else if(strncmp("DXT5 ", dds.pf_fourcc,4)==0) {
        factor=4;
        blocksize=16;
        printf("DXT5\n");
    }
    else {
        printf("unknown\n");
        exit(1);
    }

    printf("mipmap count %i\n", dds.mipmapcount);
    printf("linear size %i\n\n", dds.pitchorlinearsize);

    unsigned char image_data[buffersize];
    memset(image_data, 0, buffersize);

    if(fread(image_data, sizeof(image_data), 1, file)!=1){

        fprintf(file_out, "unable to read data for file [%s]\n", filename);
        exit (EXIT_FAILURE);
    }

    //close the file
    fclose(file);

    int n_width=dds.width;
    int n_height=dds.height;
    int n_size=0;
    int n_offset=0;

    //find the offset for each mipmap
    for(i=0; i<dds.mipmapcount; ++i ){

            if( n_width  == 0 ) n_width  = 1;
            if( n_height == 0 ) n_height = 1;

            n_size = ((n_width+3)/4) * ((n_height+3)/4) * factor;

            printf("Mipmap %i size %i\n", i, n_size*4);

            n_offset += n_size;
            n_width  /= 2;
            n_height /= 2;

            mipmap[i].size=n_size*4;
            mipmap[i].offset=n_offset;
    }

    assert(n_offset*4+EXPECTED_DDS_HEADER_SIZE==filesize);
    printf("total %i   total x 4 %i\n", n_offset, n_offset*4);

    int alpha_count=0;

    for(i=0; i<dds.pitchorlinearsize; i+=128){

        int alpha_0=Uint16_to_dec(image_data+i);
        int alpha_1=Uint16_to_dec(image_data+i+1);

        if(alpha_0<alpha_1) alpha_count++;

        //printf("%i %i\n", alpha_0, alpha_1);
    }

    int block_count=dds.pitchorlinearsize/128;

    printf("alpha count %i, block count %i alpha percent %i%%\n", alpha_count, block_count, alpha_count*100/block_count);

    if(alpha_count>0) return 1;

    return 0;
}
*/
