#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "database.h"
#include "files.h"

void create_new_database() {

    //create database tables
    log_event2(EVENT_INITIALISATION, "New database. Creating tables...");

    create_database_table(CHARACTER_TABLE_SQL);
    create_database_table(INVENTORY_TABLE_SQL);
    create_database_table(ITEM_TABLE_SQL);
    create_database_table(THREED_OBJECT_TABLE_SQL);
    create_database_table(MAP_TABLE_SQL);
    create_database_table(CHANNEL_TABLE_SQL);
    create_database_table(RACE_TABLE_SQL);
    create_database_table(GUILD_TABLE_SQL);
    create_database_table(BAG_TYPE_TABLE_SQL);
    create_database_table(CHARACTER_TYPE_TABLE_SQL);

    log_event2(EVENT_INITIALISATION, "---");

    //populate database tables with initial data
    load_database_item_table_data(ITEM_DATA_FILE);
    load_database_threed_object_table_data(THREED_OBJECT_DATA_FILE);
    load_database_map_table_data(MAP_DATA_FILE);
    load_database_channel_table_data(CHANNEL_DATA_FILE);
    load_database_race_table_data(RACE_DATA_FILE);
    load_database_guild_table_data(GUILD_DATA_FILE);
    load_database_bag_type_table_data(BAG_TYPE_DATA_FILE);
    load_database_char_type_table_data(CHARACTER_TYPE_DATA_FILE);
}

void initialise_logs(){

    //clear initialisation log first, otherwise the log record for its initialisation will be lost
    clear_file(INITIALISATION_LOG_FILE_NAME);

    clear_file(ERROR_LOG_FILE_NAME);
    clear_file(CHARACTER_LOG_FILE_NAME);
    clear_file(SESSION_LOG_FILE_NAME);
    clear_file(CHAT_LOG_FILE_NAME);
    clear_file(MOVE_LOG_FILE_NAME);
    log_event2(EVENT_INITIALISATION, "---");
}

void initialise_maps(){

    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_maps();
}

void initialise_3d_objects(){

    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_3d_objects();
}

void initialise_items(){

    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_items();
}

void initialise_channels(){

    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_channels();
}

void initialise_races(){
    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_races();
}

void initialise_guilds(){
    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_guilds();
}

void initialise_bag_types(){
    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_bag_types();
}

void initialise_character_types(){

    /*function ensures that all database related actions are confined to the database.c module. This makes it easier
    to change from sqlite to mysql at sometime in the future */

    load_character_types();
}

void initialise_bag_list(){

    memset(&bag_list, 0, sizeof(bag_list));

}

void initialise_movement_vectors(){

    vector[0].x=0; vector[0].y=1; vector[0].move_cmd=20;
    vector[1].x=1; vector[1].y=1; vector[1].move_cmd=21;
    vector[2].x=1; vector[2].y=0; vector[2].move_cmd=22;
    vector[3].x=1; vector[3].y=-1; vector[3].move_cmd=23;
    vector[4].x=0; vector[4].y=-1; vector[4].move_cmd=24;
    vector[5].x=-1; vector[5].y=-1; vector[5].move_cmd=25;
    vector[6].x=-1; vector[6].y=0; vector[6].move_cmd=26;
    vector[7].x=-1; vector[7].y=1; vector[7].move_cmd=27;
}

void initialise_character_genders(){

    strcpy(character_gender[0].description, "Female");
    strcpy(character_gender[1].description, "Male");
}

void initialise_map_list(int max_nodes){

    int i=0;

    // zero our struct data
    maps.count=0;
    maps.max=max_nodes;

    // allocate memory for our struct
    if( !(maps.map=malloc(sizeof(struct map_node_type*)*max_nodes))) {
        perror ("unable to allocate sufficient memory for map struct");
        exit (EXIT_FAILURE);
    }

    // allocate memory for our struct nodes
    for(i=0; i<max_nodes; i++){

        if( !(maps.map[i]=malloc(sizeof(struct map_node_type)))) {
            perror ("unable to allocate sufficient memory for map node struct");
            exit (EXIT_FAILURE);
        }
    }

}

void initialise_guild_list(int max_nodes){

    int i;

    /* zero our struct data */
    guilds.count=0;
    guilds.max=max_nodes;

    /* allocate memory for our struct */
    if( !(guilds.guild=malloc(sizeof(struct guild_node_type*)*max_nodes))) {
        perror ("unable to allocate sufficient memory for guild struct");
        exit (EXIT_FAILURE);
    }

    /* allocate memory for our struct
    if( !(guilds.guild=malloc(sizeof(struct node_type*)*max_nodes))) {
        perror ("unable to allocate sufficient memory for guild struct");
        exit (EXIT_FAILURE);
    }*/

    /* allocate memory for our struct nodes */
    for(i=0; i<max_nodes; i++){

        if( !(guilds.guild[i]=malloc(sizeof(struct guild_node_type)))) {
            perror ("unable to allocate sufficient memory for guild node struct");
            exit (EXIT_FAILURE);
        }
    }
}

void initialise_channel_list(int max_nodes){

    int i;

    /* zero our struct data */
    channels.count=0;
    channels.max=max_nodes;

    /* allocate memory for our struct */
    if( !(channels.channel=malloc(sizeof(struct channel_node_type*)*max_nodes))){
        perror ("unable to allocate suffient memory for channel struct");
        exit (EXIT_FAILURE);
    }

    /* allocate memory for our struct
    if( !(channels.channel=malloc(sizeof(struct channel_node_type*)*max_nodes))){
        perror ("unable to allocate suffient memory for channel struct");
        exit (EXIT_FAILURE);
    }*/

    /* allocate memory for our struct nodes */
    for(i=0; i<max_nodes; i++){

        if( !(channels.channel[i]=malloc(sizeof(struct channel_node_type)))) {
            perror ("unable to allocate suffient memory for channel node struct");
            exit (EXIT_FAILURE);
        }
        channels.channel[i]->chan_type=CHAN_VACANT;
    }
}
