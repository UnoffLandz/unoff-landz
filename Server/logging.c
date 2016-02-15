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

#include <stdio.h> //support for FILE data type, fopen, fprintf, fflush, sprintf, printf
#include <string.h> //support for strcpy
#include <stdarg.h> //required for args support

#include "logging.h"
#include "date_time_functions.h"
#include "game_data.h"
#include "server_start_stop.h"
#include "db/database_functions.h"
#include "clients.h"
#include "packet.h"
#include "string_functions.h"

struct protocol_type protocol[] = {

    {"RAW_TEXT", "RAW_TEXT"}, //0
    {"", "MOVE_TO"}, //1
    {"ADD_ACTOR", "SEND_PM"}, //2
    {"YOU_ARE", ""},    //3
    {"SYNC_CLOCK", ""}, //4
    {"NEW_MINUTE", "GET_PLAYER_INFO"}, //5
    {"REMOVE_ACTOR", ""}, //6
    {"CHANGE_MAP", "SIT_DOWN"}, //7
    {"", "SEND_ME_MY_ACTORS"}, //8
    {"", "SEND_OPENING_SCREEN"}, //9
    {"", "SEND_VERSION"}, //10
    {"", ""}, //11
    {"", ""}, //12
    {"", ""}, //13
    {"", "HEARTBEAT"}, //14
    {"", ""}, //15
    {"", "USE_OBJECT"}, //16
    {"", ""}, //17
    {"HERE_YOUR_STATS", ""},//18
    {"HERE_YOUR_INVENTORY", "LOOK_AT_INVENTORY_ITEM"}, //19
    {"", "MOVE_INVENTORY_ITEM"},//20
    {"GET_NEW_INVENTORY_ITEM", "HARVEST"},//21
    {"", "DROP_ITEM"}, //22
    {"HERE_YOUR_GROUND_ITEMS", "PICK_UP_ITEM"},//23
    {"GET_NEW_GROUND_ITEM", ""},//24
    {"", "INSPECT_BAG"}, //25
    {"CLOSE_BAG", ""},//26
    {"GET_NEW_BAG", "LOOK_AT_MAP_OBJECT"},//27
    {"GET_BAGS_LIST", ""}, //28
    {"DESTROY_BAG", ""},//29
    {"", ""}, //30
    {"", ""}, //31
    {"", ""}, //32
    {"", ""}, //33
    {"", ""}, //34
    {"", ""}, //35
    {"", ""}, //36
    {"", ""}, //37
    {"", ""}, //38
    {"", ""}, //39
    {"", ""}, //40
    {"", ""}, //41
    {"", ""}, //42
    {"", ""}, //43
    {"", ""}, //44
    {"", ""}, //45
    {"", ""}, //46
    {"", ""}, //47
    {"", ""}, //48
    {"SEND_PARTIAL_STATS", ""}, //49
    {"", ""}, //50
    {"ADD_NEW_ENHANCED_ACTOR", ""}, //51
    {"ACTOR_WEAR_ITEM", ""}, //52
    {"ACTOR_UNWEAR_ITEM", ""}, //53
    {"", ""}, //54
    {"", ""}, //55
    {"", ""}, //56
    {"", ""}, //57
    {"", ""}, //58
    {"", ""}, //59
    {"", "PING_RESPONSE"}, //60
    {"", "SET_ACTIVE_CHANNEL"}, //61
    {"", ""}, //62
    {"", ""}, //63
    {"", ""}, //64
    {"", ""}, //65
    {"", ""}, //66
    {"", ""}, //67
    {"", ""}, //68
    {"", ""}, //69
    {"", ""}, //70
    {"GET_ACTIVE_CHANNELS", ""}, //71
    {"", ""}, //72
    {"", ""}, //73
    {"", ""}, //74
    {"", ""}, //75
    {"", ""}, //76
    {"", ""}, //77
    {"", ""}, //78
    {"", ""}, //79
    {"", ""}, //80
    {"", ""}, //81
    {"", ""}, //82
    {"", ""}, //83
    {"", ""}, //84
    {"", ""}, //85
    {"", ""}, //86
    {"", ""}, //87
    {"", ""}, //88
    {"", ""}, //89
    {"", ""}, //90
    {"", ""}, //91
    {"", ""}, //92
    {"", ""}, //93
    {"", ""}, //94
    {"", ""}, //95
    {"", ""}, //96
    {"", ""}, //97
    {"", ""}, //98
    {"", ""}, //99
    {"", ""}, //100
    {"", ""}, //101
    {"", ""}, //102
    {"", ""}, //103
    {"", ""}, //104
    {"", ""}, //105
    {"", ""}, //106
    {"", ""}, //107
    {"", ""}, //108
    {"", ""}, //109
    {"", ""}, //110
    {"", ""}, //111
    {"", ""}, //112
    {"", ""}, //113
    {"", ""}, //114
    {"", ""}, //115
    {"", ""}, //116
    {"", ""}, //117
    {"", ""}, //118
    {"", ""}, //119
    {"", ""}, //120
    {"", ""}, //121
    {"", ""}, //122
    {"", ""}, //123
    {"", ""}, //124
    {"", ""}, //125
    {"", ""}, //126
    {"", ""}, //127
    {"", ""}, //128
    {"", ""}, //129
    {"", ""}, //130
    {"", ""}, //131
    {"", ""}, //132
    {"", ""}, //133
    {"", ""}, //134
    {"", ""}, //135
    {"", ""}, //136
    {"", ""}, //137
    {"", ""}, //138
    {"", ""}, //139
    {"", "LOG_IN"}, //140
    {"", "CREATE_CHAR"}, //141
    {"", ""}, //142
    {"", ""}, //143
    {"", ""}, //144
    {"", ""}, //145
    {"", ""}, //146
    {"", ""}, //147
    {"", ""}, //148
    {"", ""}, //149
    {"", ""}, //150
    {"", ""}, //151
    {"", ""}, //152
    {"", ""}, //153
    {"", ""}, //154
    {"", ""}, //155
    {"", ""}, //156
    {"", ""}, //157
    {"", ""}, //158
    {"", ""}, //159
    {"", ""}, //160
    {"", ""}, //161
    {"", ""}, //162
    {"", ""}, //163
    {"", ""}, //164
    {"", ""}, //165
    {"", ""}, //166
    {"", ""}, //167
    {"", ""}, //168
    {"", ""}, //169
    {"", ""}, //170
    {"", ""}, //171
    {"", ""}, //172
    {"", ""}, //173
    {"", ""}, //174
    {"", ""}, //175
    {"", ""}, //176
    {"", ""}, //177
    {"", ""}, //178
    {"", ""}, //179
    {"", ""}, //180
    {"", ""}, //181
    {"", ""}, //182
    {"", ""}, //183
    {"", ""}, //184
    {"", ""}, //185
    {"", ""}, //186
    {"", ""}, //187
    {"", ""}, //188
    {"", ""}, //189
    {"", ""}, //190
    {"", ""}, //191
    {"", ""}, //192
    {"", ""}, //193
    {"", ""}, //194
    {"", ""}, //195
    {"", ""}, //196
    {"", ""}, //197
    {"", ""}, //198
    {"", ""}, //199
    {"", ""}, //200
    {"", ""}, //201
    {"", ""}, //202
    {"", ""}, //203
    {"", ""}, //204
    {"", ""}, //205
    {"", ""}, //206
    {"", ""}, //207
    {"", ""}, //208
    {"", ""}, //209
    {"", ""}, //210
    {"", ""}, //211
    {"", ""}, //212
    {"", ""}, //213
    {"", ""}, //214
    {"", ""}, //215
    {"", ""}, //216
    {"", ""}, //217
    {"", ""}, //218
    {"", ""}, //219
    {"", ""}, //220
    {"", ""}, //221
    {"", ""}, //222
    {"", ""}, //223
    {"", ""}, //224
    {"", ""}, //225
    {"", ""}, //226
    {"", ""}, //227
    {"", ""}, //228
    {"", ""}, //229
    {"", "GET_DATE"}, //230
    {"", "GET_TIME"}, //231
    {"", "SERVER_STATS"}, //232
    {"", ""}, //233
    {"", ""}, //234
    {"", ""}, //235
    {"", ""}, //236
    {"", ""}, //237
    {"", ""}, //238
    {"", ""}, //239
    {"", ""}, //240
    {"", ""}, //241
    {"", ""}, //242
    {"", ""}, //243
    {"", ""}, //244
    {"", ""}, //245
    {"", ""}, //246
    {"", ""}, //247
    {"", ""}, //248
    {"YOU_DONT_EXIST", ""}, //249
    {"LOG_IN_OK", ""}, //250
    {"LOG_IN_NOT_OK", ""}, //251
    {"CREATE_CHAR_OK", ""},//252
    {"CREATE_CHAR_NOT_OK", ""},//253
    {"", ""}, //254
    {"", ""}, //255
};


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
            fprintf(stderr, "unable to clear file [%s] in function clear_file: module logging.c\n", file_name);
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
        //case EVENT_MOVE_ERROR: strcpy(file_name, MOVE_LOG_FILE_NAME); break;
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


