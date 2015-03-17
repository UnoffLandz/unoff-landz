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
