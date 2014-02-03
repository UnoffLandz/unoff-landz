#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"


void initialise_harvestables(){

    harvestables[chrysanthemums].exp=10;
    harvestables[chrysanthemums].emu=1;
    harvestables[chrysanthemums].nexus=0;
    strcpy(harvestables[chrysanthemums].name, "Chrysanthemums");
    harvestables[chrysanthemums].interval=1;
    harvestables[chrysanthemums].image_id=28;
    harvestables[chrysanthemums].food_value=0;
    harvestables[chrysanthemums].food_cool_down=0;
}

void initialise_movement_vectors(){

    vector_x[0]=0; vector_y[0]=1; movement_cmd[0]=20;
    vector_x[1]=1; vector_y[1]=1; movement_cmd[1]=21;
    vector_x[2]=1; vector_y[2]=0; movement_cmd[2]=22;
    vector_x[3]=1; vector_y[3]=-1; movement_cmd[3]=23;
    vector_x[4]=0; vector_y[4]=-1; movement_cmd[4]=24;
    vector_x[5]=-1; vector_y[5]=-1; movement_cmd[5]=25;
    vector_x[6]=-1; vector_y[6]=0; movement_cmd[6]=26;
    vector_x[7]=-1; vector_y[7]=1; movement_cmd[7]=27;
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
        clients.client[i]->cmd_buffer_end=0;

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

void initialise_character_list(int max_nodes){

    int i;

    /* zero our struct data */
    characters.count=0;
    characters.max=max_nodes;

    /* allocate memory for our struct */
    if( !(characters.character=malloc(sizeof(struct character_node_type*)*max_nodes))){
        perror ("unable to allocate suffient memory for character struct");
        exit (EXIT_FAILURE);
    }

    /* allocate memory for our struct nodes */
    for(i=0; i<max_nodes; i++){

        if( !(characters.character[i]=malloc(sizeof(struct character_node_type)))) {
            perror ("unable to allocate suffient memory for character node struct");
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
