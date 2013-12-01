#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void str_trim_right(char *str_in){

    int i=0;
    int len=strlen(str_in)-1;

    for(i=len; i>=0; i--) {
        if(!isspace(str_in[i])) break;
    }

    str_in[i+1]='\0';

    return;

}

void str_trim_left(char *str_in){

    int i=0;
    int len=strlen(str_in);

    for(i=0; i<len; i++){
        if(!isspace(str_in[i])) break;
    }

    memmove(str_in, str_in+i, len-i);
    str_in[len-i]='\0';

    return;
}

void str_conv_lower(char *str_in){

    int i;

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]>=65 && str_in[i]<=90) str_in[i]+=32;
    }

}

void str_conv_upper(char *str_in){

    int i;

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]>=97 && str_in[i]<=122)str_in[i]-=32;
    }

}

int count_str_island(char *str_in) {

    int i=0, j=0;
    int last_char=32;

    for(i=0; i<(int)strlen(str_in); i++){

        if(str_in[i]!=32 && last_char==32) {
            j++;
            last_char=str_in[i];
        }

        if(str_in[i]==32 && last_char!=32) {
            last_char=32;
        }

    }

    return j;
}

void get_str_island(char *str_in, char *str_out, int island_no) {

    int i=0, j=0;
    int last_char=32;

    int str_start=0;
    int str_end=strlen(str_in);

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]!=32 && last_char==32) {
            j++;

            if(j==island_no) {
                str_start=i;
            }

            last_char=str_in[i];
        }

        if(str_in[i]==32 && last_char!=32) {

            if(j<island_no){
                last_char=32;
            }
            else {
                str_end=i;
                break;
            }
        }
    }

    if(j==island_no) {
        memcpy(str_out, str_in+str_start, str_end-str_start);
        str_out[str_end+1]='\0';
    }
    else {
        str_out[0]='\0';
    }

}

void extract_file_name(char *str_in, char *str_out){

    int file_len=strlen(str_in);
    int i=0;

    for(i=file_len; str_in[i]!='/'; i--){
        if(i==0) break;
    }

    sprintf(str_out, "%s", str_in+i+1);
}

