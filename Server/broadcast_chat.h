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

#ifndef BROADCAST_CHAT_H_INCLUDED
#define BROADCAST_CHAT_H_INCLUDED


/** RESULT  : broadcasts an event to all clients in the chat channel

    RETURNS : void

    PURPOSE : announces players joining and leaving the chat channel

    NOTES   :
*/
void broadcast_channel_event(int chan, int actor_node, char *text_in);


/** RESULT  : broadcasts channel chat to all clients in the chat channel

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_channel_chat(int chan, int actor_node, char *text_in);


/** RESULT  : broadcasts guild chat to all clients in the guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_guild_chat(int guild_id, int actor_node, char *text_in);


/** RESULT  : broadcasts local chat to all characters in the vicinity

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_local_chat(int actor_node, char *text_in);


/** RESULT  : broadcasts server message to all connected clients

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void broadcast_server_message(char *text_in);




#endif // BROADCAST_CHAT_H_INCLUDED
