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

#include <stdio.h> //support for FILE data type, fopen, fprintf, fflush, sprintf, printf
#include <string.h> //support for strcpy
#include <stdarg.h> // required for args support

#include "logging.h"
#include "date_time_functions.h"
#include "game_data.h"
#include "server_start_stop.h"

void write_to_file(char *file_name, char *text) {

    /** RESULT  : writes text string to file

        RETURNS : void

        PURPOSE : used by log_event and write_to_file functions

        NOTES   :
    **/

    FILE *file;

    //open existing file or create new file if none exists
    if((file=fopen(file_name, "a"))==NULL) {

        log_event(EVENT_INITIALISATION, "Unable to find existing file [%s]. Creating new one", file_name);

        //create file or log error
        if((file=fopen(file_name, "a"))==NULL) {

            log_event(EVENT_ERROR, "unable to create file [%s] in function log_to_file: module files.c", file_name);
            stop_server();
        }
    }

    //write to the file or log error
    if(!fprintf(file, "%s\n", text)){

        log_event(EVENT_ERROR, "unable to write to file [%s] in function log_to_file: module files.c", file_name);
        stop_server();
    }

    fflush(file);

    fclose(file);
}

void clear_file(char *file_name){

    /** RESULT  : clears the text file specified by file_name

        RETURNS : void

        PURPOSE : used by initialise_logs function

        NOTES   :
    **/

    FILE *file;

    //open the file in append mode so we can detect if there's an existing file
    if((file=fopen(file_name, "a"))!=NULL){

        //if there's an existing file, close and reopen it in write mode (has the effect of clearing the existing content)
        fclose(file);

        if((file=fopen(file_name, "w"))==NULL){

            //If file can't be reopened then we've got a serious problem. No point in trying to log it to the error log
            //as that could be subject to the same problem. Hence, send a message to the console and close server gracefully
            printf("unable to clear file [%s] in function clear_file: module logging.c\n", file_name);
            stop_server();
        }
    }

    log_event(EVENT_INITIALISATION, "cleared file [%s]", file_name);
}

void get_event_log_file(int event_type, char *file_name){

    /** RESULT  : returns the log file corresponding to the event type

        RETURNS : void

        PURPOSE : used in functions: log_event and log_text

        NOTES   :
    **/

    switch(event_type){

        case EVENT_NEW_CHAR: strcpy(file_name, CHARACTER_LOG_FILE_NAME); break;
        case EVENT_ERROR: strcpy(file_name, ERROR_LOG_FILE_NAME); break;
        case EVENT_SESSION: strcpy(file_name, SESSION_LOG_FILE_NAME); break;
        case EVENT_CHAT: strcpy(file_name, CHAT_LOG_FILE_NAME); break;
        case EVENT_MOVE_ERROR: strcpy(file_name, MOVE_LOG_FILE_NAME); break;
        case EVENT_INITIALISATION: strcpy(file_name, INITIALISATION_LOG_FILE_NAME); break;
        case EVENT_MAP_LOAD:strcpy(file_name, MAP_LOAD_LOG_FILE_NAME); break;
        case EVENT_PACKET:strcpy(file_name, PACKET_LOG_FILE_NAME); break;

        //direct any unknown events to error log
        default: strcpy(file_name, ERROR_LOG_FILE_NAME); break;
    }
}

void write_to_log(int event_type, char *text){

    /** RESULT  : writes text to the log file corresponding with an event type

        RETURNS : void

        PURPOSE : provides a generic function that allows for both formatted and unformatted entries
        to be written to a log file. Used by functions log_event and log_text.

        NOTES   :
    **/

    char file_name[80]="";

    //get the log file name
    get_event_log_file(event_type, file_name);

    //write to the log file
    write_to_file(file_name, text);
}

void log_text(int event_type, char *fmt, ...){

    /** public function - see header */

    char text_out[1024]="";

    va_list args;
    va_start(args, fmt);
    vsprintf(text_out, fmt, args);

    va_end(args);

    write_to_log(event_type, text_out);
}

