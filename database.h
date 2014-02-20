#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#define DATABASE_FILE "unoff.db"

void open_database(char *database_name);

int get_char_id(char *char_name);

int get_max_char_id();

int get_table_count();

void create_tables();

void add_char(struct new_character_type new_character);

void load_character_from_database(int char_id, int connection);

#endif // DATABASE_H_INCLUDED
