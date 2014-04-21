#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include <sqlite3.h> //required to allow creation of the sqlite3 type used for the db handle (see next statement)

#define CHARACTER_TABLE_SQL "CREATE TABLE CHARACTER_TABLE( \
        CHAR_ID             INTEGER PRIMARY KEY AUTOINCREMENT, \
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
        LAST_IN_GAME        INT, \
        CHAR_CREATED        INT, \
        JOINED_GUILD        INT, \
        PHYSIQUE_PP         INT, \
        VITALITY_PP         INT, \
        WILL_PP             INT, \
        COORDINATION_PP     INT, \
        OVERALL_EXP         INT, \
        HARVEST_EXP         INT)"

#define INVENTORY_TABLE_SQL "CREATE TABLE INVENTORY_TABLE( \
        ID                  INTEGER PRIMARY KEY    AUTOINCREMENT, \
        CHAR_ID             INT, \
        SLOT                INT, \
        IMAGE_ID            INT, \
        AMOUNT              INT)"

#define ITEM_TABLE_SQL  "CREATE TABLE ITEM_TABLE(  \
        IMAGE_ID            INTEGER PRIMARY KEY     NOT NULL, \
        ITEM_NAME           TEXT, \
        HARVESTABLE         INT, \
        EMU                 INT, \
        INTERVAL            INT, \
        EXP                 INT, \
        FOOD_VALUE          INT, \
        FOOD_COOLDOWN       INT, \
        ORGANIC_NEXUS       INT, \
        VEGETAL_NEXUS       INT )"

#define THREED_OBJECT_TABLE_SQL  "CREATE TABLE THREED_OBJECT_TABLE(  \
        ID                  INTEGER PRIMARY KEY     AUTOINCREMENT, \
        FILE_NAME           TEXT,  \
        INVENTORY_IMAGE_ID  INT )"

#define MAP_TABLE_SQL "CREATE TABLE MAP_TABLE( \
        MAP_ID              INTEGER PRIMARY KEY     NOT NULL, \
        MAP_NAME            TEXT, \
        ELM_FILE_NAME       TEXT )"

#define CHANNEL_TABLE_SQL "CREATE TABLE CHANNEL_TABLE( \
        CHANNEL_ID          INTEGER PRIMARY KEY     NOT NULL, \
        TYPE                INT,  \
        OWNER_ID            INT,  \
        PASSWORD            TEXT, \
        NAME                TEXT, \
        DESCRIPTION         TEXT)"

#define RACE_TABLE_SQL "CREATE TABLE RACE_TABLE( \
        RACE_ID             INTEGER PRIMARY KEY     NOT NULL, \
        RACE_NAME           TEXT, \
        RACE_DESCRIPTION    TEXT, \
        INITIAL_EMU         INT,  \
        EMU_MULTIPLIER      REAL, \
        INITIAL_VISPROX     INT,  \
        VISPROX_MULTIPLIER  REAL, \
        INITIAL_CHATPROX    INT,  \
        CHATPROX_MULTIPLIER REAL, \
        INITIAL_NIGHTVIS    REAL, \
        NIGHTVIS_MULTIPLIER REAL, \
        CHAR_COUNT          INT)"

#define GUILD_TABLE_SQL "CREATE TABLE GUILD_TABLE( \
        GUILD_ID             INTEGER PRIMARY KEY     NOT NULL, \
        GUILD_TAG            TEXT, \
        GUILD_NAME           TEXT, \
        GUILD_DESCRIPTION    TEXT, \
        TAG_COLOUR           INT,  \
        LOGON_COLOUR         INT, \
        LOGOFF_COLOUR        INT, \
        CHAN_TEXT_COLOUR     INT, \
        CHAN_ID              INT, \
        DATE_CREATED         INT)"

sqlite3 *db; // database handle which is set when function open_database is called


/** RESULT  : Opens sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : void

    PURPOSE : Reduces the need to open and close the database each time a query is used

    USAGE   : database.c main.c */
void open_database(char *database_name);


/** RESULT  : gets the highest char_id in the database

    RETURNS : the highest char_id in the database

    PURPOSE : Enables the char_id of a newly created char to be associated with an entry in the clients struct

    USAGE   : process_packet protocol.c */
