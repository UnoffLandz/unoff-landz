#ifndef CHARACTER_MOVEMENT_H_INCLUDED
#define CHARACTER_MOVEMENT_H_INCLUDED

int get_move_command_vector(int cmd, int tile_pos, int map_axis);

int get_heading(int tile_pos, int tile_dest, int map_axis);

void create_path(int connection, int tile_pos, int tile_dest);

void process_char_move(int connection, time_t current_time);

void enqueue_move_to(int connection, int tile_dest);

void dequeue_move_to(int connection, int *tile_dest);

int get_direct_path(int position, int destination, int map_id, int *direct_path_count, int direct_path[]);

int get_indirect_path(int position, int destination, int map_id, int *indirect_path_count, int indirect_path[]);



#endif // CHARACTER_MOVEMENT_H_INCLUDED
