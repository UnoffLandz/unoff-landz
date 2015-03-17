#ifndef GAME_DATA_H_INCLUDED
#define GAME_DATA_H_INCLUDED

#include <sys/time.h>   // supports time_t data type

struct game_data_type {

    time_t server_start_time;
    char name_last_char_created[80];
    time_t date_last_char_created;
    int char_count;
    int game_minutes;
    int beam_map_id;
    int beam_map_tile;
    int start_map_id;
    int start_map_tile;
};
struct game_data_type game_data;

#endif // GAME_DATA_H_INCLUDED
