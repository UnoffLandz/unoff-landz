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

#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#define CHARACTER_LOG_FILE_NAME      "character.log"
#define ERROR_LOG_FILE_NAME          "error.log"
#define SESSION_LOG_FILE_NAME        "session.log"
#define CHAT_LOG_FILE_NAME           "chat.log"
#define MOVE_LOG_FILE_NAME           "move.log"
#define INITIALISATION_LOG_FILE_NAME "initialisation.log"
#define MAP_LOAD_LOG_FILE_NAME       "map_load.log"
#define PACKET_LOG_FILE_NAME         "packet.log"

enum { //log events
    EVENT_NEW_CHAR,
    EVENT_ERROR,
    EVENT_SESSION,
    EVENT_CHAT,
    EVENT_MOVE_ERROR,
    EVENT_INITIALISATION,
    EVENT_MAP_LOAD,
    EVENT_PACKET,
};

/** RESULT  : logs formatted text entries to log files

    RETURNS : void

    PURPOSE : provides a single function to handle formatted entries to log files for each event type

    NOTES   : usage - log_event(EVENT_TYPE, "text %s %i", a_str, a_int);
*/
void log_event(int event_type, char *fmt, ...);


/** RESULT  : logs unformatted text to log files

    RETURNS : void

    PURPOSE : provides a way to add unformatted entries to log files, ie blank lines and unstamped text

    NOTES   : usage - log_event(EVENT_TYPE, "text %s %i", a_str, a_int);
*/
void log_text(int event_type, char *fmt, ...);


/** RESULT  : clears the contents of all log files

    RETURNS : void

    PURPOSE : clear all log files on server boot

    NOTES   : call only after game_data.server_start_time has been set, otherwise time/date in
              initialisation log will be incorrect
*/
void initialise_logs();


/** RESULT  : logs sqlite function errors

    RETURNS : void

    PURPOSE : provides a standard format for sqlite error reporting

    NOTES   :
*/
void log_sqlite_error(char *error_type, const char *function_name, const char *module_name, int line_number, int return_code, const char *sql_stmt);


/** RESULT  : logs a protocol packet

    RETURNS : void

    PURPOSE : code reuse

    NOTES   :
*/
void log_packet(int connection, unsigned char *packet);

#endif // LOGGING_H_INCLUDED
