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

#include <stdio.h>  //support for sprintf
#include <string.h> //support for memcpy strlen strcpy
#include <stdint.h> //support for int16_t data type
#include <stdlib.h> // testing only
#include <byteswap.h> // support for __bswap32 function

#include "client_protocol.h"
#include "clients.h"
#include "logging.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "characters.h"
#include "game_data.h"
#include "log_in.h"
#include "map_object.h"
#include "character_movement.h"
#include "character_type.h"
#include "broadcast_chat.h"
#include "broadcast_movement.h"
#include "chat.h"
#include "hash_commands.h"
#include "server_start_stop.h"
#include "idle_buffer2.h"
#include "season.h"
#include "e3d.h"
#include "objects.h"
#include "harvesting.h"
#include "date_time_functions.h"
#include "client_protocol_handler.h"
#include "bags.h"
#include "packet.h"
#include "npc.h"
#include "character_inventory.h"
#include "boats.h"

#define DEBUG_CLIENT_PROTOCOL_HANDLER 1
// TODO (themuntdregger#1#): Finish PING_RESPONSE handling
// TODO (themuntdregger#1#): establish what SEND_ME_MY_ACTORS protocol does
// TODO (themuntdregger#1#): establish what SEND_OPENING_SCREEN does


int client_server_stats(int actor_node, unsigned char *packet){

    (void)(packet);//removes unused parameter warning

    int socket=clients.client[actor_node].socket;

    send_motd_header(socket);

    return 0;
}


int client_get_time(int actor_node, unsigned char *packet){

    (void)(packet);//removes unused parameter warning

    int socket=clients.client[actor_node].socket;

    send_verbose_time(socket, game_data.game_minutes);

    return 0;
}


int client_get_date(int actor_node, unsigned char *packet){

    (void)(packet);//removes unused parameter warning

    int socket=clients.client[actor_node].socket;

    //calculate day of year
    int day_of_year=game_data.game_days % game_data.year_length;
    send_verbose_date(socket, day_of_year);

    return 0;
}


int client_create_char(int actor_node, unsigned char *packet){

    size_t packet_len=get_packet_length(packet);
    push_idle_buffer2(actor_node, IDLE_BUFFER_PROCESS_CHECK_NEWCHAR, packet, packet_len);

    return 0;
}


int client_set_active_channel(int actor_node, unsigned char *packet){

    //set the active channel (chans run in reverse from 32(chan 1) - 0(chan 33).
    clients.client[actor_node].active_chan=packet[3]-32;

    //update the database
    push_sql_command("UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=%i WHERE CHAR_ID=%i", clients.client[actor_node].active_chan, clients.client[actor_node].character_id);

    return 0;
}


int client_respond_to_npc(int actor_node, unsigned char *packet){

    int npc_actor_node=*((int16_t*)(packet+3));
    int response=packet[5];

    if(clients.client[npc_actor_node].player_type==NPC){

        //turn off response timer
        clients.client[actor_node].npc_choice=false;

        for(int i=0; i<MAX_NPC_TRIGGERS; i++){

            if(npc_trigger[i].actor_node==npc_actor_node
            && npc_trigger[i].trigger_type==SELECT_OPTION){

                int action_node=npc_trigger[i].action_node;

                if(npc_action[action_node].action_type==SELL_OBJECT){

                    npc_sell_object(actor_node, npc_actor_node, response);
/*
                    //clear the options list
                    send_npc_options_list(socket, npc_actor_node, "");

                    //get cost of object to be purchased
                    int referring_action_node=clients.client[actor_node].action_node;
                    int object_amount_required=clients.client[actor_node].npc_option[response].amount;
                    int object_id_required=npc_action[referring_action_node].object_id_required;

                    int item_required_slot=item_in_inventory(actor_node, object_id_required);

                    //check actor has payment for the object to be purchased
                    if(item_required_slot==-1 || clients.client[actor_node].inventory[item_required_slot].amount < object_amount_required){


                        sprintf(text_out, "Sorry %s.\n\nYou don't have enough %s to buy that amount.", clients.client[actor_node].char_name, object[object_id_required].object_name);
                        send_npc_text(socket, text_out);

                        //ensure choice timeout is turned off
                        clients.client[actor_node].npc_choice=false;
                        return 0;
                    }

                    //check actor has room in inventory for the object to be purchased
                    int object_amount_given=clients.client[actor_node].npc_option[response].price;
                    int object_id_given=npc_action[referring_action_node].object_id_given;
                    int item_given_slot=find_inventory_slot(actor_node, object_id_given);

                    //check actor has room for payment item
                    if(item_given_slot==-1){

                        sprintf(text_out, "Sorry %s.\n\nYou don't have room for....", clients.client[actor_node].char_name);
                        send_npc_text(socket, text_out);

                        //ensure choice timeout is turned off
                        clients.client[actor_node].npc_choice=false;
                        return 0;
                    }

                    //add purchased object to and remove payment from the actors inventory
                    add_to_inventory(actor_node, object_id_given, object_amount_given, item_given_slot);
                    remove_from_inventory(actor_node, object_id_required, object_amount_required, item_required_slot);
                    sprintf(text_out, "Thanks %s.\n\nYou've just bought %i %s", clients.client[actor_node].char_name, object_amount_given, object[object_id_given].object_name);
                    send_npc_text(socket, text_out);

                    //ensure choice timeout is turned off
                    clients.client[actor_node].npc_choice=false;
                    return 0;
*/
                }
                else if(npc_action[action_node].action_type==SELL_BOAT_TICKET){

                    npc_sell_boat_ticket(actor_node, npc_actor_node, response);
                }
                else if(npc_action[action_node].action_type==GIVE_BOAT_SCHEDULE){

                    npc_give_boat_options(actor_node, npc_actor_node, action_node);
                    return 0;
                }
                else {

                    log_event(EVENT_ERROR, "unknown npc action type [%i] in function %s: module %s: line %i", npc_action[action_node].action_type, __func__, __FILE__, __LINE__);
                }
            }
        }
    }

    return 0;
}


