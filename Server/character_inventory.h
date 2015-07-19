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

#define MAX_INVENTORY_SLOTS 36

struct client_inventory_type {
        int image_id;
        int amount;
        int slot;
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

    PURPOSE : to support variable carry capacity based on race and attributes

    NOTES   :
*/
int get_inventory_emu(int connection);


/** RESULT  : adds objects to the char inventory

    RETURNS : void

    PURPOSE : common function to harvesting and picking up bags

    NOTES   :
*/
void add_item_to_inventory(int connection, int object_id, int amount);


/** RESULT  : moves objects between slots in the char inventory

    RETURNS : void

    PURPOSE : supports the MOVE_INVENTORY_ITEM protocol

    NOTES   :
*/
void move_inventory_item(int connection, int from_slot, int to_slot);


#endif // CHARACTER_INVENTORY_H_INCLUDED

