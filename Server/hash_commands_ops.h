#ifndef HASH_COMMANDS_OPS_H_INCLUDED
#define HASH_COMMANDS_OPS_H_INCLUDED


/** RESULT  : jumps char to a new map at (x/y)cartesian coordinates

    RETURNS : void

    PURPOSE : allows map developers to jump to unconnected maps

    NOTES   :
*/
int hash_jump(int actor_node, char *text);


/** RESULT  : OPS command to create guild

    RETURNS : void

    PURPOSE : OPS only

    NOTES   :
*/
int hash_ops_create_guild(int actor_node, char *text);


/** RESULT  : OPS command to appoint a player to a guild

    RETURNS : void

    PURPOSE : OPS only

    NOTES   :
*/
int hash_ops_appoint_guild_member(int actor_node, char *text);


/** RESULT  : OPS command to send message to all connected clients

    RETURNS : void

    PURPOSE : OPS only

    NOTES   :
*/
int hash_server_message(int actor_node, char *text);


/** RESULT  : change a chars guild rank

    RETURNS : void

    PURPOSE : OPS only

    NOTES   :
*/
int hash_ops_change_guild_member_rank(int actor_node, char *text);


/** RESULT  : change a guilds permission level

    RETURNS : void

    PURPOSE : OPS only

    NOTES   :
*/
int hash_ops_change_guild_permission(int actor_node, char *text);


/** RESULT  : ops kicks a member from a a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_ops_kick_guild_member(int actor_node, char *text);


#endif // HASH_COMMANDS_OPS_H_INCLUDED
