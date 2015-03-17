#ifndef MOVEMENT_H_INCLUDED
#define MOVEMENT_H_INCLUDED

#define MAX_UNOCCUPIED_TILE_SEARCH 10

struct vector_type{
    int x;
    int y;
    unsigned char move_cmd;
};
struct vector_type vector[8];


/** RESULT  : loads movement vector array with values

    RETURNS : void

    PURPOSE : initialise the vector array

*/
void initialise_movement_vectors();


/** RESULT  : finds nearest unoccupied tile

    RETURNS : nearest unoccupied tile or 0 if there is no unoccupied tiles within MAX_UNOCCUPIED_TILE_SEARCH

    PURPOSE : To support map jumps

*/
int get_nearest_unoccupied_tile(int map_id, int map_tile);

#endif // MOVEMENT_H_INCLUDED
