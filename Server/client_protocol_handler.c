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

#include "client_protocol.h"
#include "clients.h"
#include "logging.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "global.h"
#include "colour.h"
#include "server_messaging.h"
#include "server_protocol_functions.h"
#include "characters.h"
#include "character_race.h"
#include "db/db_character_tbl.h"
#include "game_data.h"
#include "log_in.h"
#include "map_object.h"
#include "character_movement.h"
#include "character_type.h"
#include "broadcast_actor_functions.h"
#include "chat.h"
#include "hash_commands.h"
#include "server_start_stop.h"
#include "db/database_functions.h"
#include "idle_buffer2.h"
#include "season.h"
#include "e3d.h"
#include "objects.h"
#include "harvesting.h"
#include "date_time_functions.h"
#include "client_protocol_handler.h"
#include "bags.h"
#include "packet.h"

#define DEBUG_CLIENT_PROTOCOL_HANDLER 1//set debug mode

void process_packet(int connection, unsigned char *packet){

    /** public function - see header */

    char text_out[1024]="";
    int protocol=packet[0];

    log_packet(connection, packet, RECEIVE);

/***************************************************************************************************/

    if(protocol==RAW_TEXT) {

        //copy unsigned char text to signed char variable
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
            if(chat_chan_open(connection)==false){

                send_text(connection, CHAT_SERVER, "%cyou have not joined a channel yet", c_red3+127);
                return;
            }

            //chat channels run from 32 (channel 1) to 63 (channel 32)
            int active_chan_slot=clients.client[connection].active_chan - CHAT_CHANNEL_0;

            //channel slots run from zero. Hence, we need to subtract 1 from the active_chan slot value
            int chan=clients.client[connection].chan[active_chan_slot-1];

            //broadcast to self
            send_text(connection, CHAT_CHANNEL_0 + active_chan_slot, "%c[%s @ %i]: %s", c_grey1+127, clients.client[connection].char_name, CHAT_CHANNEL_0+active_chan_slot, text);

            //broadcast to others
            broadcast_channel_chat(chan, connection, text);
        }

        //guild chat
        else if(text[0]=='~'){

            //remove the ~ from text string and add null terminator
            memcpy(text, text+1, strlen(text)-1);
            text[strlen(text)-1]='\0';

            //broadcast to self
            send_text(connection, CHAT_GM, "%c[%s]: %s", c_blue1+127, clients.client[connection].char_name, text);

            //broadcast to others
            broadcast_guild_chat(clients.client[connection].guild_id, connection, text);
        }

        //hash commands
        else if(text[0]=='#'){

            process_hash_commands(connection, text);
            return;
        }

        //local chat
        else {

            char map_name[80]="";
            strcpy(map_name, maps.map[clients.client[connection].map_id].map_name);

            //broadcast to self
            send_text(connection, CHAT_LOCAL,"%c%s: %s", c_grey1+127, clients.client[connection].char_name, text);

            //broadcast to others
            broadcast_local_chat(connection, text_out);
        }
    }
/***************************************************************************************************/

    else if(protocol==MOVE_TO) {

        //get destination tile
        int x_dest=uint16_t_to_dec(packet[3], packet[4]);
        int y_dest=uint16_t_to_dec(packet[5], packet[6]);
        int tile_dest=get_tile(x_dest, y_dest, clients.client[connection].map_id);

        //move the char
        start_char_move(connection, tile_dest);

        log_event(EVENT_SESSION, "Move [%s] Map [%s] x[%i] y[%i]", clients.client[connection].char_name, maps.map[clients.client[connection].map_id].map_name, x_dest, y_dest);
    }