int client_touch_player(int actor_node, unsigned char *packet){

    //char text_out[160]="";
    int touched_actor_node=*((int32_t*)(packet+3));
    int socket=clients.client[actor_node].socket;

    //check proximity
    if(get_proximity(clients.client[actor_node].map_tile, clients.client[touched_actor_node].map_tile, clients.client[actor_node].map_id)>ACTOR_CONTACT_PROXIMITY){

        send_text(socket, CHAT_SERVER, "%cYou are too far away to touch %s!", c_red3+127, clients.client[touched_actor_node].char_name);
        return 0;
    }

    if(clients.client[touched_actor_node].player_type==NPC){


        clients.client[actor_node].npc_node=touched_actor_node;

        for(int i=0; i<MAX_NPC_TRIGGERS; i++){

            if(npc_trigger[i].actor_node==touched_actor_node
            && npc_trigger[i].trigger_type==TOUCHED){

                int action_node=npc_trigger[i].action_node;

                if(npc_action[action_node].action_type==GIVE_BOAT_SCHEDULE){

                    npc_give_boat_options(actor_node, touched_actor_node, action_node);
                }
                else if(npc_action[action_node].action_type==GIVE_SALE_OPTIONS){

                    npc_give_sale_options(actor_node, touched_actor_node, action_node);
                }
            }
        }
    }

    return 0;
}


int client_look_at_map_object(int actor_node, unsigned char *packet){

    char text_out[80]="";
    int threed_object_list_pos=*((int32_t*)(packet+3));

    int map_id=clients.client[actor_node].map_id;
    int map_object_tile=map_object[threed_object_list_pos][map_id].tile;

    if(get_proximity(clients.client[actor_node].map_tile, map_object_tile, map_id)>ACTOR_CONTACT_PROXIMITY){

        send_text(clients.client[actor_node].socket, CHAT_SERVER, "%cYou are too far away to see the object!", c_red3+127);
        return 0;
    }

    //get the object item id
    int object_id=get_object_id(clients.client[actor_node].map_id, threed_object_list_pos);

    //tell the client what the item is
    if (object_id>0){

        sprintf(text_out, "%cyou see a %s. ", c_green3+127, object[object_id].object_name);

        if(object[object_id].harvestable==true){

            sprintf(text_out, "%sIt's harvestable", text_out);
        }

        if(object[object_id].edible==true){

            sprintf(text_out, "%s and it's edible", text_out);
        }

        //add a period to the end of the sentence
        sprintf(text_out, "%s.", text_out);
    }
    else {

        sprintf(text_out, "%cyou see an unknown item", c_green3+127);
    }

    send_text(clients.client[actor_node].socket, CHAT_SERVER, text_out);

    return 0;
}


int client_inspect_bag(int actor_node, unsigned char *packet){

    int bag_id=packet[3];

    //check if char is standing on bag
    for(int i=0; i<MAX_BAGS; i++){

       //if char is standing on bag then open it
       if(bag[i].tile==clients.client[actor_node].map_tile){

            send_here_your_ground_items(clients.client[actor_node].socket, bag_id);

            clients.client[actor_node].bag_open=true;
            clients.client[actor_node].open_bag_id=i;
            return 0;
       }
    }

    //if char is not standing on bag then walk towards it
    start_char_move(actor_node, bag[bag_id].tile);

    return 0;
}


