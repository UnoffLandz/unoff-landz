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

    PURPOSE : determine if inventory is overloaded

    NOTES   :the server keeps a record of the total inventory emu in the client struct in order to
             avoid having to continually recalculate this value from scratch. The value is initially
             set when the char logs-in using the get_inventory_emu function

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


/** RESULT  : broadcasts a new bag to connected clients

    RETURNS : void

    PURPOSE : supports the DROP_ITEM protocol

    NOTES   :
*/
void broadcast_drop_item_packet(int connection);

#endif // CHARACTER_INVENTORY_H_INCLUDED

