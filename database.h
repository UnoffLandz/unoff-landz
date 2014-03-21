#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#define DATABASE_FILE "unoff.db"

void open_database(char *database_name);

int get_char_data(char *char_name);

int get_max_char_id();

int get_table_count();

void create_character_table();

void create_item_table();

void create_3d_object_table();

void add_char(struct character_type character);

int get_chars_created_count();

void get_last_char_created();

void update_db_char_position(int connection);

void update_db_char_name(int connection);

void update_db_char_frame(int connection);

void update_db_char_stats(int connection);

void update_db_char_last_in_game(int connection);

void update_db_char_channels(int connection);

void update_db_char_inventory(int connection);

//void initialise_item_data();

void initialise_threed_object_data();

void load_3d_objects();

void load_items();

void add_item(int image_id, char *item_name, int harvestable, int cycle_amount, int emu, int interval, int exp,
              int food_value,
              int food_cooldown,
              int organic_nexus,
              int vegetal_nexus);

#endif // DATABASE_H_INCLUDED
