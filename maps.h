#ifndef MAPS_H_INCLUDED
#define MAPS_H_INCLUDED

#define ELM_FILE_HEADER_LEN 124

enum { //return values from add_char_to_map / remove_char_from_map
    LEGAL_MAP=0,
    ILLEGAL_MAP=-1
};

struct map_object_type{
    char e3d_filename[80];
    float x;
    float y;
    float z;
    int image_id;
};
struct map_object_type map_object;

void add_client_to_map_list(int client_id, int map_id);

void remove_client_from_map_list(int client_id, int map_id);

int get_map_object(int object_id, int map_id);

#endif // MAPS_H_INCLUDED
