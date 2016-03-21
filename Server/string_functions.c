/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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
#include <stdbool.h> //support for boolean data type

#include "string_functions.h"
#include "logging.h"
#include "server_start_stop.h"


void ssnprintf(char *str, int max_len, char *fmt, ...){

    /** public function - see header */

    va_list args;
    va_start(args, fmt);

    int i=vsprintf(str, fmt, args);

    if(i>max_len-1){

        log_event(EVENT_ERROR, "ssnprint overrun with string [%s] in function %s: module %s: line %i", str, GET_CALL_INFO);
        log_text(EVENT_ERROR, "attempt to write string length [%i] to buffer length [%i]", i, max_len);

        fprintf(stderr, "ssnprint overrun. See error.log\n");

        stop_server();
    }

    va_end(args);
}


void str_trim_right(char *str_in){

    /** public function - see header */

    int i=0;
    int len=(int)strlen(str_in)-1;

    for(i=len; i>=0; i--) {
        if(!isspace(str_in[i])) break;
    }

    str_in[i+1]='\0';

    return;
}


void str_trim_left(char *str_in){

     /** public function - see header */

    size_t i=0;
    size_t len=strlen(str_in);

    for(i=0; i<len; i++){
        if(!isspace(str_in[i])) break;
    }

    memmove(str_in, str_in+i, len-i);
    str_in[len-i]='\0';

    return;
}


void str_conv_lower(char *str_in){

     /** public function - see header */

    for(size_t i=0; i<strlen(str_in); i++){

        if(str_in[i]>=65 && str_in[i]<=90) str_in[i]=(char)tolower(str_in[i]);
    }
}


void str_conv_upper(char *str_in){

     /** public function - see header */

    for(size_t i=0; i<strlen(str_in); i++){

        if(str_in[i]>=97 && str_in[i]<=122) str_in[i]=(char)toupper(str_in[i]);
    }
}


void str_remove_underscores(char *str_in){

    /** public function - see header */

    for(size_t i=0; i<strlen(str_in); i++){

        if(str_in[i]==ASCII_UNDERSCORE) str_in[i]=ASCII_SPACE;
    }
}


void extract_file_name(char *str_in, char *str_out){

    /** public function - see header */

    size_t len=strlen(str_in);
    size_t i;

    for(i=len; i>0; i--){

        if(str_in[i]=='/'){

            sprintf(str_out, "%s", str_in+i+1);
            return;
        }
    }

    sprintf(str_out, "%s", str_in+i);
}


void parse_line(char *line, char output[][MAX_LST_LINE_LEN]){

    /** public function - see header */

    size_t start=0;

    enum{

        BLANK,
        SENTENCE,
        WORD,
    };

    int status=0;
    int j=0;
    size_t i=0;

    for(i=0; i<strlen(line); i++){

        if(line[i]==34 && status==BLANK){

            status=SENTENCE;
            start=i+1;
        }

        else if(line[i]==34 && status==SENTENCE){

            status=BLANK;
            strncpy(&output[j][0], line+start, i-start);
            j++;
        }

        else if(line[i]>32 && status==BLANK){

            status=WORD;
            start=i;
        }

        if((line[i]==32 || line[i]==10) && status==WORD){

            status=BLANK;
            strncpy(&output[j][0], line+start, i-start);
            j++;
        }
    }

    if(i==strlen(line) && status==WORD){

            strncpy(&output[j][0], line+start, i-start);
            j++;
    }
}


int strcmp_upper(char *str1, char *str2){

    /** public function - see header */

    char u_str1[80]="";
    strcpy(u_str1, str1);
    str_conv_upper(u_str1);

    char u_str2[80]="";
    strcpy(u_str2, str2);
    str_conv_upper(u_str2);

    return strcmp(u_str1, u_str2);
}