int client_pick_up_item(int actor_node, unsigned char *packet){

    int bag_slot=packet[3];
    int amount=*((int32_t*)(packet+4));
    int bag_id=clients.client[actor_node].open_bag_id;

    pick_up_from_bag_to_inventory(actor_node, bag_slot, amount, bag_id);

    return 0;
}


int client_drop_item(int actor_node, unsigned char *packet){

    int inventory_slot=packet[3];
    int withdraw_amount=*((int32_t*)(packet+4));

    drop_from_inventory_to_bag(actor_node, inventory_slot, withdraw_amount);

    return 0;
}


int client_harvest(int actor_node, unsigned char *packet){

    int threed_object_list_pos=*((int16_t*)(packet+3));

    log_event(EVENT_SESSION, "HARVEST - started, char [%s], threed object list position [%i], map [%i]", clients.client[actor_node].char_name, threed_object_list_pos, clients.client[actor_node].map_id);

    //if char is already harvesting then stop, else start harvesting
    if(clients.client[actor_node].harvest_flag==true){

        stop_harvesting(actor_node);
    }
    else {

        start_harvesting(actor_node, threed_object_list_pos);
    }

    return 0;
}


int client_move_inventory_item(int actor_node, unsigned char *packet){

    int from_slot=packet[3];
    int to_slot=packet[4];

    move_inventory_item(actor_node, from_slot, to_slot);

    return 0;
}


int client_look_at_inventory_item(int actor_node, unsigned char *packet){

    int slot=packet[3];
    int item=clients.client[actor_node].inventory[slot].object_id;
    int socket=clients.client[actor_node].socket;

    if(object[item].edible==true){

        char text_out[SEND_TEXT_MAX]="";

        sprintf(text_out, "%cYou see a %s. It's edible!", c_green3+127, object[item].object_name);
        send_inventory_item_text(socket, text_out);
    }
    else {

        char text_out[SEND_TEXT_MAX]="";

        sprintf(text_out, "%cYou see a %s.", c_green3+127, object[item].object_name);
        send_inventory_item_text(socket, text_out);
    }

    return 0;
}


int client_use_object(int actor_node, unsigned char *packet){

    int threed_object_id=*((int32_t*)(packet+3));
    int use_with_position=*((int32_t*)(packet+7));

    //if char is moving when protocol arrives, cancel rest of path
    clients.client[actor_node].path_count=0;

    printf("threed object %i use with position %i\n", threed_object_id, use_with_position);

    return 0;
}


int client_heart_beat(int actor_node, unsigned char *packet){

    (void)(actor_node);//removes unused parameter warning
    (void)(packet);

    //dummy function. No need to do anything with the command as any data receipt
    //updates the heartbeat

    return 0;
}


int client_get_player_info(int actor_node, unsigned char *packet){

    //extract integer from packet
    int other_actor_node=*((int32_t*)(packet+3));

    int socket=clients.client[actor_node].socket;

    send_text(socket, CHAT_SERVER, "You see %s", clients.client[other_actor_node].char_name);

    return 0;
}


int client_send_pm(int actor_node, unsigned char *packet){

    //extract char string from packet
    char text[1024]="";
    memcpy(text, packet+3, (size_t)get_packet_length(packet)-3);

    //extract target name and message from pm packet
    char char_name[80]="";
    char msg[1024]="";
    sscanf(text, "%s %[^\n]", char_name, msg);

    //send the message
    send_pm(actor_node, char_name, msg);

    return 0;
}


int client_move_to(int actor_node, unsigned char *packet){

    //get destination tile
    int x_dest=*((int16_t*)(packet+3));
    int y_dest=*((int16_t*)(packet+5));

    int tile_dest=get_tile(x_dest, y_dest, clients.client[actor_node].map_id);

    //move the char
    start_char_move(actor_node, tile_dest);

    log_event(EVENT_SESSION, "Move [%s] Map [%s] x[%i] y[%i] tile [%i]",
        clients.client[actor_node].char_name,
        maps.map[clients.client[actor_node].map_id].map_name,
        x_dest, y_dest, tile_dest);

    return 0;
}


