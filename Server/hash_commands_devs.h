#ifndef HASH_COMMANDS_DEVS_H_INCLUDED
#define HASH_COMMANDS_DEVS_H_INCLUDED


/** RESULT  : lists available maps

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_list_maps(int actor_node, char *text);


/** RESULT  : extended map information

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_map(int actor_node, char *text);


/** RESULT  : ops kicks a member from a a guild

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_track(int actor_node, char *text);


/** RESULT  : trace walkable tiles

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_trace(int actor_node, char *text);


/** RESULT  : creates an ascii grid illustrating tiles explored to create the path

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_trace_explore(int actor_node, char *text);


/** RESULT  : creates an ascii grid illustrating the path

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_trace_path(int actor_node, char *text);


/** RESULT  : set the map name

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_map_name(int actor_node, char *text);


/** RESULT  : set the map description

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_map_description(int actor_node, char *text);


/** RESULT  : set the map author

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_map_author(int actor_node, char *text);


/** RESULT  : set the map author email

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_map_author_email(int actor_node, char *text);


/** RESULT  : set the map development status

    RETURNS : void

    PURPOSE :

    NOTES   :
*/
int hash_set_map_development_status(int actor_node, char *text);


/** RESULT  : determines an inventory item image

    RETURNS : void

    PURPOSE : determines an item inventory image

    NOTES   :
*/
int hash_object(int actor_node, char *text);



#endif // HASH_COMMANDS_DEVS_H_INCLUDED
