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

#ifndef BROADCAST_ACTOR_FUNCTIONS_H_INCLUDED
#define BROADCAST_ACTOR_FUNCTIONS_H_INCLUDED

/** RESULT  : broadcasts the enhanced_new_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character addition

    NOTES   :
*/
void broadcast_add_new_enhanced_actor_packet(int connection);


/** RESULT  : broadcasts the remove_actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character removal

    NOTES   :
*/
void broadcast_remove_actor_packet(int sender_connection);


/** RESULT  : broadcasts the actor packet to all clients in range

    RETURNS : void

    PURPOSE : broadcasts character movement

    NOTES   :
*/
void broadcast_actor_packet(int sender_connection, unsigned char move, int sender_destination_tile);


/** RESULT  : broadcasts an event to all clients in the chat channel

    RETURNS : void

    PURPOSE : announces players joining and leaving the chat channel

    NOTES   :
*/
void broadcast_channel_event(int chan, int connection, char *text_in);


/** RESULT  : broadcasts channel chat to all clients in the chat channel

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_channel_chat(int chan, int connection, char *text_in);


/** RESULT  : broadcasts local chat to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_local_chat(int connection, char *text_in);

#endif // BROADCAST_ACTOR_FUNCTIONS_H_INCLUDED