void log_event(int event_type, const char *fmt, ...){

    /** public function - see header */

    char text_in[MAX_LOG_STRING]="";
    char text_out[MAX_LOG_STRING]="";

    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

    va_list args;
    va_start(args, fmt);


    //extract text from fmt
    vsprintf(text_in, fmt, args);

    //ensure text doesn't overflow
    size_t len=strlen(text_in);
    if(len>=MAX_LOG_STRING){

        log_event(EVENT_ERROR, "log text [%i] exceeds maximum string size [%i] in function %s: module %s: line %i", len, MAX_LOG_STRING, __func__, __FILE__, __LINE__);
        stop_server();
    }

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
/*
        case EVENT_MOVE_ERROR:
            sprintf(text_out, "[%s][%s] Move-error - %s", date_stamp_str, time_stamp_str, text_in);
        break;
*/
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
            fprintf(stderr, "unknown event type [%i] detected in function log_event: module logging.c\n", event_type);
            stop_server();
            break;
    }

    va_end(args);

    write_to_log(event_type, text_out);
}


void initialise_logs(){

    /** public function - see header */

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
            fprintf(stderr, "unable to clear initialisation log file [%s] in function initialise_logs: module logging.c\n", INITIALISATION_LOG_FILE_NAME);
            stop_server();
        }
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


