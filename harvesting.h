#ifndef HARVESTING_H_INCLUDED
#define HARVESTING_H_INCLUDED

void start_harvesting(int connection, int map_object_id);

void process_harvesting(int connection, time_t current_time);

void stop_harvesting(int connection);

#endif // HARVESTING_H_INCLUDED
