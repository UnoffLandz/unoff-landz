#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#define MAX_NPC_TRIGGERS 10
#define MAX_NPC_ACTIONS 10
#define MAX_NPC_OPTIONS 10

struct npc_trigger_type{

    int actor_node;

    enum {

        TOUCHED,
        SELECT_OPTION,
    }trigger_type;

    int select_option;

    int action_node;
};
extern struct npc_trigger_type npc_trigger[MAX_NPC_TRIGGERS];


struct npc_action_type{

    int actor_node;

    enum {

        GIVE_OPTIONS,
        SELL,
    }action_type;

    char text[80];
    char option_list[1024];
    char text_success[80];
    char text_fail[80];
    int object_id_required;
    int object_amount_required;
    int choice;

    enum {

        OBJECT,
        BOAT_TICKET
    }sell_item;

    //int map_id;
    //int map_tile;
    int boat_schedule_node;
};
extern struct npc_action_type npc_action[MAX_NPC_ACTIONS];


/** RESULT   : finds next free node in the npc struct

    RETURNS  : void

    PURPOSE  : enables decoupling of actor nodes and npc nodes which reduces the amount
               of memory used by the client struct

    NOTES    :
**/
int get_next_free_npc_node();


/**
Essentially what we have for each NPC is a series of dev (you guys) specified instructions to perform certain actions on certain triggers. The range of triggers divide roughly into:

    touched by player char
    time triggered, ie time of day
    environment triggered, ie rain, dry, light, dark
    server triggered, ie server start
    response to player char selection of an npc option.

In terms of actions, we currently have :

    move - walk to a specific tile on the current map
    appear - become visible at a specific tile and map id
    disappear - become invisible
    talk - respond directly to the interrogating char with specified npc text
    options - respond directly to the interrogating char with specified npc options
    wear - wear/unwear an item

So the script for our boat npc might look something like:

On [server start] appear at map [x] tile [x]
On midnight plus [3] hours [0] minutes move [tile] //npc takes 30 mins lunch at tavern
On midnight plus [3] hours [30] minutes move [tile] //npc returns from lunch
On midnight plus [5] hours [0] minutes disappear //npc takes 1 hour siesta
On midnight plus [0] hours [0] minutes reappear at [map id] [tile]//npc returns from siesta
On [touched] give option header [Hello I'm the boat npc. Do you want]
On [touched] give option 1 [a ticket to New Platts]
On [touched] give option 2 [a ticket to somewhere else]
On [touched] give option 3 [a schedule of boat departures]
On [option 1] take [5][carrots]
On [option 2] take [10] [carrots]
On [option 3] talk [boat schedule]
On [receive] [5][carrots] give [boat ticket] [map_id] [tile]
On [receive] [5][carrots] talk [thank you for your carrots]
On [receive [10] carrots give [boat ticket] [map_id [tile]
On [receive] [10][carrots] talk [thank you for your carrots]

Ok, so the problem with the above comes from the need to periodically check every action for a time based trigger. This kind of 'busy loop' is very wasteful so I need something more efficient.

**/


#endif // NPC_H_INCLUDED
