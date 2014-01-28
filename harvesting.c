#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for gettimeofday function

#include "global.h"
#include "protocol.h"
#include "files.h"

void process_harvesting(int connection, time_t current_time){

    int char_id=clients.client[connection]->character_id;
    int item=clients.client[connection]->harvest_item;
    int interval=harvestables[item].interval;

    if(clients.client[connection]->harvest_flag==TRUE){

        //adjust timer to compensate for minute wrap-around>
        if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

        // check for time of next harvest
        if(current_time>clients.client[connection]->time_of_last_harvest+interval) {

            //update the time of harvest
            gettimeofday(&time_check, NULL);
            clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

            characters.character[char_id]->harvest_exp+=harvestables[item].exp;
            send_partial_stats(connection, HARVEST_EXP, characters.character[char_id]->harvest_exp);

            //save updated exp to char record
            save_character(characters.character[char_id]->char_name, char_id);
        }
    }
}
