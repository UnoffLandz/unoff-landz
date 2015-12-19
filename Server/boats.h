#ifndef BOATS_H_INCLUDED
#define BOATS_H_INCLUDED

struct boat_schedule_type{

    int departure_map_id;
    int departure_map_tile;

    int destination_map_id;
    int destination_map_tile;

    int departure_time;
    int arrival_time;

    char arrival_message[1024];
    char departure_message[1024];

    int boat_map_id;
    int boat_map_tile;
};
extern struct boat_schedule_type boat_schedule[10];

#endif // BOATS_H_INCLUDED
