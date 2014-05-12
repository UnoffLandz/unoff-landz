#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h>   //needed for gettimeofday function
#include <string.h>
#include <ev.h>

#include "global.h"
#include "protocol.h"
#include "files.h"
#include "numeric_functions.h"
#include "database.h"
#include "harvesting.h"
#include "chat.h"
#include "broadcast.h"
#include "character_inventory.h"
#include "maps.h"

static void harvest_cycle_cb (struct ev_loop *loop, struct ev_timer *ev_harvest_timer, int revents) {

    (void)(revents);//removes unused parameter warning
    (void)(loop);//removes unused parameter warning

    int connection=(int) ev_harvest_timer->data;

    char text_out[1024]="";
    int image_id=clients.client[connection].inventory_image_id;
    int slot=clients.client[connection].inventory_slot;
    int amount=clients.client[connection].harvest_amount;

    // if inventory is overloaded stop harvesting
    if(get_inventory_emu(connection)+ (item[image_id].emu * amount) > get_max_inventory_emu(connection)){

        //inform the client
        sprintf(text_out, "%cYou are overloaded", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        stop_harvesting2(connection, loop);
        return;
    }

    //send updated exp to client
    clients.client[connection].harvest_exp+=(item[image_id].exp * amount);
    send_partial_stats(connection, HARVEST_EXP,  clients.client[connection].harvest_exp);

    #ifdef DEBUG
    printf("harvesting Item %i name %s EMU %i\n", image_id, item[image_id].item_name, item[image_id].emu);
    #endif

    printf("image id %i emu %i\n", image_id, item[image_id].emu);

    //send updated inventory_emu to client
    send_partial_stats(connection, INVENTORY_EMU,  get_inventory_emu(connection));

    //update exp and inventory stats on database
    update_db_char_stats(connection);

    //Add the harvested amount to the inventory and send to client
    clients.client[connection].client_inventory[slot].amount+= amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection].client_inventory[slot].amount, slot);

    //update char inventory on database
    update_db_char_slot(connection, slot);
}

void stop_harvesting2(int connection, struct ev_loop *loop){

    char text_out[1024]="";

    //unless we send this text to the client, the harvesting effect won't stop
    sprintf(text_out, "%cYou stopped harvesting. %s", c_red3+127, item[clients.client[connection].inventory_image_id].item_name);
    send_server_text(connection, CHAT_SERVER, text_out);

    //cancel further harvesting cycles and exit
    clients.client[connection].harvest_flag=FALSE;

    ev_timer_stop(loop, &ev_harvest_timer[connection]);
}
/*
void stop_harvesting(int connection){

    char text_out[1024]="";

    //unless we send this text to the client, the harvesting effect won't stop
    sprintf(text_out, "%cYou stopped harvesting. %s", c_red3+127, item[clients.client[connection]->inventory_image_id].item_name);
    send_server_text(connection, CHAT_SERVER, text_out);

    //cancel further harvesting cycles and exit
    clients.client[connection]->harvest_flag=FALSE;
}
*/
/*
void start_harvesting(int connection, int map_object_id){

    int slot=0;
    char text_out[80]="";
    int map_id=clients.client[connection]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int char_tile=clients.client[connection]->map_tile;

    //if already harvesting then stop
    if(clients.client[connection]->harvest_flag==TRUE) {

        //stop_harvesting(connection);
        return;
    }

    //find the inventory image id for the map object
    if(get_map_object(map_object_id, map_id)==NOT_FOUND){

        log_event(EVENT_ERROR, "inventory image id not found in function start_harvesting: module harvesting.c");

        sprintf(text_out, "%cSorry. You can't harvest this item.", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        clients.client[connection]->harvest_flag=FALSE;
        return;
    }

    if(get_proximity(char_tile, map_object.tile_pos, map_axis) > get_char_visual_range(connection)) {

        #ifdef DEBUG
        printf("distance [%i] visual range[%i]\n", get_proximity(char_tile, map_object.tile_pos, map_axis), get_char_visual_range(connection));
        #endif

        //abort harvesting if item is too far away
        sprintf(text_out, "%cYou are too far away to harvest that item", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return;
    }

    //record the image id of the item being harvested in the client array so we know what to harvest on each subsequent
    //harvest cycle without having to continually look it up
    clients.client[connection]->inventory_image_id=map_object.image_id;

    add_item_to_inventory(connection, map_object.image_id, 0, &slot);

    //note the slot so we don't have to parse the whole inventory on each harvest cycle
    clients.client[connection]->inventory_slot=slot;

    #ifdef DEBUG
    printf("using slot [%i] for item [%s]\n", slot, item[map_object.image_id].item_name);
    #endif

    //set the harvest flag so that harvesting will be continuous
    clients.client[connection]->harvest_flag=TRUE;

    //send message to client
    sprintf(text_out, "%cYou started to harvest %s", c_green3+127, item[clients.client[connection]->inventory_image_id].item_name);
    send_server_text(connection, CHAT_SERVER, text_out);
}
*/

