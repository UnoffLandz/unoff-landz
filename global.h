#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define MAX_CHANNELS 10
#define MAX_MAPS 10
#define MAX_GUILDS 10
#define MAX_CLIENTS 20

#define PATH_MAX 100 //longest permitted path
#define MIN_TRAVERSABLE_VALUE 1 //lowest value on height map that is traversable

#define CHANNEL_LIST_FILE "channels.lst"
#define MAP_LIST_FILE "maps.lst"
#define GUILD_LIST_FILE "guild.lst"

#define TILE_MAP_MAX 50000
#define HEIGHT_MAP_MAX 150000
#define TWOD_OBJECT_MAP_MAX 800000 //not yet used
#define THREED_OBJECT_MAP_MAX 800000 //not yet used

#define START_MAP_ID 1 //1=Isla Prima
//#define START_MAP_TILE 4236
#define START_MAP_TILE 27225

#define MIN_MAP_AXIS 10 //used to bounds check maps

#define TEMP_FILE "temp.tmp"

enum { // server to client protocol
    CHANGE_MAP=7,
    HERE_YOUR_STATS=18,
    HERE_YOUR_INVENTORY=19,
    GET_NEW_INVENTORY_ITEM=21
};

enum {//stats codes
    //0 1 physique
    //1 3 coordination
    //4 5 reasoning
    //6 7 will
    //8 9 instinct
    //10 11 vitality
    //12 13 human
    //14 15 animal
    //16 17 vegetal
    //18 19 inorganic
    //20 21 artificial
    //22 23 magic

    //24 25 manufacturing
    HARVEST_LVL=26,
    HARVEST_MAX_LVL=27,

    //26 27 harvest
    //28 29 alchemy
    //30 31 overall
    //32 33 defence
    //34 35 attack
    //36 37 magic
    //38 39 potion

    //40 41 ?????
    //42 43 material points
    //44 45 ethereal points
    //46 food level
    //47 ????
    //48 ????

    //49 50 manufacturing
    HARVEST_EXP=51,
    HARVEST_MAX_EXP=52,

     //53 54 alchemy
    //55 56 overall
    //57 58 defence
    //59 60 attack
    //61 62 magic
    //63 64 potion

    //65 66 ????
    //67 68 summoning (R)
    //69 70 summoning (L)
    //71 72 crafting (R)
    //73 74 crafting (L)
    //75 76 engineering (R)
    //77 78 engineering (L)
    //79 80 ranging (R)
    //81 82 ranging (L)
    //83 84 tailoring (R)
    //85 86 tailoring (L)

    //87 88 action points
    //89 ?????
};

enum{// harv item code
     chrysanthemums=1112,
};

enum {// frame type
    nothing=0,
    stand_up=14,
    sit_down=13,
};

enum {// actor movement vectors
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST,
};

struct vector_type{
    int x;
    int y;
    unsigned char move_cmd;
};

struct vector_type vector[8];

enum{ // general boolean values
    FALSE,
    TRUE
};

enum { //log events
    EVENT_NEW_CHAR,
    EVENT_ERROR,
    EVENT_SESSION,
    EVENT_CHAT,
    EVENT_MOVE_ERROR
};

enum {// colours
    c_red1,
    c_orange1,
    c_yellow1,
    c_green1,
    c_blue1,
    c_purple1,
    c_grey1,
    c_red2,
    c_orange2,
    c_yellow2,
    c_green2,
    c_blue2,
    c_purple2,
    c_grey2,
    c_red3,
    c_orange3,
    c_yellow3,
    c_green3,
    c_blue3,
    c_purple3,
    c_grey3,
    c_red4,
    c_orange4,
    c_yellow4,
    c_green4,
    c_blue4,
    c_purple4,
    c_grey4,
};

enum{ // actor type
    HUMAN_FEMALE=0,
    HUMAN_MALE=1,
    ELF_FEMALE=2,
    ELF_MALE=3,
    DWARF_FEMALE=4,
    DWARF_MALE=5,
    GNOME_FEMALE=37,
    GNOME_MALE=38,
    ORCHAN_FEMALE=39,
    ORCHAN_MALE=40,
    DRAEGONI_FEMALE=41,
    DRAEGONI_MALE=42
};

enum{ // race
    HUMAN,
    ELF,
    DWARF,
    GNOME,
    ORCHAN,
    DRAEGONI
};

enum{ // gender
    FEMALE,
    MALE
};

enum { // skin type
    SKIN_BROWN,
    SKIN_NORMAL,
};

enum { // hair type
    HAIR_BLACK,
    HAIR_BLOND,
};

enum { // shirt type
    SHIRT_BLACK,
    SHIRT_BLUE,
};

enum { // pants type
    PANTS_BLACK,
    PANTS_BLUE,
};

