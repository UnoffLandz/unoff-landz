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

#include <stdio.h>  //support for sprintf
#include <ev.h>     // evlib event library
#include <string.h> //support for memcpy strlen strcpy

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
#include "db_character_tbl.h"
#include "game_data.h"
#include "chat_channels.h"
#include "log_in.h"
#include "maps.h"
#include "character_movement.h"
#include "character_type.h"
#include "broadcast_actor_functions.h"
#include "chat.h"
#include "hash_commands.h"
#include "server_start_stop.h"
#include "database_functions.h"
#include "database_buffer.h"
#include "test.h"

#define DEBUG_PACKET 1//set debug mode

void process_packet(int connection, unsigned char *packet){

    /** public function - see header */

    unsigned char data[1024];
    char text[1024]="";
    char text_out[1024]="";

    int protocol=packet[0], lsb=packet[1], msb=packet[2];
    //int map_object_id=0;
    //int use_with_position=0;
    //int image_id=0;
    //int amount=0;
    //int move_to_slot=0, move_from_slot=0, bag_id=0, bag_slot=0, inventory_slot=0;

    int data_length=lsb+(msb*256)-1;

    //packet logging
    int i=0;
    for(i=0; i<data_length+2; i++){

        sprintf(text_out, "%s %i", text_out, packet[i]);
    }

    log_event(EVENT_PACKET,"Receive from [%i]%s", connection, text_out);

    //extract data from packet
    for(i=0; i<data_length; i++){

        data[i]=packet[i+3]; // unsigned char array for bit manipulation
        text[i]=packet[i+3]; // signed char array for text manipulation
    }

    //remove colour codes from text
    //filter_str_range(text, MIN_PRINTABLE_ASCII, MAX_PRINTABLE_ASCII);

    /*when the client connects to the server, it will send a SIT_DOWN message if there is a delay in logging in which
    unless trapped, results in a floating point error exception that causes the server to crash.*/
    //if(clients.client[connection].client_status==CONNECTED && protocol ==SIT_DOWN) return;

/***************************************************************************************************/

    if(protocol==RAW_TEXT) {

        #if DEBUG_PACKET==1
        printf("RAW_TEXT [%s]\n", text);
        #endif

        // trim off excess left hand space
        str_trim_left(text);

        //channel chat
        if(text[0]=='@'){

            //remove the @ from text string and add null terminator
            memcpy(text, text+1, strlen(text)-1);
            text[strlen(text)-1]='\0';

            // check if char has an active channel
            if(clients.client[connection].active_chan==0){

                sprintf(text_out, "%cyou have not joined a channel yet", c_red3+127);
                send_raw_text(connection, CHAT_SERVER, text_out);
                return;
            }

            // get the chan number
            int active_chan_slot=clients.client[connection].active_chan-1;
            int chan=clients.client[connection].chan[active_chan_slot];
            printf("active chan slot %i  chan %i\n", active_chan_slot, chan);

            //broadcast to self
            sprintf(text_out, "%c[%s @ %i]: %s", c_grey1+127, clients.client[connection].char_name, chan, text);
            send_raw_text(connection, CHAT_SERVER, text_out);

            //broadcast to others
            broadcast_channel_chat(chan, connection, text);

            log_event(EVENT_CHAT, "broadcast channel [%s @% i]: %s", clients.client[connection].char_name, chan, text);

            #if DEBUG_PACKET==1
            printf("channel chat by [%s] on chan [%i] %s\n", clients.client[connection].char_name, chan, text);
            #endif
        }

        //hash commands
        else if(text[0]=='#'){

            #if DEBUG_PACKET==1
            printf("#command by [%s] %s\n", clients.client[connection].char_name, text);
            #endif

            process_hash_commands(connection, text);

            log_event(EVENT_SESSION, "#command %s [%s]", clients.client[connection].char_name, text);

            return;
        }

        //local chat
        else {

            char map_name[80]="";
            strcpy(map_name, maps.map[clients.client[connection].map_id].map_name);

            sprintf(text_out, "%c%s: %s", c_grey1+127, clients.client[connection].char_name, text);

            //broadcast to self
            send_raw_text(connection, CHAT_LOCAL, text_out);

            //broadcast to others
            broadcast_local_chat(connection, text_out);

            log_event(EVENT_CHAT, "broadcast local [%s] %s: %s", map_name, clients.client[connection].char_name, text);

            #if DEBUG_PACKET==1
            printf("local chat on map [%s] %s: %s\n", map_name, clients.client[connection].char_name, text);
            #endif
        }
    }
/***************************************************************************************************/

    else if(protocol==MOVE_TO) {

        #if DEBUG_PACKET==1
        printf("MOVE_TO %i %s\n", connection, clients.client[connection].char_name);
        #endif

        //returns 2x 2byte integers indicating the x/y axis of the destination
        int x_dest=Uint16_to_dec(data[0], data[1]);
        int y_dest=Uint16_to_dec(data[2], data[3]);
        int tile_dest=x_dest+(y_dest*maps.map[clients.client[connection].map_id].map_axis);

        #if DEBUG_PACKET==1
        printf("position x[%i] y[%i] tile[%i]\n", x_dest, y_dest, tile_dest);
        #endif

        start_char_move(connection, tile_dest);

        log_event(EVENT_SESSION, "Protocol MOVE_TO by [%s]...", clients.client[connection].char_name);
        log_text(EVENT_SESSION, "Map [%s] x[%i] y[%i]", maps.map[clients.client[connection].map_id].map_name, x_dest, y_dest);
    }
/***************************************************************************************************/

    else if(protocol==SEND_PM) {

        #if DEBUG_PACKET==1
        printf("SEND_PM %i %i %s\n", lsb, msb, text);
        #endif

        //extract target name and message from pm packet
        char char_name[80]="";
        get_str_island(text, char_name, 1);

        char msg[1024]="";
        get_str_island(text, msg, 2);

        //log the event here as the send_pm function adds a log entry if the target char is not found
        log_event(EVENT_CHAT, "send pm from [%s] to [%s] %s", clients.client[connection].char_name, char_name, msg);

        //send the message
        send_pm(connection, char_name, msg);
    }
/***************************************************************************************************/

    else if(protocol==SIT_DOWN){

        char sql[MAX_SQL_LEN]="";

        #if DEBUG_PACKET==1
        printf("SIT_DOWN %i\n", data[0]);
        #endif

        // the protocol recognises two sets of sit stand command. The first is implemented via the actor command set
        //which is used in the ADD_ACTOR packet; the second is implemented via the frame set which is used in the
        //ADD_ENHANCED_ACTOR packet. When using the ADD_ACTOR packet, command 13=sit down and command 14=stand up. When
        //using the ADD_ENHANCED_ACTOR packet, command 12=sit, command 13=stand and command 14=stand idle.

        switch(clients.client[connection].frame){

            case frame_sit:

                #if DEBUG_PACKET==1
                printf("Stand\n");
                #endif

                clients.client[connection].frame=frame_stand;

                broadcast_actor_packet(connection, actor_cmd_stand_up, clients.client[connection].map_tile);

                //update database here else, if we do it after the switch structure, an unknown frame value
                //could end up being updated to the database
                snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i;",clients.client[connection].frame, clients.client[connection].character_id);
                db_push_buffer(sql, 0, DB_BUFFER_PROCESS_SQL, NULL);

                log_event(EVENT_SESSION, "Protocol SIT_DOWN by [%s] (stand)", clients.client[connection].char_name);
                break;

            case frame_stand:

                #if DEBUG_PACKET==1
                printf("Sit\n");
                #endif

                clients.client[connection].frame=frame_sit;

                broadcast_actor_packet(connection, actor_cmd_sit_down, clients.client[connection].map_tile);

                //update database here else, if we do it after the switch structure, an unknown frame value
                //could end up being updated to the database
                sprintf(sql, "UPDATE CHARACTER_TABLE SET FRAME=%i WHERE CHAR_ID=%i;",clients.client[connection].frame, clients.client[connection].character_id);
                db_push_buffer(sql, 0, DB_BUFFER_PROCESS_SQL, NULL);

                log_event(EVENT_SESSION, "Protocol SIT_DOWN by [%s] (sit)", clients.client[connection].char_name);
                break;

            default:

                #if DEBUG_PACKET==1
                printf("Unknown sit stand command [%i]\n", data[0]);
                #endif

                log_event(EVENT_ERROR, "Protocol SIT_DOWN by [%s] (unknown)", clients.client[connection].char_name);
                break;
        }
     }
/***************************************************************************************************/

    else if(protocol==GET_PLAYER_INFO){

        int other_connection=Uint32_to_dec(data[0], data[1], data[2], data[3]);

        #if DEBUG_PACKET==1
        printf("GET_PLAYER_INFO [%i] [%s]\n", other_connection, clients.client[other_connection].char_name);
        #endif

        sprintf(text_out, "You see %s", clients.client[other_connection].char_name);
        send_raw_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_ERROR, "Protocol GET_PLAYER_INFO by [%s] (%s)", clients.client[connection].char_name, clients.client[other_connection].char_name);
   }
