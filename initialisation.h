#ifndef INITIALISATION_H_INCLUDED
#define INITIALISATION_H_INCLUDED

#include "global.h"

void initialise_channel_list(int max_nodes);

void initialise_message_list(int max_nodes);

void initialise_client_list(int max_nodes);

void initialise_guild_list(int max_nodes);

void initialise_character_list(int max_nodes);

void initialise_map_list(int max_nodes);

void initialise_movement_vectors();

#endif // INITIALISATION_H_INCLUDED
