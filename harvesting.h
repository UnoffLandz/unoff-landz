#ifndef HARVESTING_H_INCLUDED
#define HARVESTING_H_INCLUDED

#define MAX_INVENTORY_SLOTS 36
#define INVENTORY_STRING_LENGTH 1+(MAX_INVENTORY_SLOTS*8)
#define MAX_EMU 100

void process_harvesting(int connection, time_t current_time);

int get_used_inventory_slot(int connection, int image_id);

int get_unused_inventory_slot(int connection);

void get_inventory_string(int connection, unsigned char *inventory_str);

#endif // HARVESTING_H_INCLUDED
