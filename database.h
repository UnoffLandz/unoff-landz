#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#define DATABASE_FILE "unoff.db"
#define DB_QUEUE_ELEMENT_SIZE 1024
#define DB_QUEUE_MAX_ELEMENTS 10

#define DB_QUEUE_MAX_NODES 10

enum { // return value for function check_queue in module database.c
    QUEUE_EMPTY,
    QUEUE_HAS_DATA,
};

void open_database(char *database_name);

int get_char_data(char *char_name);

int get_max_char_id();

int get_table_count();

void create_character_table();

void create_item_table();

int get_item_data(int item_id);

void add_char(struct character_type character);

void execute_sql(char *sql);

int get_chars_created_count();

void get_last_char_created();

void update_db_char_position(int connection);

void update_db_char_name(int connection);

void update_db_char_frame(int connection);

void update_db_char_stats(int connection);

void update_db_char_last_in_game(int connection);

void update_db_char_channels(int connection);


#endif // DATABASE_H_INCLUDED