enum { // boots type
    BOOTS_BLACK,
    BOOTS_BROWN,
};

enum { // head type
    HEAD_1,
    HEAD_2,
    HEAD_3,
    HEAD_4,
    HEAD_5,
};

enum { // shield type
    SHIELD_WOOD=0,
    SHIELD_WOOD_ENHANCED=1,
    SHIELD_IRON=2,
    SHIELD_STEEL=3,
    SHIELD_TITANIUM=4,
    SHIELD_BRONZE=5,
    QUIVER_ARROWS=7,
    SHIELD_NONE=11,
    QUIVER_BOLTS=13,
};

enum { // weapon type
    WEAPON_NONE=0,
    SWORD_1=1,
};

enum { // cape type
    CAPE_BLACK,
    CAPE_BLUE,
    CAPE_BLUEGRAY,
    CAPE_BROWN,
    CAPE_BROWNGRAY,
    CAPE_GRAY,
    CAPE_GREEN,
    CAPE_GREENGRAY,
    CAPE_PURPLE,
    CAPE_WHITE,
    CAPE_FUR,
    CAPE_GOLD,
    CAPE_RED,
    CAPE_ORANGE,
    CAPE_MOD,
    CAPE_DERIN,
    CAPE_RAVENOD,
    CAPE_PLACID,
    CAPE_LORD_VERMOR,
    CAPE_AISLINN,
    CAPE_SOLDUS,
    CAPE_LOTHARION,
    CAPE_LEARNER,
    CAPE_UNUSED_23,
    CAPE_UNUSED_24,
    CAPE_UNUSED_25,
    CAPE_UNUSED_26,
    CAPE_UNUSED_27,
    CAPE_UNUSED_28,
    CAPE_UNUSED_29,
    CAPE_NONE,
};

enum { // helmet type
    HELMET_IRON,
    HELMET_FUR,
    HELMET_LEATHER,
    HELMET_RACOON,
    HELMET_SKUNK,
    HELMET_CROWN_OF_MANA,
    HELMET_CROWN_OF_LIFE,
    HELMET_STEEL,
    HELMET_TITANIUM,
    HELMET_BRONZE,
    HELMET_UNUSED_10,
    HELMET_UNUSED_11,
    HELMET_UNUSED_12,
    HELMET_UNUSED_13,
    HELMET_UNUSED_14,
    HELMET_UNUSED_15,
    HELMET_UNUSED_16,
    HELMET_UNUSED_17,
    HELMET_UNUSED_18,
    HELMET_UNUSED_19,
    HELMET_NONE,
};

enum { // neck type
    NECK_0,
    NECK_1,
    NECK_2,
    NECK_3,
    NECK_4,
    NECK_6,
    NECK_7,
};


/** MAPS */
struct map_node_type{
    char map_name[1024];    // eg Isla Prima
    char elm_filename[1024];// eg startmap.elm
    int map_axis;
    unsigned char tile_map[TILE_MAP_MAX];
    int tile_map_size;
    unsigned char height_map[HEIGHT_MAP_MAX];
    int height_map_size;
    unsigned char threed_object_map[THREED_OBJECT_MAP_MAX];
    int threed_object_map_size;
    unsigned char twod_object_map[TWOD_OBJECT_MAP_MAX];
    int twod_object_map_size;
    int client_list[MAX_CLIENTS];
    int client_list_count;
};

struct map_list_type {
    int count;
    int max;
    struct map_node_type **map;
};

struct map_list_type maps;


/** GUILDS */
struct guild_node_type{
    char guild_name[1024];
    char guild_tag[1024];
    int tag_colour;
    int log_on_notification_colour;
    int log_off_notification_colour;
    int guild_chan_text_colour;
    int guild_chan_number;
};

struct guild_list_type {
    int count;
    int max;
    struct guild_node_type **guild;
};

struct guild_list_type guilds;


/** CLIENTS */
struct client_node_type{
    enum {LOGGED_IN, CONNECTED, LOGGED_OUT} status;
    int packet_buffer[1024];
    int packet_buffer_length;
    int character_id;
    int path[PATH_MAX];
    int path_max;
    int path_count;
    time_t time_of_last_move;
    time_t time_of_last_heartbeat;
    time_t time_of_last_harvest;
    int harvest_flag;
    int harvest_item;
    unsigned char cmd_buffer[10][1024];
    int cmd_buffer_end;
    char ip_address[16];
    int sit_down;

