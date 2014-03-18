#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "database.h"

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

void initialise_client_list(int max_nodes){

    int i=0;

    // zero our struct data
    clients.count=0;
    clients.max=max_nodes;

    // allocate memory for our struct
    if( !(clients.client=malloc(sizeof(struct client_node_type*)*max_nodes))) {
        perror ("unable to allocate suffient memory for client struct");
        exit (EXIT_FAILURE);
    }

    // allocate memory for our struct nodes
    for(i=0; i<max_nodes; i++){

        if( !(clients.client[i]=malloc(sizeof(struct client_node_type)))) {
            perror ("unable to allocate suffient memory for client node struct");
            exit (EXIT_FAILURE);
        }

        clients.client[i]->status=LOGGED_OUT;
        clients.client[i]->path_count=0;
        //clients.client[i]->cmd_buffer_end=0;

     }
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
