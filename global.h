#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define MAX_CHARACTERS 20
#define MAX_CHANNELS 10
#define MAX_MAPS 10
#define MAX_GUILDS 10
#define MAX_CLIENTS 20

#define MOVE_BUFFER_MAX 100 //maximum buffered MOVE_TO's
#define PATH_QUEUE_MAX 50000 //maximum tiles that can be queued whilst exploring a path
#define PATH_MAX 100 //longest permitted path
#define MIN_TRAVERSABLE_VALUE 1 //lowest value on height map that is traversable

#define CHANNEL_LIST_FILE "channels.lst"
#define MAP_LIST_FILE "maps.lst"
#define GUILD_LIST_FILE "guild.lst"
#define CHARACTER_LIST_FILE "character.lst"

enum { //log events
    EVENT_NEW_CHAR,
    EVENT_ERROR,
    EVENT_SESSION
};

enum { //return values for validate_password function
    WRONG_PASSWORD=-1,
    CHARNAME_NOT_FOUND=-2
};

enum { //return values for file loading
    FILE_FOUND=0,
    FILE_NOT_FOUND=-1
};

enum { //return values for get_direct_path and get_indirect_path
    PATH_TOO_LONG=-4,
    PATH_ILLEGAL_DESTINATION=-3,
    PATH_UNREACHABLE=-2,
    PATH_BLOCKED=-1,
    PATH_OPEN=0
};

enum { //return values for leave channel and join_channel
    CHANNEL_UNKNOWN,
    CHANNEL_INVALID,
    CHANNEL_SYSTEM,
    CHANNEL_NOT_OPEN,
    NOT_IN_CHANNEL,
    CHANNEL_BARRED,
    NO_FREE_CHANNEL_SLOTS,
    CHANNEL_JOINED,
    CHANNEL_LEFT
};

enum { //return values from process_guild_chat
    GM_INVALID,
    GM_NO_GUILD,
    GM_NO_PERMISSION,
    GM_SENT
};

enum { //return values from process_inter_guild_chat
    IG_NOT_AVAILABLE,
    IG_NO_PERMISSION,
    IG_INVALID_GUILD,
    IG_SENT,
    IG_MALFORMED
};

enum{ //return values from process_hash_command
    HASH_CMD_UNSUPPORTED,
    HASH_CMD_UNKNOWN,
    HASH_CMD_EXECUTED,
    HASH_CMD_FAILED
};

enum {
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

enum { /* channel types */
    CHAT_LOCAL,
    CHAT_PERSONAL,
    CHAT_GM,
    CHAT_SERVER,
    CHAT_MOD,
    CHAT_CHANNEL1,
    CHAT_CHANNEL2,
    CHAT_CHANNEL3,
    CHAT_MODPM,
    CHAT_SERVER_PM,
};

enum { /* skin type */
    SKIN_BROWN,
    SKIN_NORMAL,
};

enum { /* hair type */
    HAIR_BLACK,
    HAIR_BLOND,
};

enum { /* shirt type */
    SHIRT_BLACK,
    SHIRT_BLUE,
};

enum { /* pants type */
    PANTS_BLACK,
    PANTS_BLUE,
};

enum { /* boots type */
    BOOTS_BLACK,
    BOOTS_BROWN,
};

enum { /* head type */
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

enum { /* cape type */
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

enum { /* helmet type */
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

enum { /* neck type */
    NECK_0,
    NECK_1,
    NECK_2,
    NECK_3,
    NECK_4,
    NECK_6,
    NECK_7,
};

// create the char struct
struct character_node_type{
    char char_name[1024];
    char password[1024];
    int time_played;
    enum {CHAR_ALIVE, CHAR_DEAD, CHAR_BANNED} char_status;
    int active_chan;
    int chan[4];       // chan0, chan1, chan2  (chan3 used for guild chat)
    int gm_permission; // permission to use #GM command (aka mute)
    int ig_permission; // permission to use #IG command
    int map_id;
    int map_tile;
    int guild_id;
    // date joined guild
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
    int neck_type;
    int max_health;
    int current_health;
    int visual_proximity; // proximity for display of other actors/creatures
    int local_text_proximity; //  proximity for local messages from other actors
};

struct character_list_type {
    int count;
    int max;
    struct character_node_type **character;
};

/* create the maps struct */
struct map_node_type{
    char map_name[1024];    // eg Isla Prima
    char elm_filename[1024];// eg startmap.elm
    int map_axis;
    unsigned char tile_map[50000];
    int tile_map_size;
    unsigned char height_map[50000];
    int height_map_size;
    unsigned char object_3d_map[50000];
    int object_3d_map_size;
    unsigned char object_2d_map[50000];
    int object_2d_map_size;
};

struct map_list_type {
    int count;
    int max;
    struct map_node_type **map;
};

/* create the guilds struct */
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

/* create the clients struct */
struct client_node_type{
    enum {LOGGED_IN, CONNECTED, LOGGED_OUT} status;
    int sock;
    int packet_buffer[1024];
    int packet_buffer_length;
    int character_id;
    int path[25];
    int path_max;
    int path_count;
    int move_buffer[MOVE_BUFFER_MAX];
    int move_buffer_size;
    time_t time_of_last_move;
    //signed long int time_of_last_move;
    //unsigned char *cmd_buffer[10];
    unsigned char cmd_buffer[10][1024];
    int cmd_buffer_end;
 };

struct client_list_type {
    int count;
    int max;
    struct client_node_type **client;
};

struct channel_node_type{
    enum {CHAN_SYSTEM, CHAN_PERMANENT, CHAN_GUILD, CHAN_CHAT, CHAN_VACANT} chan_type;
    char channel_name[1024];
    int channel_id;
    int owner_id; /* could be char or guild depending on chan_type */
    char password[1024];
    char description[1024];
};

struct channel_list_type {
    int count;
    int max;
    int spool_size;
    struct channel_node_type **channel;
};

//Declare structs as global to make passing them to functions more practical
struct guild_list_type guilds;
struct character_list_type characters;
struct map_list_type maps;
struct client_list_type clients;
struct channel_list_type channels;

//other structs
struct timeval time_check;

#endif // GLOBAL_H_INCLUDED
