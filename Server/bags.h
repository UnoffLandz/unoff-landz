#ifndef BAGS_H_INCLUDED
#define BAGS_H_INCLUDED

#define MAX_BAGS 50
#define MAX_PLAYER_BAGS 10
#define MAX_BAG_SLOTS 50
#define BAG_POOF_INTERVAL 100
#define BAG_LIST_MAX 10

#include <time.h>

struct bag_inventory_type{

    int object_id;
    int amount;
};

struct bag_type {

    int character_id;
    time_t bag_refreshed;
    int bag_type_id;
    int tile;
    int map_id;
    bool bag_in_use;
    int bag_emu;

    struct bag_inventory_type inventory[MAX_BAG_SLOTS];
};

extern struct bag_type bag[MAX_BAGS];


/** RESULT  : creates a new bag

    RETURNS : bag_id

    PURPOSE :

    NOTES   :
*/
int create_bag(int map_id, int tile);


/** RESULT  : finds an existing inventory slot with an item or the next free slot

    RETURNS : slot number or -1 if no existing or free slot found

    PURPOSE : used in function: start_harvesting

    NOTES   :
*/
int find_bag_slot(int actor_node, int object_id);


/** RESULT  : adds objects to the bag

    RETURNS : the amount actually added to bag

    PURPOSE : used in function: process_packet DROP_ITEM protocol

    NOTES   :
*/
int add_to_bag(int bag_id, int object_id, int amount, int slot);


/** RESULT  : determines if bag is empty

    RETURNS : true/false

    PURPOSE :

    NOTES   :
*/
bool bag_empty(int bag_id);


/** RESULT  : removes item from a bag

    RETURNS : true/false

    PURPOSE :

    NOTES   : doesn't reset bag poof time
*/
void remove_item_from_bag(int bag_id, int amount, int bag_slot);


/** RESULT  : calculates the emu of a bag

    RETURNS : bag emu

    PURPOSE :

    NOTES   : function is unused but reserved for future development
*/

int get_bag_inventory_emu(int bag_id);


/** RESULT  : broadcasts bag drop to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_get_new_bag_packet(int actor_node, int bag_id);


/** RESULT  : broadcasts bag destroy to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_destroy_bag_packet(int bag_id);


/** RESULT  : reveal/conceal bags to this client

    RETURNS : void

    PURPOSE :
**/
void broadcast_bags_to_client(int actor_node, int last_tile);

#endif // BAGS_H_INCLUDED
