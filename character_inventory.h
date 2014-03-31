#ifndef CHARACTER_INVENTORY_H_INCLUDED
#define CHARACTER_INVENTORY_H_INCLUDED

#define MAX_INVENTORY_SLOTS 36
#define INVENTORY_STRING_LENGTH 1+(MAX_INVENTORY_SLOTS*8)

int get_used_inventory_slot(int connection, int image_id);

int get_unused_inventory_slot(int connection);

void get_inventory_string(int connection, unsigned char *inventory_str);

int get_inventory_emu(int connection);

int get_char_carry_capacity(int connection);


#endif // CHARACTER_INVENTORY_H_INCLUDED
