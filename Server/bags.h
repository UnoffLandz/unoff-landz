#ifndef BAGS_H_INCLUDED
#define BAGS_H_INCLUDED

#define MAX_BAGS 50
#define MAX_PLAYER_BAGS 10
#define MAX_BAG_SLOTS 50

#include <time.h>

struct bag_inventory_type{

    int object_id;
    int amount;
};

struct bag_type {

    int character_id;
    time_t bag_created;
    int bag_type_id;
    int tile;
    int map_id;
    bool bag_in_use;

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
int find_bag_slot(int connection, int object_id);


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
bool is_bag_empty(int bag_id);

#endif // BAGS_H_INCLUDED
