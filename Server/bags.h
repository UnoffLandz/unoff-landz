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

int create_bag(int connection, int map_id, int tile);

#endif // BAGS_H_INCLUDED