void start_harvesting2(int connection, int map_object_id, struct ev_loop *loop){

    int slot=0;
    char text_out[80]="";
    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int char_tile=clients.client[connection].map_tile;
    int interval=0;

    //if already harvesting then stop
    if(clients.client[connection].harvest_flag==TRUE) {

        ev_timer_start(loop, &ev_harvest_timer[connection]);
        stop_harvesting2(connection, loop);
        return;
    }

    //find the inventory image id for the map object
    if(get_map_object(map_object_id, map_id)==NOT_FOUND){

        log_event(EVENT_ERROR, "inventory image id not found in function start_harvesting: module harvesting.c");

        sprintf(text_out, "%cSorry. You can't harvest this item.", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        clients.client[connection].harvest_flag=FALSE;
        return;
    }

    if(get_proximity(char_tile, map_object.tile_pos, map_axis) > get_char_visual_range(connection)) {

        #ifdef DEBUG
        printf("distance [%i] visual range[%i]\n", get_proximity(char_tile, map_object.tile_pos, map_axis), get_char_visual_range(connection));
        #endif

        //abort harvesting if item is too far away
        sprintf(text_out, "%cYou are too far away to harvest that item", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return;
    }

    add_item_to_inventory(connection, map_object.image_id, 0, &slot);

    //note the slot so we don't have to parse the whole inventory on each harvest cycle
    clients.client[connection].inventory_image_id=map_object.image_id;
    clients.client[connection].inventory_slot=slot;
    clients.client[connection].harvest_flag=TRUE;
    clients.client[connection].harvest_amount=1; /**change this factor for perks/cloaks and special days **/

    #ifdef DEBUG
    printf("using slot [%i] for item [%s]\n", slot, item[map_object.image_id].item_name);
    #endif

    //send message to client
    sprintf(text_out, "%cYou started to harvest %s", c_green3+127, item[clients.client[connection].inventory_image_id].item_name);
    send_server_text(connection, CHAT_SERVER, text_out);

    interval=item[map_object.image_id].interval;

    ev_harvest_timer[connection].data=(int*) connection;
    ev_timer_init(&ev_harvest_timer[connection], harvest_cycle_cb, interval, interval);
    ev_timer_start(loop, &ev_harvest_timer[connection]);
}

/*
void process_harvesting(int connection, time_t current_time){

    char text_out[1024]="";
    int image_id=clients.client[connection]->inventory_image_id;
    int slot=clients.client[connection]->inventory_slot;

    // exit if there's nothing to be harvested
    if(clients.client[connection]->harvest_flag==FALSE) return;

    //adjust timer to compensate for minute wrap-around>
    if(clients.client[connection]->time_of_last_harvest>current_time) current_time+=60;

    // exit if the harvest interval hasn't expired
    if(current_time<clients.client[connection]->time_of_last_harvest+item[image_id].interval) return;

    //update the time of harvest
    gettimeofday(&time_check, NULL);
    clients.client[connection]->time_of_last_harvest=time_check.tv_sec;

    // if inventory is overloaded stop harvesting
    if(get_inventory_emu(connection)+ (item[image_id].emu * item[image_id].cycle_amount) > get_char_carry_capacity(connection)){

        //inform the client
        sprintf(text_out, "%cYou are overloaded", c_red1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        //stop_harvesting(connection);
        return;
    }

    //send updated exp to client
    clients.client[connection]->harvest_exp+=item[image_id].exp;
    send_partial_stats(connection, HARVEST_EXP,  clients.client[connection]->harvest_exp);

    #ifdef DEBUG
    printf("harvesting Item %i name %s EMU %i\n", image_id, item[image_id].item_name, item[image_id].emu);
    #endif

    //send updated inventory_emu to client
    clients.client[connection]->inventory_emu += item[image_id].cycle_amount * item[image_id].emu;
    send_partial_stats(connection, INVENTORY_EMU,  clients.client[connection]->inventory_emu);

    //update exp and inventory stats on database
    update_db_char_stats(connection);

    //Add the harvested amount to the inventory and send to client
    clients.client[connection]->client_inventory[slot].amount+= item[image_id].cycle_amount;
    send_get_new_inventory_item(connection, image_id, clients.client[connection]->client_inventory[slot].amount, slot);

    //update char inventory on database
    update_db_char_slot(connection, slot);
}
*/
