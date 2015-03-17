#ifndef CHARACTER_MOVEMENT_H_INCLUDED
#define CHARACTER_MOVEMENT_H_INCLUDED

#include <ev.h>         // evlib event library

#define PATH_MAX 100    // maximum tiles of precalculated character movement

enum{//return values add_char_to_map
    ADD_MAP_ILLEGAL,
    ADD_MAP_UNREACHABLE,
    ADD_MAP_SUCESS
};


enum{//return values remove_char_from_map
    REMOVE_MAP_ILLEGAL,
    REMOVE_MAP_SUCESS
};


/** RESULT  : Removes actor from map

    RETURNS : REMOVE_MAP_ILLEGAL - map doesn't exist
              REMOVE_MAP_SUCCESS - character was removed from map

    PURPOSE : used at log-out to remove a char from map. Also on map jumps
**/
int remove_char_from_map(int connection);


/** RESULT  : adds a character to a map

    RETURNS : ADD_MAP_ILLEGAL - map doesn't exist
              ADD_MAP_FAILED  - no unoccupied tiles on map
              ADD_MAP_SUCCESS - character was added to map

    PURPOSE : used at log-in to add a char to a map. Also by function move_char_between_maps
**/
int add_char_to_map(int connection, int new_map_id, int map_tile);


/** RESULT  : moves a character between maps

    RETURNS : void

    PURPOSE : supports map jumps
**/
void move_char_between_maps(int connection, int new_map_id, int new_map_tile);


/** RESULT  : moves a character one step along the path

    RETURNS : void

    PURPOSE : makes the character move
**/
void process_char_move(int connection, time_t current_utime);


/** RESULT  : starts the character moving

    RETURNS : void

    PURPOSE : makes the character move
**/
//void start_char_move(int connection, int destination, struct ev_loop *loop);
void start_char_move(int connection, int destination);

#endif // CHARACTER_MOVEMENT_H_INCLUDED