/***************************************************************************************************/

    else if(protocol==SEND_PM) {

        //copy unsigned char text to signed char variable
        char text[1024]="";
        memcpy(text, packet+3, (size_t)get_packet_length(packet)-3);

        //extract target name and message from pm packet
        char char_name[80]="";
        char msg[1024]="";
        sscanf(text, "%s %[^\n]", char_name, msg);

        //send the message
        send_pm(connection, char_name, msg);

        log_event(EVENT_CHAT, "send pm from [%s] to [%s] %s", clients.client[connection].char_name, char_name, msg);
    }
/***************************************************************************************************/

    else if(protocol==SIT_DOWN){

        // the protocol recognises two sets of sit stand command. The first is implemented via the actor command set
        //which is used in the ADD_ACTOR packet; the second is implemented via the frame set which is used in the
        //ADD_ENHANCED_ACTOR packet. When using the ADD_ACTOR packet, command 13=sit down and command 14=stand up. When
        //using the ADD_ENHANCED_ACTOR packet, command 12=sit, command 13=stand and command 14=stand idle.

        if(packet[3]==SIT){//make the char stand

            clients.client[connection].frame=frame_stand;
            broadcast_actor_packet(connection, actor_cmd_stand_up, clients.client[connection].map_tile);

            #if DEBUG_CLIENT_PROTOCOL_HANDLER==1
            printf("Stand\n");
            #endif
        }
        else {// make the char sit

            clients.client[connection].frame=frame_sit;
            broadcast_actor_packet(connection, actor_cmd_sit_down, clients.client[connection].map_tile);

            #if DEBUG_CLIENT_PROTOCOL_HANDLER==1
            printf("Sit\n");
            #endif
        }

        //update database
        push_sql_command("UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i",clients.client[connection].frame, clients.client[connection].character_id);

        log_event(EVENT_SESSION, "Protocol SIT_DOWN by [%s] frame[%i]", clients.client[connection].char_name,  clients.client[connection].frame);
    }
/***************************************************************************************************/

    else if(protocol==GET_PLAYER_INFO){

        //extract integer from packet
        int other_connection=*((int*)(packet+3));

        send_text(connection, CHAT_SERVER, "You see %s", clients.client[other_connection].char_name);
   }
/***************************************************************************************************/

    else if(protocol==SEND_ME_MY_ACTORS){

    }
/***************************************************************************************************/

    else if(protocol==SEND_OPENING_SCREEN){

    }
/***************************************************************************************************/

    else if(protocol==SEND_VERSION){

        int first_digit=*((int16_t*)(packet+3));
        int second_digit=*((int16_t*)(packet+5));

        int major=(int)packet[7];
        int minor=(int)packet[8];

        int release=(int)packet[9];

        int patch=(int)packet[10];

        int host1=(int)packet[11];
        int host2=(int)packet[12];
        int host3=(int)packet[13];
        int host4=(int)packet[14];

        int port=((int)packet[15] *256)+(int)packet[16];

        log_text(EVENT_SESSION, "first digit [%i] second digit [%i] major [%i] minor [%i] release [%i] patch [%i] host [%i.%i.%i.%i] port [%i]", first_digit, second_digit, major, minor, release, patch, host1, host2, host3, host4, port);
    }
/***************************************************************************************************/

    else if(protocol==HEARTBEAT){

        //no need to do anything on this message as any data receipt updates the heartbeat
    }