void log_event(int event_type, char *fmt, ...){

    /** public function - see header */

    char text_in[1024]="";
    char text_out[1024]="";

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    va_list args;
    va_start(args, fmt);

    vsprintf(text_in, fmt, args);

    //get the date stamps
    get_time_stamp_str(time(NULL), time_stamp_str);
    get_date_stamp_str(time(NULL), date_stamp_str);

    //determine the event format
    switch(event_type){

        case EVENT_NEW_CHAR:
            sprintf(text_out, "[%s][%s] Character - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_ERROR:
            sprintf(text_out, "[%s][%s] Error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_SESSION:
            sprintf(text_out, "[%s][%s] session - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_CHAT:
            sprintf(text_out, "[%s][%s] Event - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_MOVE_ERROR:
            sprintf(text_out, "[%s][%s] Move-error - %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_INITIALISATION:
            sprintf(text_out, "[%s][%s] %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_MAP_LOAD:
            sprintf(text_out, "[%s][%s] %s", date_stamp_str, time_stamp_str, text_in);
        break;

        case EVENT_PACKET:
            sprintf(text_out, "%s", text_in);
        break;

        default:
            //we should never reach here as unknown events are directed by function get_event_log_file to the
            //the error log. However, just in case the unthinkable occurs, we capture it here and terminate the
            //server gracefully
            printf("unknown event type [%i] detected in function log_event: module logging.c\n", event_type);
            stop_server();
            break;
    }

    va_end(args);

    write_to_log(event_type, text_out);
}

void initialise_logs(){

    /** public function - see header */

    char time_stamp_str[9]="";
    char verbose_date_stamp_str[50]="";

    FILE *file;

    //as we need to append an opening entry 'SERVER BOOT-UP COMMENCED' to the initialisation log file, we need
    //to do this immediately after that log file has been cleared. We can't do this using the clear_file function
    //as the 'cleared file' entry will be appended before the opening entry, hence, we need to clear the log and
    //append the opening entry here.

    //open the file in append mode so we can detect if there's an existing file
    if((file=fopen(INITIALISATION_LOG_FILE_NAME, "a"))!=NULL){

        //if there's an existing file, close and reopen it in write mode (has the effect of clearing the existing content)
        fclose(file);

        if((file=fopen(INITIALISATION_LOG_FILE_NAME, "w"))==NULL){

            //If the initialisation log file can't be reopened then we've got a serious problem. No point in trying to
            //log it to the error log as that hasn't been cleared yet and could be subject to the same problem. Hence,
            //send a message to the console and close server gracefully
            printf("unable to clear initialisation log file [%s] in function initialise_logs: module logging.c\n", INITIALISATION_LOG_FILE_NAME);
            stop_server();
        }

        get_time_stamp_str(game_data.server_start_time, time_stamp_str);
        get_verbose_date_str(game_data.server_start_time, verbose_date_stamp_str);
        log_text(EVENT_INITIALISATION, "SERVER BOOT-UP at %s on %s", time_stamp_str, verbose_date_stamp_str);

        // inserts a blank line to create a logical separator with subsequent log entries
        log_text(EVENT_INITIALISATION, "");
    }

    log_text(EVENT_INITIALISATION, "initialising log files...");
    log_event(EVENT_INITIALISATION, "cleared file [%s]", INITIALISATION_LOG_FILE_NAME);

    //we can now use the clear_file function to initialise the other log files
    clear_file(ERROR_LOG_FILE_NAME);
    clear_file(CHARACTER_LOG_FILE_NAME);
    clear_file(SESSION_LOG_FILE_NAME);
    clear_file(CHAT_LOG_FILE_NAME);
    clear_file(MOVE_LOG_FILE_NAME);
    clear_file(MAP_LOAD_LOG_FILE_NAME);
    clear_file(PACKET_LOG_FILE_NAME);

    // inserts a blank line to create a logical separator with subsequent log entries
    log_text(EVENT_INITIALISATION, "");
}

void log_sqlite_error(char *error_type, const char *function_name, const char *module_name, int line_number, int return_code, char *sql_stmt){

        /** public function - see header */

        //we use const char in prototype so we can use __func__ and __FILE__ without causing compiler warnings

        log_event(EVENT_ERROR, "%s at line [%i] in function %s: module %s", error_type, line_number, function_name, module_name);
        log_text(EVENT_ERROR, "sql statement [%s]", sql_stmt);

        switch(return_code) {

            case 1:
            log_text(EVENT_ERROR, "sqlite return code 1: SQLITE_ERROR: sql error or missing database");
            break;

            case 19:
            log_text(EVENT_ERROR, "sqlite return code 19: SQLITE_CONSTRAINT: attempt to overwrite an existing record");
            break;

            default:
            log_text(EVENT_ERROR, "sqlite return code [%i]", return_code);
            break;
        }

        stop_server();
}