/***************************************************************************************************/

    else if(protocol==SEND_ME_MY_ACTORS){

        #if DEBUG_PACKET==1
        printf("SEND_ME_MY_ACTORS %i %i \n", lsb, msb);
        #endif

        log_event(EVENT_ERROR, "Protocol SEND_ME_MY_ACTORS by [%s]", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==SEND_OPENING_SCREEN){

        #if DEBUG_PACKET==1
        printf("SEND OPENING SCREEN %i %i \n", lsb, msb);
        #endif

        log_event(EVENT_SESSION, "Protocol SEND_OPENING_SCREEN by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==SEND_VERSION){

        #if DEBUG_PACKET==1
        printf("SEND_VERSION %i %i\n", lsb, msb);
        #endif

        int first_digit=Uint16_to_dec(data[0], data[1]);
        int second_digit=Uint16_to_dec(data[2], data[3]);
        int major=(int)data[4];
        int minor=(int)data[5];
        int release=(int)data[6];
        int patch=(int)data[7];
        int host1=(int)data[8];
        int host2=(int)data[9];
        int host3=(int)data[10];
        int host4=(int)data[11];
        int port=((int)data[12] *256)+(int)data[13];

        #if DEBUG_PACKET==1
        printf("maj [%i] min [%i] version [%i.%i.%i.%i]\n",
               first_digit, second_digit,
               major, minor,
               release,
               patch);

        printf("server host [%i.%i.%i.%i] port [%i]\n", host1, host2, host3, host4, port);
        #endif

        log_event(EVENT_SESSION, "Protocol SEND_VERSION by [%s]...", clients.client[connection].char_name);
        log_text(EVENT_SESSION, "first digit [%i] second digit [%i]", first_digit, second_digit);
        log_text(EVENT_SESSION, "major [%i] minor [%i] release [%i] patch [%i]", major, minor, release, patch);
        log_text(EVENT_SESSION, "host [%i.%i.%i.%i] port [%i]", host1, host2, host3, host4, port);
    }
/***************************************************************************************************/

    else if(protocol==HEARTBEAT){

        #if DEBUG_PACKET==1
        printf("HEARTBEAT %i %i \n", lsb, msb);
        #endif

        //no need to do anything on this message as any data receipt updates the heartbeat
    }
/***************************************************************************************************/

    else if(protocol==USE_OBJECT){

        #if DEBUG_PACKET==1
        printf("USE_OBJECT %i %i \n", lsb, msb);
        #endif

/*
        //returns a 4byte integer indicating the threed object id, followed by a 4byte integer indicating ????

        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        use_with_position=Uint32_to_dec(data[4], data[5], data[6], data[7]);

        #if DEBUG_PACKET==1
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

        log_event(EVENT_SESSION, "Protocol USE_OBJECT by [%s]...", clients.client[connection].char_name);
 */
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_INVENTORY_ITEM){

        #if DEBUG_PACKET==1
        printf("LOOK_AT_INVENTORY_ITEM %i %i \n", lsb, msb);
        #endif

/*
        //returns a Uint8 giving the slot number looked at

        inventory_slot=(int)data[0];
        image_id=clients.client[connection].client_inventory[inventory_slot].image_id;

        #if DEBUG_PACKET==1
        printf("LOOK_AT_INVENTORY_ITEM - slot [%i]\n", inventory_slot);
        #endif

        sprintf(text_out, "%c%s", c_green3+127, item[image_id].item_name);
        send_server_text(connection, CHAT_SERVER, text_out);

        log_event(EVENT_SESSION, "Protocol LOOK_AT_INVENTORY_ITEM by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==MOVE_INVENTORY_ITEM){

        #if DEBUG_PACKET==1
        printf("MOVE_INVENTORY_ITEM %i %i \n", lsb, msb);
        #endif

/*
        //returns 2 Uint8 indicating the slots to be moved from and to
        //if an attempt is made to move to an occupied slot or, to move from an empty slot, the client will automatically block

        move_from_slot=(int)data[0];
        move_to_slot=(int)data[1];

        #if DEBUG_PACKET==1
        printf("MOVE_INVENTORY_ITEM - slot [%i] to slot [%i]\n", move_from_slot, move_to_slot);
        #endif

        image_id=clients.client[connection].client_inventory[move_from_slot].image_id;
        amount=clients.client[connection].client_inventory[move_from_slot].amount;

        //zero the 'from slot'
        clients.client[connection].client_inventory[move_from_slot].image_id=0;
        clients.client[connection].client_inventory[move_from_slot].amount=0;
        send_get_new_inventory_item(connection, 0, 0, move_from_slot);

        //save to database
        update_db_char_slot(connection, move_from_slot);

        //place item in the 'to slot'
        clients.client[connection].client_inventory[move_to_slot].image_id=image_id;
        clients.client[connection].client_inventory[move_to_slot].amount=amount;
        send_get_new_inventory_item(connection, image_id, amount, move_to_slot);

        //save to the database
        update_db_char_slot(connection, move_to_slot);

        log_event(EVENT_SESSION, "Protocol MOVE_INVENTORY_ITEM by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==HARVEST){

        #if DEBUG_PACKET==1
        printf("HARVEST %i %i \n", lsb, msb);
        #endif

/*
        //returns a integer corresponding to the id of an object in the map 3d object list

        map_object_id=Uint16_to_dec(data[0], data[1]);

        #if DEBUG_PACKET==1
        printf("HARVEST object_id %i\n", map_object_id);
        #endif

        start_harvesting2(connection, map_object_id, loop);

        log_event(EVENT_SESSION, "Protocol HARVEST by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==DROP_ITEM){

        #if DEBUG_PACKET==1
        printf("DROP_ITEM %i %i \n", lsb, msb);
        #endif

/*
        //returns a byte indicating the slot number followed by a 32bit integer indicating the amount to be dropped

        inventory_slot=data[0];
        amount=Uint32_to_dec(data[1], data[2], data[3], data[4]);
        image_id=clients.client[connection].client_inventory[inventory_slot].image_id;

        #if DEBUG_PACKET==1
        printf("DROP_ITEM image_id [%i] drop amount [%i]\n", image_id, amount);
        #endif

        drop_from_inventory(connection, inventory_slot, amount, loop);

        log_event(EVENT_SESSION, "Protocol DROP_ITEM by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==PICK_UP_ITEM){

        #if DEBUG_PACKET==1
        printf("PICK_UP_ITEM %i %i \n", lsb, msb);
        #endif

/*
        //returns a 4byte integer indicating quantity followed by 1 byte indicating bag slot position

        bag_slot=data[0];
        amount=Uint32_to_dec(data[1], data[2], data[3], data[4]);

        #if DEBUG_PACKET==1
        printf("PICK_UP_ITEM lsb [%i] msb [%i] amount [%i] slot [%i]\n", lsb, msb, amount, bag_slot);
        #endif

        //pick_up_from_bag(connection, bag_slot, loop);

        log_event(EVENT_SESSION, "Protocol PICK_UP_ITEM by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==INSPECT_BAG){

        #if DEBUG_PACKET==1
        printf("INSPECT_BAG %i %i \n", lsb, msb);
        #endif

/*
        //returns a Unit8 indicating the bag_id

        bag_id=data[0];

        #if DEBUG_PACKET==1
        printf("INSPECT_BAG - lsb [%i] msb [%i] bag id [%i]\n", lsb, msb, bag_id);
        #endif

        //check we are standing on the bag
        if(bag_list[bag_id].tile_pos==current_tile) {

            //if we are standing on the bag then attempt to open it
           //send_here_your_ground_items(connection, bag_id);
            clients.client[connection].bag_open=TRUE;
        }
        else {

            //not standing on bag so move towards it
            start_char_move(connection, bag_list[bag_id].tile_pos, loop);
        }

        log_event(EVENT_SESSION, "Protocol INSPECT_BAG by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_MAP_OBJECT){

        #if DEBUG_PACKET==1
        printf("LOOK_AT_MAP_OBJECT %i %i \n", lsb, msb);
        #endif

/*
        //returns a Uint32 indicating the object_id of the item looked at

        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);


        //populate the map_object struct with data from the map_object
        get_map_object(map_object_id, map_id);

        //tell the client what the map object is
        if(map_object.image_id>0){
            sprintf(text_out, "%c%s", c_green3+127, item[map_object.image_id].item_name);
        }
        else {
            sprintf(text_out, "%cUnknown item", c_green3+127);
        }

        send_server_text(connection, CHAT_SERVER, text_out);

        #if DEBUG_PACKET==1
        printf("LOOK_AT_MAP_OBJECT - map object [%i] [%s]\n", map_object_id, item[map_object.image_id].item_name);
        #endif

        log_event(EVENT_SESSION, "Protocol LOOK_AT_MAP_OBJECT by [%s]...", clients.client[connection].char_name);
*/
    }
/***************************************************************************************************/

    else if(protocol==PING_RESPONSE){

        #if DEBUG_PACKET==1
        printf("PING_RESPONSE %i %i \n", lsb, msb);
        #endif

        log_event(EVENT_SESSION, "Protocol PING_RESPONSE by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==SET_ACTIVE_CHANNEL){

        #if DEBUG_PACKET==1
        printf("SET_ACTIVE_CHANNEL [%s] [%i]\n", clients.client[connection].char_name, data[0]);
        #endif

        //set the active channel
        clients.client[connection].active_chan=MAX_ACTIVE_CHANNELS-data[0];

        //update the database
        char sql[MAX_SQL_LEN]="";
        snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET ACTIVE_CHAN=%i WHERE CHAR_ID=%i", data[0], clients.client[connection].character_id);
        db_push_buffer(sql, 0, DB_BUFFER_PROCESS_SQL, NULL);

        log_event(EVENT_SESSION, "Protocol SET_ACTIVE_CHANNEL by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==LOG_IN){

        #if DEBUG_PACKET==q
        printf("LOG_IN connection [%i] lsb [%i] msb [%i]\n", connection, lsb, msb);
        #endif

        //place log event before process so the log entries from the process_log_in function follow
        //in a logical order
        log_event(EVENT_SESSION, "Protocol LOG_IN by [%i]...", connection);

        //process_log_in(connection, packet);
        db_push_buffer("", connection, DB_BUFFER_PROCESS_LOGIN, packet);
    }
/***************************************************************************************************/

    else if(protocol==CREATE_CHAR){

        #if DEBUG_PACKET==1
        printf("CREATE_CHAR connection [%i] lsb [%i] msb [%i]\n", connection, lsb, msb);
        #endif

        //place log event before process so the following are in a logical order
        log_event(EVENT_SESSION, "Protocol CREATE_CHAR by [%i]...", connection);

        //detect and handle malformed packet
        if(count_str_island(text)!=2){

            send_create_char_not_ok(connection);
            log_event(EVENT_ERROR, "malformed login attempt for new char");
            stop_server();
        }

        db_push_buffer("", connection, DB_BUFFER_PROCESS_CHECK_NEWCHAR, packet);

/*
        //get the char name and password from the packet
        char char_name[80]="";
        char password[80]="";
        get_name_and_password_from_newchar_packet(packet, char_name, password);

        //check if char name is already used
        if(get_db_char_data(char_name)==FOUND){

            //sends message to create char window on client
            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text(connection, CHAT_SERVER, text_out);

            send_create_char_not_ok(connection);
            log_event(EVENT_SESSION, "Attempt to create new char with existing char name [%s]\n", char_name);

            return;
        }

        clients.client[connection].client_status=LOGGED_IN;

        //extract data from the create_char packet
        strcpy(character.char_name, char_name);
        strcpy(character.password, password);
        i=strlen(char_name)+strlen(password)+2;

        character.skin_type=data[i++];
        character.hair_type=data[i++];
        character.shirt_type=data[i++];
        character.pants_type=data[i++];
        character.boots_type=data[i++];
        character.char_type=data[i++];
        character.head_type=data[i++];

        //set the char to stand
        character.frame=frame_stand;

        //set the char creation time
        character.char_created=time(NULL);

        //set starting channel
        character.active_chan=1;
        character.chan[0]=1; //nub chan

        //set starting map and tile
        character.map_id=game_data.beam_map_id;
        character.map_tile=game_data.beam_map_tile;

        //add character entry to database and get the record id
        clients.client[connection].character_id=add_db_char_data(character);

        //add initial items to inventory
        //int slot=0;
        //add_item_to_inventory(connection, 612, 1, &slot);
        //add_item_to_inventory(connection, 613, 1, &slot);
        //add_item_to_inventory(connection, 216, 1, &slot);
        //add_item_to_inventory(connection, 217, 1, &slot);

        //update game data
        race[character.char_type].char_count++;
        game_data.char_count++;
        strcpy(game_data.name_last_char_created, char_name);
        game_data.date_last_char_created=character.char_created;

        //notify client that character has been created
        sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
        send_raw_text(connection, CHAT_SERVER, text_out);
        send_create_char_ok(connection);

        //log character creation event
        log_event(EVENT_NEW_CHAR, "[%s] password [%s]\n", char_name, password);
*/
    }
/***************************************************************************************************/

    else if(protocol==GET_DATE){

        #if DEBUG_PACKET==1
        printf("GET DATE %i %i \n", lsb, msb);
        #endif

        log_event(EVENT_SESSION, "Protocol GET_DATE by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==GET_TIME){

        #if DEBUG_PACKET==1
        printf("GET TIME %i %i\n", lsb, msb);
        #endif

        log_event(EVENT_SESSION, "Protocol GET_TIME by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else if(protocol==SERVER_STATS){

        #if DEBUG_PACKET==1
        printf("SERVER_STATS %i %i \n", lsb, msb);
        #endif

        send_motd_header(connection);

        log_event(EVENT_SESSION, "Protocol SERVER_STATS by [%s]...", clients.client[connection].char_name);
    }
/***************************************************************************************************/

    else {

        #if DEBUG_PACKET==1
        printf("UNKNOWN PROTOCOL %i %i \n", lsb, msb);
        #endif

        // catch unknown protocols
        log_event(EVENT_ERROR, "unknown protocol [%i]", protocol);
    }
}
