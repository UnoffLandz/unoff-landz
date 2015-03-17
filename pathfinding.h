#ifndef PATHFINDING_H_INCLUDED
#define PATHFINDING_H_INCLUDED

#define PATH_STACK_MAX 25 //maximum size of the stack that is used to determine the path

enum{ //nodes for explore/path arrays
    TILE,
    VALUE,
    STATUS
};

enum{ // values for explore_stack/path_stack STATUS (used in functions explore_path and get_astar_path)
    UNEXPLORED,
    EXPLORED
};

enum{ //return values for functions add_tile_to_explore_stack and add_adjacent_tiles_to_explore_stack
    ADD_TILE_ABORT,
    ADD_TILE_COMPLETE
};

/** RESULT  : fills client path array with an a-star path

    RETURNS : ASTAR_ABORT       - the explore stack was exceeded
              ASTAR_UNREACHABLE - path to destination could not be found
              ASTAR_REACHABLE   - path to destination was found
              ASTAR_UNKNOWN     - explore_path result unknown

    PURPOSE : to provide a list of explored tiles from which a path can be determined

    USAGE   : pathfinding.c get_astar_path
*/
int get_astar_path(int connection, int start_tile, int destination_tile);


#endif // PATHFINDING_H_INCLUDED
