#ifndef PATHINDING_H_INCLUDED
#define PATHINDING_H_INCLUDED

enum{ //return values for public function get_astar_path (pathfinding.c)
    ASTAR_UNREACHABLE,
    ASTAR_REACHABLE,
    ASTAR_ABORT,
    ASTAR_UNKNOWN
};

enum{//tile bounds returned from check_tile_bounds function
    TILE_OUTSIDE_BOUNDS=-2,
    TILE_NON_TRAVERSABLE=-1,
    TILE_TRAVERSABLE=0
};

enum {// return values for is_map_tile_occupied
    TILE_OCCUPIED,
    TILE_UNOCCUPIED
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

#endif // PATHINDING_H_INCLUDED
