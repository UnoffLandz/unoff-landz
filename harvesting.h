#ifndef HARVESTING_H_INCLUDED
#define HARVESTING_H_INCLUDED

#define MAX_INVENTORY_SLOTS 36

//struct to carry data returned by functions find_inventory_item and new_inventory_item
struct inventory_type{
    int slot;
    int item_id;
    int image_id;
    int amount;
    int flags;
};

struct inventory_type inventory;

void process_harvesting(int connection, time_t current_time);

int find_inventory_item(int connection, int find_item_id);

void new_inventory_item(int connection, int item_id);

#endif // HARVESTING_H_INCLUDED
