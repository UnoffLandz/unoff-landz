#ifndef CHARACTER_MOVEMENT_H_INCLUDED
#define CHARACTER_MOVEMENT_H_INCLUDED

int get_move_command_vector(int cmd, int tile_pos, int map_axis);

int get_heading(int tile_pos, int tile_dest, int map_axis);

void process_char_move(int connection, time_t current_time);

void enqueue_move_to(int connection, int tile_dest);

/** RESULT  : Move a char between maps

    RETURNS : void

    PURPOSE : Consolidate all required operations into a resuable function that can be called
              to move a char between maps

    USAGE   : protocol.c process_packet
*/
void move_char_between_maps(int connection, int new_map_id, int new_map_tile);

/** RESULT  : if the target tile is occupied, finds nearest unoccupied tile

    RETURNS : address of the nearest unoccupied tile

    PURPOSE : To ensure that actors don't move to occupied tiles

    USAGE   : protocol.c add_char_to_map / hash_commands.c process_hash_command
*/
int get_nearest_unoccupied_tile(int map_id, int map_tile);

/** RESULT  : sends actor to map

    RETURNS : LEGAL_MAP / ILLEGAL_MAP

    PURPOSE : Consolidate all required operations into a resuable function that can be called
                  at login and on map change

    USAGE   : protocol.c process_packet
*/
int add_char_to_map(int connection, int new_map_id, int map_tile);

#endif // CHARACTER_MOVEMENT_H_INCLUDED