int get_max_char_id();


/** RESULT  : Finds sqlite database file and creates the handle [db] which can then be called by other
              database functions.

    RETURNS : an enum value 0=FOUND -1=NOT_FOUND

    PURPOSE : Purpose of this function is twofold. It is used to test if a character exists and, if so;
              loads the data for that character into into the character struct

    USAGE   : send_pm:chat.c, rename_char:hash_command.c, process_log_in:log_in.c, process_packet:protocol.c */
int get_char_data_from_db(char *char_name);

/** RESULT  : Determines the number of tables in the database

    RETURNS : The number of tables in the database

    PURPOSE : To determine if a database table structure needs to be created.

    USAGE   : database_exists database.c */
int database_table_count();


/** RESULT  : Creates a database table structure

    RETURNS : void

    PURPOSE : Creates a required table when a new database file is created

    USAGE   : main:main.c */
void create_database_table(char *sql);


/** RESULT  : Adds a character entry to the character_table

    RETURNS : void

    PURPOSE : allows a new character to be created

    USAGE   : process_packet:protocol.c */
void add_char(struct client_node_type character);


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


/** RESULT  : Updates a characters entire inventory to the database

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_harvesting:harvesting.c */
void update_db_char_inventory(int connection);


/** RESULT  : Updates a specific slot in a characters inventory to the database

    RETURNS : void

    PURPOSE : Facilitates persistent character state

    USAGE   : process_harvesting:harvesting.c */
void update_db_char_slot(int connection, int slot);


/** RESULT  : load race data from the database to the race struct array

    RETURNS : void

    PURPOSE : allows race data to be held in memory for faster operations

    USAGE   : initialise_races: initialisation.c */
void load_races();


/** RESULT  : load channel data from the database to the channels struct array

    RETURNS : void

    PURPOSE : allows channel data to be held in memory for faster operations

    USAGE   : initialise_channels: initialisation.c */
void load_channels();


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

    PURPOSE : allows map data to be held in memory for faster operations

    USAGE   : initialise_maps:initialisation.c */
void load_maps();


/** RESULT  : loads guild data from the database to the map struct array

    RETURNS : void

    PURPOSE : allows guild data to be held in memory for faster operations

    USAGE   : initialise_guilds:initialisation.c */
void load_guilds();


/** RESULT  : adds an entry to the item_table of the database

    RETURNS : void

    PURPOSE : enables database entries to be bulk loaded from a text file

    USAGE   : load_database_item_table_data:files.c */
void add_item(int image_id, char *item_name,
              int harvestable,
              int emu,
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
void add_threed_object(char *filename, int image_id);


/** RESULT  : adds an entry to the map_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_threed_object_table_data:files.c */
void add_map(int map_id, char *map_name, char *elm_file_name);


/** RESULT  : adds an entry to the channel_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_channel_table_data:files.c */
void add_channel(int channel_id, int owner_id,
                 int channel_type,
                 char *password,
                 char *channel_name,
                 char*channel_description);


/** RESULT  : adds an entry to the race_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_race_table_data:files.c */
void add_race(int race_id, char *race_name, char *race_description,
              int initial_carry_capacity,
              float carry_capacity_multiplier,
              int initial_visprox,
              float visprox_multiplier,
              int initial_chatprox,
              float chatprox_multiplier,
              float initial_nightvis,
              float nightvis_multiplier
              );


/** RESULT  : adds an entry to the guild_table of the database

    RETURNS : void

    PURPOSE : enables data to be bulk loaded from a text file

    USAGE   : load_database_guild_table_data:files.c */
void add_guild(int guild_id, char *guild_tag, char *guild_name, char *guild_description,
              int tag_colour,
              int log_on_colour,
              int log_off_colour,
              int chan_text_colour,
              int chan_id
              );


/** RESULT  : Updates the number of chars created for a particular race

    RETURNS : void

    PURPOSE : ???

    USAGE   : process_packet protocol.c */
void update_db_race_count(int race_id);

#endif // DATABASE_H_INCLUDED