/***************************************************************************************************/

    else if(protocol==USE_OBJECT){

/*
        //returns a 4byte integer indicating the threed object id, followed by a 4byte integer indicating ????

        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        use_with_position=Uint32_to_dec(data[4], data[5], data[6], data[7]);

        #if DEBUG_CLIENT_PROTOCOL_HANDLER==1
        printf("USE_OBJECT - map object [%i] position [%i]\n", map_object_id, use_with_position);
        #endif

        //if char is moving when protocol arrives, cancel rest of path
        clients.client[connection].path_count=0;

        //travel from IP to Ravens Isle
        if(map_object_id==520 && clients.client[connection].map_id==1) move_char_between_maps(connection, 2, 64946);

        //travel from Ravens Isle to IP
        if(map_object_id==5416 && clients.client[connection].map_id==2) move_char_between_maps(connection, 1, 4053);

        //travel from Ravens Isle to neiva
        if(map_object_id==4986 && clients.client[connection].map_id==2 && clients.client[connection].map_tile==108627){
            move_char_between_maps(connection, 3, 3000);
        }
 */
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_INVENTORY_ITEM){

        //returns a Uint8 giving the slot number looked at

        int slot=packet[3];
        int item=clients.client[connection].inventory[slot].object_id;

        if(object[item].edible==true){

            send_text(connection, CHAT_SERVER, "%cYou see a %s. It's edible!", c_green3+127, object[item].object_name);
        }
        else {

            send_text(connection, CHAT_SERVER, "%cYou see a %s.", c_green3+127, object[item].object_name);
        }
    }
/***************************************************************************************************/

    else if(protocol==MOVE_INVENTORY_ITEM){

        //returns 2 Uint8 indicating the slots to be moved from and to
        //if an attempt is made to move to an occupied slot or, to move from an empty slot, the client will automatically block

        int from_slot=packet[3];
        int to_slot=packet[4];

        move_inventory_item(connection, from_slot, to_slot);
    }
/***************************************************************************************************/

    else if(protocol==HARVEST){

        //returns a 16bit integer corresponding to the order of the object in the map 3d object list

        int threed_object_list_pos=uint16_t_to_dec(packet[3], packet[4]);

        log_event(EVENT_SESSION, "protocol HARVEST - started, char [%s], threed object list position [%i], map [%i]", clients.client[connection].char_name, threed_object_list_pos, clients.client[connection].map_id);

        //if char is already harvesting then stop, else start harvesting
        if(clients.client[connection].harvest_flag==true){

            stop_harvesting(connection);
        }
        else {

            start_harvesting(connection, threed_object_list_pos);
        }
    }
/***************************************************************************************************/

    else if(protocol==DROP_ITEM){

        //returns a byte indicating the slot number followed by a 32bit integer indicating the amount to be dropped
        int inventory_slot=packet[3];
        int withdraw_amount=*((int*)(packet+4));

        //determine the item to be dropped
        int object_id=clients.client[connection].inventory[inventory_slot].object_id;

        int bag_id;
        int bag_slot=-1;

        if(clients.client[connection].bag_open==true){//use existing bag

            //find the existing bag id
            bag_id=clients.client[connection].open_bag_id;

            //find a slot in which to place the item
            bag_slot=find_bag_slot(bag_id, object_id);

            //check if max bag slots exceeded
            if(bag_slot==-1){

                send_text(connection, CHAT_SERVER, "%cthere are no slots left in this bag", c_red3+127);
                return;
            }

        }
        else {// create a new bag

            // get new bag id
            bag_id=create_bag(clients.client[connection].map_id, clients.client[connection].map_tile);

            //check if max bags permitted by server has been exceeded
            if(bag_id==-1){

                send_text(connection, CHAT_SERVER, "%cThe server has reached the maximum number of bags. Wait for one to poof! ", c_red3+127);
                return;
            }

           //broadcast the bag drop
            broadcast_get_new_bag_packet(connection, bag_id);

            //place item in first slot of new bag
            bag_slot=0;
         }

        //update char to show that it is standing on an open bag
        clients.client[connection].bag_open=true;
        clients.client[connection].open_bag_id=bag_id;

        //remove item from char inventory
        int amount_withdrawn=remove_from_inventory(connection, object_id, withdraw_amount, inventory_slot);

        // add to bag
        int amount_added=add_to_bag(bag_id, object_id, amount_withdrawn, bag_slot);

        //catch if amount added to bag is less than amount withdrawn from inventory
        if(amount_added != amount_withdrawn){

            log_event(EVENT_ERROR, "char [%s] error dropping item from inventory", clients.client[connection].char_name);
            log_text(EVENT_ERROR, "item [%s]", object[object_id].object_name);
            log_text(EVENT_ERROR, "amount withdrawn from inventory [%i]", amount_withdrawn);
            log_text(EVENT_ERROR, "amount added to bag [%i]", amount_added);

            stop_server();
        }

        //send revised char inventory and bag inventory to client
        send_here_your_inventory(connection);
        send_here_your_ground_items(connection, bag_id);
    }
