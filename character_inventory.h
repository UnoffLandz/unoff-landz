#ifndef CHARACTER_INVENTORY_H_INCLUDED
#define CHARACTER_INVENTORY_H_INCLUDED

#define MAX_INVENTORY_SLOTS 36
#define INVENTORY_STRING_LENGTH 1+(MAX_INVENTORY_SLOTS*8)
#define MAX_BAG_SLOTS 10
#define MAX_BAG_SPAM 10

/** RESULT  : sends entire char inventory

    RETURNS : void

    PURPOSE : send char inventory at log in

    USAGE   : protocol.c process_packet
*/
void send_here_your_inventory(int connection);


/** RESULT  : updates a specific slot in the char inventory

    RETURNS : void

    PURPOSE : send char inventory at log in

    USAGE   : harvesting.c
*/
void send_get_new_inventory_item(int connection, int item_image_id, int amount, int slot);

int get_used_inventory_slot(int connection, int image_id, int *slot);

int get_unused_inventory_slot(int connection, int *slot);

void get_inventory_string(int connection, unsigned char *inventory_str);

int get_inventory_emu(int connection);

int get_char_carry_capacity(int connection);

void send_get_new_bag(int connection, int bag_id);

void send_destroy_bag(int connection, int bag_id);

int bag_exists(int map_id, int tile_pos, int *bag_id);

void send_here_your_ground_items(int connection, int bag_id);

void send_get_new_ground_item(int connection, int item_image_id, int amount, int slot);

int bag_is_empty(int bag_id);

void send_s_close_bag(int connection);

int create_empty_bag(int map_id, int tile, int *bag_id, struct ev_loop *loop);

int add_item_to_inventory(int connection, int image_id, int amount, int *inventory_slot);

int add_item_to_bag(int bag_id, int image_id, int amount, int *bag_slot, struct ev_loop *loop);

int remove_item_from_inventory(int connection, int image_id, int amount, int slot);

int remove_item_from_bag(int bag_id, int image_id, int amount, int *bag_slot, struct ev_loop *loop);

int get_inventory_capacity(int connection, int image_id);

int is_inventory_slots_exceeded(int connection, int image_id);

void pick_up_from_bag(int connection, int bag_slot, struct ev_loop *loop);

void drop_from_inventory(int connection, int inventory_slot, int drop_amount, struct ev_loop *loop);

#endif // CHARACTER_INVENTORY_H_INCLUDED
