#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include "global.h"

int load_character(char *filename, int char_id);
void load_all_characters(const char *filename);
void save_character(char *filename, int char_id);
void save_new_character(char *char_name, int id);

int load_channel(char *filename, int i);
void load_all_channels(const char *filename);

int load_guild(char *filename, int i);
void load_all_guilds(const char *filename);

int load_map(char *filename, int i);
void load_all_maps(const char *filename);

void read_motd(int new_sock);

int get_file_size(char *file_name);

void log_event(int event_type, char *text_in);

#endif // FILES_H_INCLUDED