/***************************************************************************************************/

    else if(protocol==PICK_UP_ITEM){
/*
        //returns a 4byte integer indicating quantity followed by 1 byte indicating bag slot position

        bag_slot=data[0];
        amount=Uint32_to_dec(data[1], data[2], data[3], data[4]);

        #if DEBUG_CLIENT_PROTOCOL_HANDLER==1
        printf("PICK_UP_ITEM lsb [%i] msb [%i] amount [%i] slot [%i]\n", lsb, msb, amount, bag_slot);
        #endif

        //pick_up_from_bag(connection, bag_slot, loop);

        log_event(EVENT_SESSION, "Protocol PICK_UP_ITEM by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==INSPECT_BAG){

        //returns a Unit8 indicating the bag_id

        int bag_id=packet[3];

        //find bag at the chars current position
        bool bag_found=false;
        int i=0;

        for(i=0; i<MAX_BAGS; i++){

           if(bag[i].tile==clients.client[connection].map_tile){

                bag_found=true;
                break;
           }
        }

        //if there is no bag then abort
        if(bag_found==false){

            send_text(connection, CHAT_SERVER, "%cthere is no bag at this location", c_red3+127);
            return;
        }

        //if there is a bag then open it
        send_here_your_ground_items(connection, bag_id);

        clients.client[connection].bag_open=true;
        clients.client[connection].open_bag_id=i;
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_MAP_OBJECT){

        //returns a Uint32 indicating the position of the object in the map 3d object list
        int threed_object_list_pos=Uint32_to_dec(packet[3], packet[4], packet[5], packet[6]);

        //get the object item id
        int object_id=get_object_id(clients.client[connection].map_id, threed_object_list_pos);

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

        send_text(connection, CHAT_SERVER, text_out);
    }
/***************************************************************************************************/

    else if(protocol==PING_RESPONSE){

    }
/***************************************************************************************************/

    else if(protocol==SET_ACTIVE_CHANNEL){

        //set the active channel
        clients.client[connection].active_chan=packet[3]-32;

        //update the database
        push_sql_command("UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=%i WHERE CHAR_ID=%i", packet[3], clients.client[connection].character_id);

    }
/***************************************************************************************************/

    else if(protocol==LOG_IN){

        size_t packet_len=get_packet_length(packet);
        push_idle_buffer2(connection, IDLE_BUFFER_PROCESS_LOGIN, packet, packet_len);
    }
/***************************************************************************************************/

    else if(protocol==CREATE_CHAR){

        size_t packet_len=get_packet_length(packet);
        push_idle_buffer2(connection, IDLE_BUFFER_PROCESS_CHECK_NEWCHAR, packet, packet_len);
    }
/***************************************************************************************************/

    else if(protocol==GET_DATE){

        //calculate day of year
        int day_of_year=game_data.game_days % game_data.year_length;
        send_verbose_date(connection, day_of_year);
    }
/***************************************************************************************************/

    else if(protocol==GET_TIME){

        send_verbose_time(connection, game_data.game_minutes);
    }
/***************************************************************************************************/

    else if(protocol==SERVER_STATS){

        send_motd_header(connection);
    }
/***************************************************************************************************/

    else {

        #if DEBUG_CLIENT_PROTOCOL_HANDLER==1
        printf("UNKNOWN PROTOCOL %i %i \n", packet[1], packet[2]);
        #endif

        // catch unknown protocols
        log_event(EVENT_SESSION, "unknown protocol [%i]", protocol);
    }
}