int client_send_version(int actor_node, unsigned char *packet){

    //extract the data from the packet
    client_socket[actor_node].client_version_first_digit=*((int16_t*)(packet+3));
    client_socket[actor_node].client_version_second_digit=*((int16_t*)(packet+5));
    client_socket[actor_node].client_version_major=packet[7];
    client_socket[actor_node].client_version_minor=packet[8];
    client_socket[actor_node].client_version_release=packet[9];
    client_socket[actor_node].client_version_patch=packet[10];
    memcpy(client_socket[actor_node].client_version_ip_address, packet+11, 4);
    client_socket[actor_node].client_version_port=__bswap_16(*((int16_t*)(packet+15))); //bytes are in network order which is big endian, so convert to little endian

    //log the data
    log_text(EVENT_SESSION, "first digit [%i] second digit [%i] major [%i] minor [%i] release [%i] patch [%i] ip address [%i.%i.%i.%i] port [%i]",
    client_socket[actor_node].client_version_first_digit,
    client_socket[actor_node].client_version_second_digit,
    client_socket[actor_node].client_version_major,
    client_socket[actor_node].client_version_minor,
    client_socket[actor_node].client_version_release,
    client_socket[actor_node].client_version_patch,
    client_socket[actor_node].client_version_ip_address[0],
    client_socket[actor_node].client_version_ip_address[1],
    client_socket[actor_node].client_version_ip_address[2],
    client_socket[actor_node].client_version_ip_address[3],
    client_socket[actor_node].client_version_port);

    if(get_packet_length(packet)==OL_SEND_VERSION_LEN){

        //get OL extra data
        client_socket[actor_node].client_version_os_type=packet[16];
        //byte 17 is unused
        client_socket[actor_node].client_version_ol_ver_flag=*((uint16_t*)(packet+18));

        //log the data
        log_text(EVENT_SESSION, "additional OL version data: OL o/s version [%i] OL unused byte [%i]",
        client_socket[actor_node].client_version_os_type,
        (int)client_socket[actor_node].client_version_ol_ver_flag);
    }

    return 0;
}


int client_raw_text(int actor_node, unsigned char *packet){

    int socket=clients.client[actor_node].socket;

    //extract text string from packet
    char text[1024]="";
    memcpy(text, packet+3, (size_t)get_packet_length(packet)-3);

    // trim off excess left hand space
    str_trim_left(text);

    //channel chat
    if(text[0]=='@'){

        //remove the @ from text string and add null terminator
        memcpy(text, text+1, strlen(text)-1);
        text[strlen(text)-1]='\0';

        // check if char has an active channel
        if(chat_chan_open(actor_node)==false){

            send_text(socket, CHAT_SERVER, "%cyou have not joined a channel yet", c_red3+127);
            return 0;
        }

        //0 1 2
        int active_chan_slot=clients.client[actor_node].active_chan;

        //channel slots run from zero. Hence, we need to subtract 1 from the active_chan slot value
        int chan=clients.client[actor_node].chan[active_chan_slot];

        //broadcast text
        broadcast_channel_chat(chan, actor_node, text);
    }

    //guild chat
    else if(text[0]=='~'){

        //need to convert to a hash command and pass through the process_hash_command function
        //otherwise non-guild chars will be able to use command

        //replace [~] with [#gm] then process as hash command
        memcpy(text, text+1, strlen(text)-1);
        text[strlen(text)-1]='\0';

        char text_out[1024]="#GM ";
        strcat(text_out, text);
        strcpy(text, text_out);

        process_hash_commands(actor_node, text_out);
    }

    //hash commands
    else if(text[0]=='#'){

        process_hash_commands(actor_node, text);
    }

    //local chat
    else {

        //broadcast text
        broadcast_local_chat(actor_node, text);
    }

    return 0;
}

int client_sit_down(int actor_node, unsigned char *packet){

    // the protocol recognises two sets of sit stand command. The first is implemented via the actor command set
    //which is used in the ADD_ACTOR packet; the second is implemented via the frame set which is used in the
    //ADD_ENHANCED_ACTOR packet. When using the ADD_ACTOR packet, command 13=sit down and command 14=stand up. When
    //using the ADD_ENHANCED_ACTOR packet, command 12=sit, command 13=stand and command 14=stand idle.

    //The client automatically issues a SIT_DOWN packet after a few minutes inactivity.
    //even if the char has connected but not yet logged in. We therefore need to exclude
    //processing under these conditions otherwise it results in a seg fault.
    if(client_socket[actor_node].socket_node_status==CLIENT_CONNECTED){

        log_event(EVENT_SESSION, "Ignore SIT_DOWN command whilst client is connected but not yet logged in");
        return 0;
    }

    //command returns 1 data element carrying a value that indicates if the char has sit or stood
    switch(packet[3]){

        case STAND:{

            clients.client[actor_node].frame=frame_stand;
            broadcast_actor_packet(actor_node, actor_cmd_stand_up, clients.client[actor_node].map_tile);
            break;
        }

        case SIT:{

            clients.client[actor_node].frame=frame_sit;
            broadcast_actor_packet(actor_node, actor_cmd_sit_down, clients.client[actor_node].map_tile);
            break;
        }

        default:{

            log_event(EVENT_ERROR, "unknown sit/stand value [%i] in function %s: module %s: line %i", packet[3], __func__, __FILE__, __LINE__);
            stop_server();
        }
    }

    //update database
    push_sql_command("UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i",clients.client[actor_node].frame, clients.client[actor_node].character_id);

    return 0;
}

