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
#include "objects.h"
#include "server_protocol_functions.h"
#include "e3d.h"

#define DEBUG_HARVESTING 0

void stop_harvesting(int connection){

    /** public function - see header */

    char text_out[80]="";

    clients.client[connection].harvest_flag=false;

    int object_id=clients.client[connection].harvest_object_id;

    sprintf(text_out, "%cYou stopped harvesting.", c_green3+127);
    //We ought to be able to use the following message, but the the OL/EL client relies on the above
    //phrase to stop the harvesting effect
    //sprintf(text_out, "%cyou stopped harvesting %s", c_green3+127, object[object_id].object_name);
    send_raw_text(connection, CHAT_SERVER, text_out);

    #if DEBUG_HARVESTING==1
    printf("harvesting stopped, char [%s]\n", clients.client[connection],char_name);
    #endif

    log_event(EVENT_SESSION, "harvesting stopped, character [%s]", clients.client[connection].char_name);
}

void start_harvesting(int connection, int threed_object_list_pos){

    /** public function - see header */

    char text_out[80]="";

    //get the map id
    int map_id=clients.client[connection].map_id;

    //get the id for the object being harvested
    int object_id=get_object_id(map_id, threed_object_list_pos);

    //get the position of the object
    int object_tile=get_object_tile(map_id, threed_object_list_pos);

    //get the distance between the object and the char
    int object_proximity=get_proximity(clients.client[connection].map_tile, object_tile, maps.map[map_id].map_axis);

    //determine if the object is close enough for the char to harvest
    if(object_proximity<=MIN_HARVEST_PROXIMITY){

        //check if we know what this item is
        if (object_id>0){

            //check if item is harvestable
            if(object[object_id].harvestable==true){

                sprintf(text_out, "%cyou started to harvest %s. ", c_green3+127, object[object_id].object_name);

                //set the chars harvest flag to show that it is now harvesting and set the item
                clients.client[connection].harvest_flag=true;

                //record what item the char is harvesting
                clients.client[connection].harvest_object_id=object_id;

                #if DEBUG_HARVESTING==1
                printf("harvesting started, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
                #endif

                log_text(EVENT_SESSION, "started harvesting");
            }
            else {

                sprintf(text_out, "%csorry! %s isn't harvestable.", c_red3+122, object[object_id].object_name);

                #if DEBUG_HARVESTING==1
                printf("item not harvestable, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
                #endif

                log_text(EVENT_SESSION, "attempt to harvest unharvestable object");
            }
        }
        else {

            sprintf(text_out, "%cyou tried to harvest an unknown item", c_red3+127);

            #if DEBUG_HARVESTING==1
            printf("harvest item unknown, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
            #endif

            log_event(EVENT_ERROR, "attempt to harvest unknown object");
        }
    }
    else {

        sprintf(text_out, "%cyou are too far away to harvest that item", c_red3+127);

        #if DEBUG_HARVESTING==1
        printf("harvest item too far away, char [%s], object number [%i], item_id [%i] item name [%s]\n", clients.client[connection],char_name, object_number, item_id, map_object[item_id].object_name);
        #endif

        log_event(EVENT_ERROR, "object to far away to harvest");
    }

    send_raw_text(connection, CHAT_SERVER, text_out);
}
