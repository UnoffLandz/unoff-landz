#ifndef CHARACTER_INVENTORY_H_INCLUDED
#define CHARACTER_INVENTORY_H_INCLUDED

#define MAX_INVENTORY_SLOTS 36

struct client_inventory_type {
        int image_id;
        int amount;
        int slot;
        int flags;
};
struct client_inventory_type client_inventory;


/** RESULT  : calculates the maximum carry capacity for a char

    RETURNS : the maximum carry capacity for a char

    PURPOSE : to support variable carry capacity based on race and attributes

    NOTES   :
*/
int get_max_inventory_emu(int connection);


/** RESULT  : calculates the emu of items in an inventory

    RETURNS : the total emu of items in an inventory

    PURPOSE : to support variable carry capacity based on race and attributes

    NOTES   :
*/
int get_inventory_emu(int connection);


#endif // CHARACTER_INVENTORY_H_INCLUDED

