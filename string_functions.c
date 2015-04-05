/******************************************************************************************************************
	Copyright 2014 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/

#include <stdio.h> //support for vsprintf and sprintf
#include <string.h> //support for strlen
#include <ctype.h> // support for isspace
#include <stdarg.h> // support for args

#include "string_functions.h"
#include "logging.h"
#include "server_start_stop.h"


void ssnprintf(char *str, int max_len, char *fmt, ...){

    /** public function - see header */

    va_list args;
    va_start(args, fmt);

    int i=vsprintf(str, fmt, args);

    if(i>max_len-1){

        log_event(EVENT_ERROR, "ssnprint overrun with string [%s] in function %s: module %s: line %i", str, __func__, __FILE__, __LINE__);
        log_text(EVENT_ERROR, "attempt to write string length [%i] to buffer length [%i]", i, max_len);
        stop_server();
    }

    va_end(args);
}

void str_trim_right(char *str_in){

    /** public function - see header */

    int i=0;
    int len=strlen(str_in)-1;

    for(i=len; i>=0; i--) {
        if(!isspace(str_in[i])) break;
    }

    str_in[i+1]='\0';

    return;
}

void str_trim_left(char *str_in){

     /** public function - see header */

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

     /** public function - see header */

    int i;

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]>=65 && str_in[i]<=90) str_in[i]+=32;
    }
}

void str_conv_upper(char *str_in){

     /** public function - see header */

    int i;

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]>=97 && str_in[i]<=122)str_in[i]-=32;
    }
}

void str_remove_underscores(char *str_in){

    /** public function - see header */

    int i;

    for(i=0; i<(int) strlen(str_in); i++){

        if(str_in[i]==ASCII_UNDERSCORE) str_in[i]=ASCII_SPACE;
    }
}

int count_str_island(char *str_in) {

    /** public function - see header */

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

     /** public function - see header */

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

    /** public function - see header */

    int len=strlen(str_in);
    int i=0;

    for(i=len; i>0; i--){

        if(str_in[i]=='/'){

            sprintf(str_out, "%s", str_in+i+1);
            return;
        }
    }

    sprintf(str_out, "%s", str_in+i);
}

/*
void filter_str_range(char *str_in, int min_ascii, int max_ascii){

    int i=0;
    int clean=0;

    for(i=0; i<(int)strlen(str_in); i++){

        str_in[i-clean]=str_in[i];

        if(str_in[i]<min_ascii || str_in[i]>max_ascii) clean++;

    }
    str_in[i-clean]='\0';
}
*/
