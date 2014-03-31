#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include "global.h"

#define GUILD_LIST_FILE         "guild.lst"

#define ITEM_DATA_FILE          "item_data.txt"
#define THREED_OBJECT_DATA_FILE "threed_object_data.txt"
#define MAP_DATA_FILE           "map_data.txt"
#define CHANNEL_DATA_FILE       "channel_data.txt"
#define RACE_DATA_FILE          "race_data_txt"
#define DATABASE_FILE           "unoff.db"

#define CHARACTER_LOG           "character.log"
#define ERROR_LOG               "error.log"
#define SESSION_LOG             "session_log"
#define CHAT_LOG                "chat_log"
#define MOVE_LOG                "move_log"

enum { //log events
    EVENT_NEW_CHAR,
    EVENT_ERROR,
    EVENT_SESSION,
    EVENT_CHAT,
    EVENT_MOVE_ERROR
};

void clear_file(char *file_name);

int load_guild(char *file_name, int i);
void load_all_guilds(char *file_name);
void save_guild(char *file_name, int id);

int load_map(int map_id);

int get_file_size(char *file_name);

void log_event(int event_type, char *text_in);

void load_database_item_table_data(char *file_name);

void load_database_threed_object_table_data(char *file_name);

void load_database_map_table_data(char *file_name);

void load_database_channel_table_data(char *file_name);

void load_database_race_table_data(char *file_name);

#endif // FILES_H_INCLUDED
