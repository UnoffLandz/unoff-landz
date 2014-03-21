#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include "global.h"

enum { //return values for file loading
    FILE_FOUND=0,
    FILE_NOT_FOUND=-1
};

int load_character(char *file_name, int char_id);
void load_all_characters(char *file_name);
void save_character(char *file_name, int char_id);
void save_new_character(char *char_name, int id);

int load_channel(char *file_name, int i);
void load_all_channels(char *file_name);
void save_channel(char *file_name, int id);

int load_guild(char *file_name, int i);
void load_all_guilds(char *file_name);
void save_guild(char *file_name, int id);

int load_map(char *file_name, int i);
void load_all_maps(char *file_name);
void save_map(char *file_name, int id);

int get_file_size(char *file_name);

void log_event(int event_type, char *text_in);

void load_database_item_table_data(char *file_name);


#endif // FILES_H_INCLUDED