int client_log_in( int actor_node, unsigned char *packet){

    //push processing of the log in onto the idle buffer
    size_t packet_len=get_packet_length(packet);
    push_idle_buffer2(actor_node, IDLE_BUFFER_PROCESS_LOGIN, packet, packet_len);

    return 0;
}

typedef int (*protocol_function)(int actor_node, unsigned char *packet);

struct protocol_array_entry {

    int command;
    bool process_when_connected;
    bool process_when_logged_in;
    protocol_function fn;
};


struct protocol_array_entry protocol_entries[] = {

//                              Process When Process When
//   Client-Server Protocol     Connected    Logged-in     callback function
//   -------------------------- ------ -------- ---------- -----------------
    {RAW_TEXT,                  true,        true,         client_raw_text},
    {MOVE_TO,                   false,       true,         client_move_to},
    {SEND_PM,                   false,       true,         client_send_pm},
    {SIT_DOWN,                  false,       true,         client_sit_down},
    {GET_PLAYER_INFO,           false,       true,         client_get_player_info},
    {SEND_VERSION,              true,        true,         client_send_version},
    {HEARTBEAT,                 true,        true,         client_heart_beat},
    {USE_OBJECT,                false,       true,         client_use_object},
    {MOVE_INVENTORY_ITEM,       false,       true,         client_move_inventory_item},
    {HARVEST,                   false,       true,         client_harvest},
    {DROP_ITEM,                 false,       true,         client_drop_item},
    {PICK_UP_ITEM,              false,       true,         client_pick_up_item},
    {INSPECT_BAG,               false,       true,         client_inspect_bag},
    {LOOK_AT_MAP_OBJECT,        false,       true,         client_look_at_map_object},
    {TOUCH_PLAYER,              false,       true,         client_touch_player},
    {RESPOND_TO_NPC,            false,       true,         client_respond_to_npc},
    {SET_ACTIVE_CHANNEL,        false,       true,         client_set_active_channel},
    {LOG_IN,                    true,        false,        client_log_in},
    {GET_DATE,                  false,       true,         client_get_date},
    {GET_TIME,                  false,       true,         client_get_time},
    {CREATE_CHAR,               true,        false,        client_create_char},
    {LOOK_AT_INVENTORY_ITEM,    false,       true,         client_look_at_inventory_item},
    {SERVER_STATS,              false,       true,         client_server_stats},

    {255, false, false, 0}
};


static const struct protocol_array_entry *find_protocol_entry(int command) {

    /** RESULT  : finds hash command

        RETURNS : void

        PURPOSE : code modularity

        NOTES   :
    */

    int i=0;

    while(protocol_entries[i].fn!=NULL){

        if(protocol_entries[i].command==command){

            return &protocol_entries[i];
        }

        ++i;
    }

    return NULL;
}


void process_packet(int actor_node, unsigned char *packet){

    /** public function - see header */

    //log the packet
    int socket=clients.client[actor_node].socket;
    log_packet(socket, packet, RECEIVE);

    //lookup the protocol command function
    int protocol=packet[0];

    const struct protocol_array_entry *protocol_entry = find_protocol_entry((int)protocol);

    //check if protocol command exists
    if(!protocol_entry){

        log_event(EVENT_SESSION, "unknown client-server protocol");
        return;
    }

    //prevent processing of commands whilst client is connected
    if(protocol_entry->process_when_connected==false && client_socket[actor_node].socket_node_status==CLIENT_CONNECTED){

        return;
    }

    //prevent processing of commands whilst client is logged in
    if(protocol_entry->process_when_logged_in==false && client_socket[actor_node].socket_node_status==CLIENT_LOGGED_IN){

        return;
    }

    //execute the protocol command
    protocol_entry->fn(actor_node, packet);
}