    char char_name[1024];
    char password[1024];
    int char_status;
    int time_played;
    int active_chan;
    int chan[4];       // chan0, chan1, chan2  (chan3 used for guild chat)
    int gm_permission; // permission to use #GM command (aka mute)
    int ig_permission; // permission to use #IG command
    int map_id;
    int map_tile;
    int guild_id;
    int char_type;
    int skin_type;
    int hair_type;
    int shirt_type;
    int pants_type;
    int boots_type;
    int head_type;
    int shield_type;
    int weapon_type;
    int cape_type;
    int helmet_type;
    int frame;
    int max_health;
    int current_health;
    int visual_proximity; // proximity for display of other actors/creatures
    int local_text_proximity; //  proximity for local messages from other actors
    time_t last_in_game; // date char was last in-game
    time_t char_created; // date char was created
    time_t joined_guild; // date joined guild

    int physique;
    int max_physique;
    int coordination;
    int max_coordination;
    int reasoning;
    int max_reasoning;
    int will;
    int max_will;
    int instinct;
    int max_instinct;
    int vitality;
    int max_vitality;

    int human;
    int max_human;
    int animal;
    int max_animal;
    int vegetal;
    int max_vegetal;
    int inorganic;
    int max_inorganic;
    int artificial;
    int max_artificial;
    int magic;
    int max_magic;

    int manufacturing_lvl;
    int max_manufacturing_lvl;
    int harvest_lvl;
    int max_harvest_lvl;
    int alchemy_lvl;
    int max_alchemy_lvl;
    int overall_lvl;
    int max_overall_lvl;
    int attack_lvl;
    int max_attack_lvl;
    int defence_lvl;
    int max_defence_lvl;
    int magic_lvl;
    int max_magic_lvl;
    int potion_lvl;
    int max_potion_lvl;

    int material_pts;
    int max_material_pts;
    int ethereal_pts;
    int max_ethereal_pts;

    int food_lvl;

    int manufacture_exp;
    int max_manufacture_exp;
    int harvest_exp;
    int max_harvest_exp;
    int alchemy_exp;
    int max_alchemy_exp;
    int overall_exp;
    int max_overall_exp;
    int attack_exp;
    int max_attack_exp;
    int defence_exp;
    int max_defence_exp;
    int magic_exp;
    int max_magic_exp;
    int potion_exp;
    int max_potion_exp;

    int book_id;
    int max_book_time;
    int elapsed_book_time;

    unsigned char inventory[1024];
    int inventory_size;
};

struct client_list_type {
    int count;
    int max;
    struct client_node_type **client;
};

struct client_list_type clients;


/** CHANNELS */
struct channel_node_type{
    enum {CHAN_SYSTEM, CHAN_PERMANENT, CHAN_GUILD, CHAN_CHAT, CHAN_VACANT} chan_type;
    char channel_name[1024];
    int channel_id;
    int owner_id; /* could be char or guild depending on chan_type */
    char password[1024];
    char description[1024];
    int client_list[MAX_CLIENTS];
    int client_list_count;
};

struct channel_list_type {
    int count;
    int max;
    int spool_size;
    struct channel_node_type **channel;
};

struct channel_list_type channels;


/** HARVESTABLES */
struct harvestables_type{
    int exp;
    int emu;
    int nexus;
    char name[20];
    int interval;
    int image_id;
    int food_value;
    int food_cool_down;
};

struct harvestables_type harvestables[2000];


/** OTHERS */
struct timeval time_check;
time_t server_start_time;

#include <sqlite3.h>
sqlite3 *db;

enum { //return values for get_char_id function
    CHAR_FOUND=-1,
    CHAR_NOT_FOUND =0
};

//struct used to pass to database on character creation and get_char_id function
struct character_type{
    int char_id;
    char char_name[1024];
    char password[1024];
    int char_status;
    int active_chan;
    int chan[3];
    int gm_permission;
    int ig_permission;
    int map_id;
    int map_tile;
    int char_type;
    int skin_type;
    int hair_type;
    int shirt_type;
    int pants_type;
    int boots_type;
    int head_type;
    int shield_type;
    int weapon_type;
    int cape_type;
    int helmet_type;
    int frame;
    int max_health;
    int current_health;
    int visual_proximity; // proximity for display of other actors/creatures
    int local_text_proximity; //  proximity for local messages from other actors
    int char_created;
    time_t last_in_game;
    time_t joined_guild;
    int guild_id;
    int overall_exp;
    int harvest_exp;
};

struct character_type character;

/*
// the database buffer queue template
struct queue_type {
    int start;       // the node where the queue starts
    int end;         // the node where the queue ends
    int count;       // the number of used nodes in queue
    int max_nodes;   // the maximum number of nodes in queue
    char **text_str; // the node array
};

//create database buffer from template
struct queue_type db_buffer_queue;
*/

//struct to carry global data
struct game_data_type {
    int char_count;
    char name_last_char_created[1024];
    time_t date_last_char_created;
};

struct game_data_type game_data;



#endif // GLOBAL_H_INCLUDED
