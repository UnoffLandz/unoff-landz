/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/


#ifndef CHARACTER_INVENTORY_H_INCLUDED
#define CHARACTER_INVENTORY_H_INCLUDED

#include <stdbool.h>

#define MAX_INVENTORY_SLOTS 36

struct client_inventory_type {

        int object_id;
        int amount;
        int flags;
};
extern struct client_inventory_type client_inventory;


/** RESULT  : calculates the maximum carry capacity for a char

    RETURNS : the maximum carry capacity for a char

    PURPOSE : to support variable carry capacity based on race and attributes

    NOTES   :
*/
int get_max_inventory_emu(int connection);


/** RESULT  : calculates the emu of items in an inventory

    RETURNS : the total emu of items in an inventory

    PURPOSE : used in function process_char_harvest

    NOTES   :
*/
int get_inventory_emu(int connection);


/** RESULT  : finds an existing inventory slot with an item or the next free slot

    RETURNS : slot number or -1 if no existing or free slot found

    PURPOSE : used in function: start_harvesting

    NOTES   :
*/
int find_inventory_slot(int connection, int object_id);


/** RESULT  : adds objects to the char inventory

    RETURNS : true if item is added / false if not added

    PURPOSE : used in function: process_char_harvest

    NOTES   :
*/
bool add_to_inventory(int connection, int object_id, int amount, int slot);


/** RESULT  : removes objects to the char inventory

    RETURNS : amount removed from inventory

    PURPOSE : used in function: process_packet (DROP_ITEM)

    NOTES   : the removal amount passed by the client takes no account of whether there is
              sufficient in the inventory. Hence, the amount returned by the function indicates
              what was actually removed
*/
int remove_from_inventory(int connection, int object_id, int amount, int slot);


/** RESULT  : moves objects between slots in the char inventory

    RETURNS : void

    PURPOSE : supports the MOVE_INVENTORY_ITEM protocol

    NOTES   :
*/
void move_inventory_item(int connection, int from_slot, int to_slot);


/** RESULT  : broadcasts a new bag to connected clients

    RETURNS : void

    PURPOSE : supports the DROP_ITEM protocol

    NOTES   :
*/
void broadcast_drop_item_packet(int connection);

#endif // CHARACTER_INVENTORY_H_INCLUDED

