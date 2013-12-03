#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

void add_client_to_map(int client_id, int map_id);

void remove_client_from_map_list(int client_id, int map_id);

void get_threed_object_pos(int object_id, int map_id, float *x_pos, float *y_pos);

#endif // MAPS_H_INCLUDED
