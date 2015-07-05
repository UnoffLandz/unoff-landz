/******************************************************************************************************************
	Copyright 2014, 2015 UnoffLandz

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

#include <stdio.h> //support for sprintf

#include "clients.h"
#include "colour.h"
#include "server_messaging.h"
#include "logging.h"
#include "maps.h"
#include "harvesting.h"
#include "map_objects.h"
#include "server_protocol_functions.h"

#define DEBUG_HARVESTING 0

void stop_harvesting(int connection){

    char text_out[80]="";

    clients.client[connection].harvest_flag=HARVESTING_OFF;

    sprintf(text_out, "%cyou stopped harvesting", c_green3+127);
    send_raw_text(connection, CHAT_SERVER, text_out);

    #if DEBUG_HARVESTING==1
    printf("harvesting stopped, char [%s]\n", clients.client[connection],char_name);
    #endif

    log_event(EVENT_SESSION, "harvesting stopped, character [%s]", clients.client[connection].char_name);
}

void start_harvesting(int connection, int map_object_number){

    char text_out[80]="";

    //get the item_id for the object
    int map_id=clients.client[connection].map_id;
    int item_id=maps.map[map_id].threed_object_lookup[map_object_number].item_id;

    //get the position of the object
    int object_tile=get_tile(
        maps.map[map_id].threed_object_lookup[map_object_number].x,
        maps.map[map_id].threed_object_lookup[map_object_number].y,
        clients.client[connection].map_id);

    //get the distance between the object and the char
    int object_proximity=get_proximity(clients.client[connection].map_tile, object_tile, maps.map[map_id].map_axis);

    //determine if the object is close enough for the char to harvest
    if(object_proximity<=MIN_HARVEST_PROXIMITY){

        //check if we know what this item is
        if (item_id>0){

            //check if item is harvestable
            if(map_object[item_id].harvestable==HARVESTABLE){

                sprintf(text_out, "%cyou started to harvest %s. ", c_green3+127, map_object[item_id].object_name);

                clients.client[connection].harvest_flag=HARVESTING_ON;

                #if DEBUG_HARVESTING==1
                printf("harvesting started, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
                #endif

                log_event(EVENT_SESSION, "harvesting started, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection].char_name, map_object_number, item_id, map_object[item_id].object_name);
            }
            else {

                sprintf(text_out, "%cSorry! %s isn't harvestable.", c_red3+122, map_object[item_id].object_name);

                #if DEBUG_HARVESTING==1
                printf("item not harvestable, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
                #endif

                log_event(EVENT_SESSION, "protocol HARVEST - item not harvestable, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection].char_name, map_object_number, item_id, map_object[item_id].object_name);
            }
        }
        else {

            sprintf(text_out, "%cyou tried to harvest an unknown item", c_red3+127);

            #if DEBUG_HARVESTING==1
            printf("harvest item unknown, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
            #endif

            log_event(EVENT_ERROR, "harvest item unknown, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection].char_name, map_object_number, item_id, map_object[item_id].object_name);
        }
    }
    else {

        sprintf(text_out, "%cyou are too far away to harvest that item", c_red3+127);

        #if DEBUG_HARVESTING==1
        printf("harvest item too far away, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
        #endif

        log_event(EVENT_ERROR, "harvest item too far away, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection].char_name, map_object_number, item_id, map_object[item_id].object_name);
    }

    send_raw_text(connection, CHAT_SERVER, text_out);
}
