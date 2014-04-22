#ifndef FILES_H_INCLUDED
#define FILES_H_INCLUDED

#include "global.h"

#define ITEM_DATA_FILE          "item_data.txt"
#define THREED_OBJECT_DATA_FILE "threed_object_data.txt"
#define MAP_DATA_FILE           "map_data.txt"
#define CHANNEL_DATA_FILE       "channel_data.txt"
#define RACE_DATA_FILE          "race_data_txt"
#define GUILD_DATA_FILE         "guild_data_txt"
#define BAG_TYPE_DATA_FILE      "bag_type_data.txt"

#define DATABASE_FILE_NAME      "unoff.db"

#define CHARACTER_LOG_FILE_NAME      "character.log"
#define ERROR_LOG_FILE_NAME          "error.log"
#define SESSION_LOG_FILE_NAME        "session.log"
#define CHAT_LOG_FILE_NAME           "chat.log"
#define MOVE_LOG_FILE_NAME           "move.log"
#define INITIALISATION_LOG_FILE_NAME "initialisation.log"

#define ELM_FILE_VERSION 1

#define ITEM_DATA_FILE_FORMAT   "UNOFFLANDZ Item data file\n" \
                                "\n" \
                                "Image Item                                           Food  Food     Organic Vegetal\n" \
                                "ID    Name              Harvestable EMU Interval EXP Value Cooldown Nexus   Nexus  \n" \
                                "------------------------------------------------------------------------------------------\n"

#define THREED_OBJECT_DATA_FILE_FORMAT  "#UNOFFLANDZ 3d object data file\n" \
                                        "#\n" \
                                        "#Image e3d      \n" \
                                        "#ID    File Name\n" \
                                        "---------------\n"

#define MAP_DATA_FILE_FORMAT    "#UNOFFLANDZ map data file\n" \
                                "#\n" \
                                "#Map   Map         ELM            \n" \
                                "#ID    Name        File name      \n" \
                                "---------------------------------\n"

#define CHANNEL_DATA_FILE_FORMAT    "#UNOFFLANDZ channel data file\n" \
                                    "#\n" \
                                    "#Channel Channel  Channel    Channel       Channel    \n" \
                                    "#ID      Type     Password   Name          Description\n" \
                                    "-----------------------------------------------------\n"

#define RACE_DATA_FILE_FORMAT   "#UNOFFLANDZ race data file\n" \
                                "#\n" \
                                "#                                                        Initial              Initial              Initial Night      \n" \
                                "#Race Race          Race              Initial Emu        Visual    Proximity  Chat      Chat       Night   Vis        \n" \
                                "#ID   Name          Description       Emu     Multiplier Proximity Multiplier Proximity Multiplier Vis     Multiplier \n" \
                                "---------------------------------------------------------------------------------------------------------------------\n"

#define GUILD_DATA_FILE_FORMAT  "#UNOFFLANDZ guild data file\n" \
                                "#\n" \
                                "#Guild Guild  Guild           Guild             Tag    Log On Log Off Chan   Chan\n" \
                                "#ID    Tag    Name            Description       Colour Colour Colour  Colour ID  \n" \
                                "--------------------------------------------------------------------------------\n"

#define BAG_TYPE_DATA_FILE_FORMAT  "#UNOFFLANDZ bag type data file\n" \
                                   "#\n" \
                                   "#Bag   Bag\n" \
                                   "#Type  Type            Poof  Max\n" \
                                   "#Token Description     Time  Emu\n" \
                                   "--------------------------------\n"

enum { //log events
    EVENT_NEW_CHAR,
    EVENT_ERROR,
    EVENT_SESSION,
    EVENT_CHAT,
    EVENT_MOVE_ERROR,
    EVENT_INITIALISATION
};

void clear_file(char *file_name);

int load_map(int map_id);

int get_file_size(char *file_name);

void log_event(int event_type, char *text_in);
void log_event2(int event_type, char *fmt, ...);

void load_database_item_table_data(char *file_name);

void load_database_threed_object_table_data(char *file_name);

void load_database_map_table_data(char *file_name);

void load_database_channel_table_data(char *file_name);

void load_database_race_table_data(char *file_name);

void load_database_guild_table_data(char *file_name);

void load_database_bag_type_table_data(char *file_name);

void load_e3d(char *filename);

#endif // FILES_H_INCLUDED
