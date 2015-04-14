/******************************************************************************************************************
	Copyright 2014 UnoffLandz

	This file is part of unoff_server_4.

	unoff_server_4 is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	unoff_server_4 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/

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

    //prevents problems that arise where visual range attributes are zero
    if(visual_proximity<3) visual_proximity=3;

    return visual_proximity;
}


int char_in_game(char *char_name){

    /** public function - see header */

    //convert char name to upper case
    char target_name[80]="";
    strcpy(target_name, char_name);
    str_conv_upper(target_name);

    char compare_name[80]="";

    int i=0;
    for(i=0; i<MAX_CLIENTS; i++){

        if(clients.client[i].client_status==LOGGED_IN){

            //convert compare name to upper case
            strcpy(compare_name, clients.client[i].char_name);
            str_conv_upper(compare_name);

            //compare the target name and compare name
            if(strcmp(target_name, compare_name)==0) {

                return i;
            }
        }
    }

    return NOT_FOUND;
}


int char_age(int connection){

    /** public function - see header */

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
