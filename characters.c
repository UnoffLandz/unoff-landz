#include <string.h> //supports strcmp
#include <stdio.h> // supports sprintf

#include "clients.h"
#include "character_race.h"
#include "character_type.h"
#include "game_data.h"
#include "attributes.h"
#include "global.h"
#include "logging.h"
#include "string_functions.h"
#include "server_start_stop.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "characters.h"

int get_char_visual_range(int connection){

    /** public function - see header */

    int race_id=get_char_race_id(connection);
    int visual_proximity=0;

    if(game_data.game_minutes<180){

        visual_proximity = attribute[race_id].day_vision[clients.client[connection].vitality_pp];
    }
    else {

        visual_proximity = attribute[race_id].night_vision[clients.client[connection].instinct_pp];
    }

    return visual_proximity;
}


int char_in_game(char *char_name){

    /** public function - see header */

    int i;

    for(i=0; i<MAX_CLIENTS; i++){

        if(strcmp(clients.client[i].char_name, char_name)==0){

            if(clients.client[i].client_status==LOGGED_IN) return i;
        }
    }

    return NOT_FOUND;
}


int char_age(int connection){

    int age=(clients.client[connection].time_of_last_minute - clients.client[connection].char_created) / (60*60*24);

    return age;
}

/*
int rename_char(int connection, char *new_char_name){

    //check that no existing char has the new name
    if(get_char_data_from_db(new_char_name)!=NOT_FOUND) return CHAR_RENAME_FAILED_DUPLICATE;

    //update char name and save
    strcpy(clients.client[connection].char_name, new_char_name);
    update_db_char_name(connection);

    // add this char to each connected client
    broadcast_add_new_enhanced_actor_packet(connection);

    return CHAR_RENAME_SUCCESS;
}
*/
