#ifndef HARVESTING_H_INCLUDED
#define HARVESTING_H_INCLUDED

//void start_harvesting(int connection, int map_object_id);

void start_harvesting2(int connection, int map_object_id, struct ev_loop *loop);

//void process_harvesting(int connection, time_t current_time);

//void stop_harvesting(int connection);

void stop_harvesting2(int connection, struct ev_loop *loop);

#endif // HARVESTING_H_INCLUDED
