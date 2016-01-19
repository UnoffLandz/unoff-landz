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

#ifndef BROADCAST_MOVEMENT_H_INCLUDED
#define BROADCAST_MOVEMENT_H_INCLUDED

/** RESULT  : broadcasts the enhanced_new_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character addition

    NOTES   :
*/
void broadcast_add_new_enhanced_actor_packet(int actor_node);


/** RESULT  : broadcasts the remove_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character removal

    NOTES   :
*/
void broadcast_remove_actor_packet(int actor_node);


/** RESULT  : broadcasts the actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character movement

    NOTES   :
*/
void broadcast_actor_packet(int actor_node, unsigned char move, int sender_destination_tile);


/** RESULT  : broadcasts bag drop to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_get_new_bag_packet(int actor_node, int bag_id);


/** RESULT  : broadcasts bag destroy to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_destroy_bag_packet(int bag_id);


/** RESULT  : reveal/conceal bags to this client

    RETURNS : void

    PURPOSE :
**/
void broadcast_bags_to_client(int actor_node, int last_tile);

#endif // BROADCAST_MOVEMENT_H_INCLUDED
