#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include <sqlite3.h> //required to allow creation of the sqlite3 type used for the db handle (see next statement)
sqlite3 *db; // database handle which is set when function open_database is called

#define CHARACTER_TABLE_SQL "CREATE TABLE CHARACTER_TABLE( \
        CHAR_ID             INT PRIMARY KEY NOT NULL, \
        CHAR_NAME           TEXT            NOT NULL, \
        PASSWORD            TEXT            NOT NULL, \
        CHAR_STATUS         INT, \
        TIME_PLAYED         INT, \
        ACTIVE_CHAN         INT, \
        CHAN_0              INT, \
        CHAN_1              INT, \
        CHAN_2              INT, \
        GM_PERMISSION       INT, \
        IG_PERMISSION       INT, \
        MAP_ID              INT, \
        MAP_TILE            INT, \
        GUILD_ID            INT, \
        CHAR_TYPE           INT, \
        SKIN_TYPE           INT, \
        HAIR_TYPE           INT, \
        SHIRT_TYPE          INT, \
        PANTS_TYPE          INT, \
        BOOTS_TYPE          INT, \
        HEAD_TYPE           INT, \
        SHIELD_TYPE         INT, \
        WEAPON_TYPE         INT, \
        CAPE_TYPE           INT, \
        HELMET_TYPE         INT, \
        FRAME               INT, \
        MAX_HEALTH          INT, \
        CURRENT_HEALTH      INT, \
        VISUAL_PROXIMITY    INT, \
        LOCAL_TEXT_PROXIMITY INT, \
        LAST_IN_GAME        INT, \
        CHAR_CREATED        INT, \
        JOINED_GUILD        INT, \
        INVENTORY_LENGTH    INT, \
        INVENTORY           BLOB, \
        OVERALL_EXP         INT, \
        HARVEST_EXP         INT )"

#define ITEM_TABLE_SQL  "CREATE TABLE ITEM_TABLE(  \
        IMAGE_ID           INT PRIMARY KEY     NOT NULL, \
        ITEM_NAME           TEXT, \
        HARVESTABLE         INT, \
        HARVEST_CYCLE       INT, \
        CYCLE_AMOUNT        INT, \
        EMU                 INT, \
        INTERVAL            INT, \
        EXP                 INT, \
        FOOD_VALUE          INT, \
        FOOD_COOLDOWN       INT, \
        ORGANIC_NEXUS       INT, \
        VEGETAL_NEXUS       INT )"

#define THREED_OBJECT_TABLE_SQL  "CREATE TABLE THREED_OBJECT_TABLE(  \
        OBJECT_ID           INT PRIMARY KEY     NOT NULL, \
        FILE_NAME           TEXT,  \
        INVENTORY_IMAGE_ID  INT )"

#define MAP_TABLE_SQL "CREATE TABLE MAP_TABLE( \
        MAP_ID              INT PRIMARY KEY     NOT NULL, \
        MAP_NAME            TEXT, \
        ELM_FILE_NAME       TEXT )"

/** RESULT  : Opens sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : void

    PURPOSE : Reduces the need to open and close the database each time a query is used

    USAGE   : database.c main.c */
void open_database(char *database_name);


/** RESULT  : Finds sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : an enum value 0=FOUND -1=NOT_FOUND

    PURPOSE : Purpose of this function is twofold. It is used to test if a character exists and, if so;
              loads the data for that character into into the character struct

    USAGE   : send_pm:chat.c, rename_char:hash_command.c, process_log_in:log_in.c, process_packet:protocol.c */
int get_char_data(char *char_name);


/** RESULT  : Finds the highest entry in the ID field of the database character table

    RETURNS : The value of the highest entry in the ID field of the database character table

    PURPOSE : Allows each entry on the character table to be given a unique ID

    USAGE   : process_packet:protocol.c */
int get_max_char_id();


/** RESULT  : Determines the number of tables in the database

    RETURNS : The number of tables in the database

    PURPOSE : To determine if a database table structure needs to be created.

    USAGE   : main:main.c */
