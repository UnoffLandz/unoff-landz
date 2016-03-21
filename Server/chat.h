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
******************************************************************************************************************/

#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

#include <stdbool.h> //support for bool data type

#define MAX_CHANNELS 10
#define MAX_CHAN_SLOTS 3
#define LOCAL_CHAT_RANGE 10

#define CHANNELS_FILE "channels.lst"


struct channel_node_type{

    enum {CHAN_VACANT, CHAN_SYSTEM, CHAN_PERMANENT, CHAN_UNUSED, CHAN_CHAT} chan_type;
    char channel_name[80];
    int owner_id; // could be char or guild depending on chan_type
    char password[80];
    char description[80];
    bool new_chars;
};

struct channel_list_type {

    bool data_loaded;
    struct channel_node_type channel[MAX_CHANNELS];
};
extern struct channel_list_type channels;


enum { //return values from process_guild_chat
    GM_INVALID,
    GM_NO_GUILD,
    GM_NO_PERMISSION,
    GM_SENT
};

enum { //return values for join_channel
    CHANNEL_JOINED,
    CHANNEL_NOT_JOINED
};

enum { //return values for leave channel
    CHANNEL_LEFT,
    CHANNEL_NOT_LEFT
};


/** RESULT  : determines if a player has a chan open

    RETURNS : -1 if not in chan or chan slot number

    PURPOSE : used by list_clients_in_chan, leave_channel, broadcast_channel_chat

    NOTES   :
**/
int player_in_chan(int actor_node, int chan);


/** RESULT  : joins a chat_channel

    RETURNS : CHANNEL_JOINED/CHANNEL_NOT_JOINED

    PURPOSE :

    NOTES   :
**/
int join_channel(int actor_node, int chan);


/** RESULT  : leaves a chat_channel

    RETURNS : CHANNEL_LEFT/CHANNEL_NOT_LEFT

    PURPOSE :

    NOTES   :
**/
int leave_channel(int actor_node, int chan);


/** RESULT  : send a private message

    RETURNS : void

    PURPOSE :

    NOTES   :
**/
void send_pm(int actor_node, char *receiver_name, char *message);


/** RESULT  : sends chat channel data to client

    RETURNS : void

    PURPOSE : wrapper for send_get_active_channels

    NOTES   : when the client is disconnected from server, it still allows chat chan tabs to
              be closed. However, if the client reconnects, the chan tabs are not automatically
              reopened, leading to a situation in which the client remains connected to a
              chat channel but without a corresponding chan tab.

              The solution to this appears to be to send a copy of the get_active_channels
              protocol to the client with all channel data set to zero, then, resend the
              protocol with the required channel data. This is achieved using the
              clear_client_channels and send_client_channels functions
**/
void send_client_channels(int actor_node);


/** RESULT  : clears chat channel tabs on client

    RETURNS : void

    PURPOSE : wrapper for send_get_active_channels

    NOTES   : when the client is disconnected from server, it still allows chat chan tabs to
              be closed. However, if the client reconnects, the chan tabs are not automatically
              reopened, leading to a situation in which the client remains connected to a
              chat channel but without a corresponding chan tab.

              The solution to this appears to be to send a copy of the get_active_channels
              protocol to the client with all channel data set to zero, then, resend the
              protocol with the required channel data. This is achieved using the
              clear_client_channels and send_client_channels functions

**/
void clear_client_channels(int actor_node);


/** RESULT  : Determines if a client has any open chat chans

    RETURNS : true / false

    PURPOSE :

    NOTES   : used in function process_packet
**/
bool chat_chan_open(int actor_node);


/** RESULT  : Determines if a chan is the active chan for a particular client

    RETURNS : true / false

    PURPOSE :

    NOTES   :
**/
bool is_chan_active(int actor_node, int chan);

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


/** RESULT  : Adds default chat channels to a new char

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
void add_new_char_chat_channels();

#endif // CHAT_H_INCLUDED
