/******************************************************************************************************************
	Copyright 2014, 2015, 2016 UnoffLandz

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

#ifndef BROADCAST_EQUIPABLES_H_INCLUDED
#define BROADCAST_EQUIPABLES_H_INCLUDED

/** RESULT  : broadcasts the actor_wear_item packet to all clients in range

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_actor_equip_item(int actor_node, int equipable_item_type, int equipable_item_id);


/** RESULT  : broadcasts the actor_unwear_item packet to all clients in range

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_actor_unequip_item(int actor_node, int equipable_item_type, int equipable_item_id);

#endif // BROADCAST_EQUIPABLES_H_INCLUDED