int get_table_count();


/** RESULT  : Creates a database table structure

    RETURNS : void

    PURPOSE : Creates a required table when a new database file is created

    USAGE   : main:main.c */
void create_database_table(char *table_name, char *sql);


/** RESULT  : Adds a character entry to the character_table

    RETURNS : void

    PURPOSE : allows a new character to be created

    USAGE   : process_packet:protocol.c */
void add_char(struct character_type character);


/** RESULT  : Determines the number of character entries in the character_table

    RETURNS : The number of characters that have been created in-game

    PURPOSE : Used at server boot to populate the game_data struct data that is then used in the MOTD

    USAGE   : main:main.c */
int get_chars_created_count();


/** RESULT  : Determines the last character entry in the character_table

    RETURNS : void

    PURPOSE : Used at server boot to populate the game_data struct that is used in the MOTD

    USAGE   : main:main.c */
void get_last_char_created();


/** RESULT  : Updates the character entry in the character_table with the current map and tile

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_char_move:character_movement.c */
void update_db_char_position(int connection);


/** RESULT  : Updates the character entry in the character_table with a new character name

    RETURNS : void

    PURPOSE : Facilitates hash command #NAME_CHANGE

    USAGE   : rename_char:hash_command.c */
void update_db_char_name(int onnection);


/** RESULT  : Updates the character entry in the character_table with the actor frame

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_packet:protocol.c */
void update_db_char_frame(int connection);


/** RESULT  : Updates the character entry in the character_table with stats data

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_harvesting:harvesting.c */
void update_db_char_stats(int connection);


/** RESULT  : Updates the character entry in the character_table with the time the character was last
              in-game

    RETURNS : void

    PURPOSE : Facilitates the #DETAILS command which tells other player when another char was last in game

    USAGE   : close_connection_slot:main.c */
void update_db_char_last_in_game(int connection);


/** RESULT  : Updates the character entry in the character_table with the total time the character has
              been played

    RETURNS : void

    PURPOSE : Facilitates the #DETAILS command and character age related functions

    USAGE   : close_connection_slot:main.c */
void update_db_char_time_played(int connection);


/** RESULT  : Updates the character entry in the character_table with the active and current channel
              selection

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : join_channel:chat.c, process_packet:protocol.c */
void update_db_char_channels(int connection);


/** RESULT  : Updates the character entry in the character_table with the current inventory

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_harvesting:harvesting.c */
void update_db_char_inventory(int connection);


/** RESULT  : loads 3d object data from the database to the threed_object struct array

    RETURNS : void

    PURPOSE : allows 3d object data to be held in memory for faster operations

    USAGE   : initialise_3d_objects:initialisation.c */
void load_3d_objects();


/** RESULT  : loads item data from the database to the item struct array

    RETURNS : void

    PURPOSE : allows item data to be held in memory for faster operations

    USAGE   : initialise_items:initialisation.c */
void load_items();


/** RESULT  : loads map data from the database to the map struct array

    RETURNS : void

    PURPOSE : allows item data to be held in memory for faster operations

    USAGE   : initialise_items:initialisation.c */
void load_maps();


/** RESULT  : adds an entry to the item_table of the database

    RETURNS : void

    PURPOSE : enables database entries to be bulk loaded from a text file

    USAGE   : load_database_item_table_data:files.c */
void add_item(int image_id, char *item_name, int harvestable, int harvest_cycle, int cycle_amount, int emu,
              int interval,
              int exp,
              int food_value,
              int food_cooldown,
              int organic_nexus,
              int vegetal_nexus);


/** RESULT  : adds an entry to the threed_object_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_threed_object_table_data:files.c */
void add_threed_object(int id, char *filename, int image_id);


/** RESULT  : adds an entry to the map_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_threed_object_table_data:files.c */
void add_map(int map_id, char *map_name, char *elm_file_name);

#endif // DATABASE_H_INCLUDED