void log_sqlite_error(char *error_type, const char *function_name, const char *module_name, int line_number, int return_code, const char *sql_stmt){

        /** public function - see header */

        //N.B we use const char in prototype so we can use __func__ and __FILE__ without causing compiler warnings

        log_event(EVENT_ERROR, "%s at line [%i] in function %s: module %s", error_type, line_number, function_name, module_name);
        log_text(EVENT_ERROR, "sql statement [%s]", sql_stmt);
        log_text(EVENT_ERROR, "return code [%i]: error type [%s]", return_code, sqlite3_errmsg(db));

        stop_server();
}


void log_packet(int socket, unsigned char *packet, int direction){

    /** public function - see header */

    size_t packet_length=get_packet_length(packet);

    if(packet_length>=MAX_PACKET_SIZE){

        log_event(EVENT_ERROR, "data length [%i] exceeded max [%i] in function %s: module %s: line %i", packet_length, MAX_PACKET_SIZE, __func__, __FILE__, __LINE__);
        stop_server();
    }

    //list packet contents
    char text_out[1024]="";

    for(size_t i=0; i<packet_length; i++){

        sprintf(text_out, "%s %i", text_out, packet[i]);
    }

    //list raw text
    char text_out2[1024]="";
    if(packet[0]==0){

        for(size_t i=4; i<packet_length; i++){

            if(packet[i]>=MIN_PRINTABLE_ASCII && packet[i]<=MAX_PRINTABLE_ASCII){

                sprintf(text_out2, "%s%c", text_out2, packet[i]);
            }
        }
    }

    int actor_node=client_socket[socket].actor_node;
    char char_name[MAX_CHAR_NAME_LEN]="";

    if(client_socket[socket].socket_node_status==CLIENT_LOGGED_IN){

        strcpy(char_name, clients.client[actor_node].char_name);
    }
    else {

        sprintf(char_name, "socket %d", socket);
    }

    char protocol_name[80]="";

    //log packets send from server
    if(direction==SEND) {

        if(strlen(protocol[packet[0]].server)>0){

            strcpy(protocol_name, protocol[packet[0]].server);
        }
        else {

            strcpy(protocol_name, "Unknown Protocol");
        }

        log_event(EVENT_PACKET, "Sent to       [%i] %s %s", socket, protocol_name, text_out);
        log_event(EVENT_SESSION, "Sent to [%s] %s[%i] %s", char_name, protocol_name, (int)packet[0], text_out2);
    }

    //log packets received from client
    if(direction==RECEIVE) {

        if(strlen(protocol[packet[0]].client)>0){

            strcpy(protocol_name, protocol[packet[0]].client);
        }
        else{

            strcpy(protocol_name, "Unknown Protocol");
        }

        log_event(EVENT_PACKET, "Received from [%i] %s %s", socket, protocol_name, text_out);
        log_event(EVENT_SESSION, "Received from [%s] %s[%i] %s", char_name, protocol_name, (int)packet[0], text_out2);
    }
}

