#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for usec time
#include <time.h>
#include <unistd.h>
#include <ev.h> //needed for bag timer

#include "global.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "protocol.h"
#include "hash_commands.h"
#include "broadcast.h"
#include "files.h"
#include "character_movement.h"
#include "chat.h"
#include "debug.h"
#include "maps.h"
#include "motd.h"
#include "pathfinding.h"
#include "chat.h"
#include "log_in.h"
#include "database.h"
#include "harvesting.h"
#include "character_inventory.h"

void send_server_text(int connection, int channel, char *text){

    /* function definition sends to sock rather than connection so that we can reach clients when a connection has not been
    allocated. This happens when a client tries to connect after the maximum number of connections has been exceeded, in
    which case, we need to be able to send a message to that client telling it to wait until a connection becomes available.
    See the 'find free connection' block in main.c */

    unsigned char packet[1024];
    int text_length;
    int message_length;
    int packet_length;

    text_length=strlen(text);
    message_length=text_length+2; // add 1 for the channel byte and 1 for the msb byte

    packet[0]=0;
    packet[1]=message_length % 256;
    packet[2]=message_length / 256;
    packet[3]=channel;

    memmove(packet+4, text, text_length);

    packet_length=message_length+2; // add 1 for the protocol byte and 1 for the lsb byte

    send(connection, packet, packet_length, 0);
}

void send_create_char_ok(int sock){

    unsigned char packet[3];

    packet[0]=252;
    packet[1]=1;
    packet[2]=0;

    send(sock, packet, 3, 0);
}

void send_create_char_not_ok(int sock){

    unsigned char packet[3];

    packet[0]=253;
    packet[1]=1;
    packet[2]=0;

    send(sock, packet, 3, 0);
}

void send_here_your_stats(int connection){

    unsigned char packet[1024];

    int i=0;

    packet[0]=HERE_YOUR_STATS;
    packet[1]=227;
    packet[2]=0;

    for(i=3; i<230; i++){
        packet[i]=0;
    }

    packet[3]=clients.client[connection]->physique % 256;
    packet[4]=clients.client[connection]->physique / 256;
    packet[5]=clients.client[connection]->max_physique % 256;
    packet[6]=clients.client[connection]->max_physique / 256;
    packet[7]=clients.client[connection]->coordination % 256;
    packet[8]=clients.client[connection]->coordination / 256;
    packet[9]=clients.client[connection]->max_coordination % 256;
    packet[10]=clients.client[connection]->max_coordination / 256;
    packet[11]=clients.client[connection]->reasoning % 256;
    packet[12]=clients.client[connection]->reasoning / 256;
    packet[13]=clients.client[connection]->max_reasoning % 256;
    packet[14]=clients.client[connection]->max_reasoning / 256;
    packet[15]=clients.client[connection]->will % 256;
    packet[16]=clients.client[connection]->will / 256;
    packet[17]=clients.client[connection]->max_will % 256;
    packet[18]=clients.client[connection]->max_will / 256;
    packet[19]=clients.client[connection]->instinct % 256;
    packet[20]=clients.client[connection]->instinct / 256;
    packet[21]=clients.client[connection]->max_instinct % 256;
    packet[22]=clients.client[connection]->max_instinct / 256;
    packet[23]=clients.client[connection]->vitality % 256;
    packet[24]=clients.client[connection]->vitality / 256;
    packet[25]=clients.client[connection]->max_vitality % 256;
    packet[26]=clients.client[connection]->max_vitality / 256;

    packet[27]=clients.client[connection]->human % 256;
    packet[28]=clients.client[connection]->human / 256;
    packet[29]=clients.client[connection]->max_human % 256;
    packet[30]=clients.client[connection]->max_human / 256;
    packet[31]=clients.client[connection]->animal % 256;
    packet[32]=clients.client[connection]->animal / 256;
    packet[33]=clients.client[connection]->max_animal % 256;
    packet[34]=clients.client[connection]->max_animal / 256;
    packet[35]=clients.client[connection]->vegetal % 256;
    packet[36]=clients.client[connection]->vegetal / 256;
    packet[37]=clients.client[connection]->max_vegetal % 256;
    packet[38]=clients.client[connection]->max_vegetal / 256;
    packet[39]=clients.client[connection]->inorganic % 256;
    packet[40]=clients.client[connection]->inorganic / 256;
    packet[41]=clients.client[connection]->max_inorganic % 256;
    packet[42]=clients.client[connection]->max_inorganic / 256;
    packet[43]=clients.client[connection]->artificial % 256;
    packet[44]=clients.client[connection]->artificial / 256;
    packet[45]=clients.client[connection]->max_artificial % 256;
    packet[46]=clients.client[connection]->max_artificial / 256;
    packet[47]=clients.client[connection]->magic % 256;
    packet[48]=clients.client[connection]->magic / 256;
    packet[49]=clients.client[connection]->max_magic % 256;
    packet[50]=clients.client[connection]->max_magic / 256;

    packet[51]=clients.client[connection]->manufacturing_lvl % 256;
    packet[52]=clients.client[connection]->manufacturing_lvl / 256;
    packet[53]=clients.client[connection]->max_manufacturing_lvl % 256;
    packet[54]=clients.client[connection]->max_manufacturing_lvl / 256;
    packet[55]=clients.client[connection]->harvest_lvl % 256;
    packet[56]=clients.client[connection]->harvest_lvl / 256;
    packet[57]=clients.client[connection]->max_harvest_lvl % 256;
    packet[58]=clients.client[connection]->max_harvest_lvl / 256;
    packet[59]=clients.client[connection]->alchemy_lvl % 256;
    packet[60]=clients.client[connection]->alchemy_lvl / 256;
    packet[61]=clients.client[connection]->max_alchemy_lvl % 256;
    packet[62]=clients.client[connection]->max_alchemy_lvl / 256;
    packet[63]=clients.client[connection]->overall_lvl % 256;
    packet[64]=clients.client[connection]->overall_lvl / 256;
    packet[65]=clients.client[connection]->max_overall_lvl % 256;
    packet[66]=clients.client[connection]->max_overall_lvl / 256;
    packet[67]=clients.client[connection]->attack_lvl % 256;
    packet[68]=clients.client[connection]->attack_lvl / 256;
    packet[69]=clients.client[connection]->max_attack_lvl % 256;
    packet[70]=clients.client[connection]->max_attack_lvl / 256;
    packet[71]=clients.client[connection]->defence_lvl % 256;
    packet[72]=clients.client[connection]->defence_lvl / 256;
    packet[73]=clients.client[connection]->max_defence_lvl % 256;
    packet[74]=clients.client[connection]->max_defence_lvl / 256;
    packet[75]=clients.client[connection]->magic_lvl % 256;
    packet[76]=clients.client[connection]->magic_lvl / 256;
    packet[77]=clients.client[connection]->max_magic_lvl % 256;
    packet[78]=clients.client[connection]->max_magic_lvl / 256;
    packet[79]=clients.client[connection]->potion_lvl % 256;
    packet[80]=clients.client[connection]->potion_lvl / 256;
    packet[81]=clients.client[connection]->max_potion_lvl % 256;
    packet[82]=clients.client[connection]->max_potion_lvl / 256;

    packet[83]=clients.client[connection]->inventory_emu % 256; // amount of emu in inventory
    packet[84]=clients.client[connection]->inventory_emu / 256;

    int max_carry_capacity=get_char_carry_capacity(connection);
    packet[85]=max_carry_capacity % 256; // max emu that can be held in inventory
    packet[86]=max_carry_capacity / 256;

    packet[87]=clients.client[connection]->material_pts % 256;
    packet[88]=clients.client[connection]->material_pts / 256;
    packet[89]=clients.client[connection]->max_material_pts % 256;
    packet[90]=clients.client[connection]->max_material_pts / 256;

    packet[91]=clients.client[connection]->ethereal_pts % 256;
    packet[92]=clients.client[connection]->ethereal_pts / 256;
    packet[93]=clients.client[connection]->max_ethereal_pts % 256;
    packet[94]=clients.client[connection]->max_ethereal_pts / 256;

    packet[95]=clients.client[connection]->food_lvl % 256;
    packet[96]=clients.client[connection]->food_lvl / 256;
    packet[97]=clients.client[connection]->elapsed_book_time % 256;
    packet[98]=clients.client[connection]->elapsed_book_time / 256;

    //packet[99]=; Unused
    //packet[100]=; Unused

    packet[101]=clients.client[connection]->manufacture_exp % 256;
    packet[102]=clients.client[connection]->manufacture_exp / 256 % 256;
    packet[103]=clients.client[connection]->manufacture_exp / 256 / 256 % 256;
    packet[104]=clients.client[connection]->manufacture_exp / 256 / 256 / 256 % 256;
    packet[105]=clients.client[connection]->max_manufacture_exp % 256;
    packet[106]=clients.client[connection]->max_manufacture_exp / 256 % 256;
    packet[107]=clients.client[connection]->max_manufacture_exp / 256 / 256 % 256;
    packet[108]=clients.client[connection]->max_manufacture_exp / 256 / 256 / 256 % 256;
    packet[109]=clients.client[connection]->harvest_exp % 256;
    packet[110]=clients.client[connection]->harvest_exp / 256 % 256;
    packet[111]=clients.client[connection]->harvest_exp / 256 / 256 % 256;
    packet[112]=clients.client[connection]->harvest_exp / 256 / 256 / 256 % 256 ;
    packet[113]=clients.client[connection]->max_harvest_exp % 256;
    packet[114]=clients.client[connection]->max_harvest_exp / 256 % 256;
    packet[115]=clients.client[connection]->max_harvest_exp / 256 / 256 % 256;
    packet[116]=clients.client[connection]->max_harvest_exp / 256 / 256 / 256 % 256;
    packet[117]=clients.client[connection]->alchemy_exp % 256;
    packet[118]=clients.client[connection]->alchemy_exp / 256 % 256;
    packet[119]=clients.client[connection]->alchemy_exp / 256 / 256 % 256;
    packet[120]=clients.client[connection]->alchemy_exp / 256 / 256 / 256 % 256;
    packet[121]=clients.client[connection]->max_alchemy_exp % 256;
    packet[122]=clients.client[connection]->max_alchemy_exp / 256 % 256;
    packet[123]=clients.client[connection]->max_alchemy_exp / 256 / 256 % 256;
    packet[124]=clients.client[connection]->max_alchemy_exp / 256 / 256 / 256 % 256;
    packet[125]=clients.client[connection]->overall_exp % 256;
    packet[126]=clients.client[connection]->overall_exp / 256 % 256;
    packet[127]=clients.client[connection]->overall_exp / 256 / 256 % 256;
    packet[128]=clients.client[connection]->overall_exp / 256 / 256 / 256 % 256;
    packet[129]=clients.client[connection]->max_overall_exp % 256;
    packet[130]=clients.client[connection]->max_overall_exp / 256 % 256;
    packet[131]=clients.client[connection]->max_overall_exp / 256 / 256 % 256;
    packet[132]=clients.client[connection]->max_overall_exp / 256 / 256 / 256 % 256;
    packet[133]=clients.client[connection]->attack_exp % 256;
    packet[134]=clients.client[connection]->attack_exp / 256 % 256;
    packet[135]=clients.client[connection]->attack_exp / 256 / 256 % 256;
    packet[136]=clients.client[connection]->attack_exp / 256 / 256 / 256 % 256;
    packet[137]=clients.client[connection]->max_attack_exp % 256;
    packet[138]=clients.client[connection]->max_attack_exp / 256 % 256;
    packet[139]=clients.client[connection]->max_attack_exp / 256 / 256 % 256;
    packet[140]=clients.client[connection]->max_attack_exp / 256 / 256 / 256 % 256;
    packet[141]=clients.client[connection]->defence_exp % 256;
    packet[142]=clients.client[connection]->defence_exp / 256 % 256;
    packet[143]=clients.client[connection]->defence_exp / 256 / 256 % 256;
    packet[144]=clients.client[connection]->defence_exp / 256 / 256 / 256 % 256;
    packet[145]=clients.client[connection]->max_defence_exp % 256;
    packet[146]=clients.client[connection]->max_defence_exp / 256 % 256;
    packet[147]=clients.client[connection]->max_defence_exp / 256 / 256 % 256;
    packet[148]=clients.client[connection]->max_defence_exp / 256 / 256 / 256 % 256;
    packet[149]=clients.client[connection]->magic_exp % 256;
    packet[150]=clients.client[connection]->magic_exp / 256 % 256;
    packet[151]=clients.client[connection]->magic_exp / 256 / 256 % 256;
    packet[152]=clients.client[connection]->magic_exp / 256 / 256 / 256 % 256;
    packet[153]=clients.client[connection]->max_magic_exp % 256;
    packet[154]=clients.client[connection]->max_magic_exp / 256 % 256;
    packet[155]=clients.client[connection]->max_magic_exp / 256 / 256 % 256;
    packet[156]=clients.client[connection]->max_magic_exp / 256 / 256 / 256 % 256;
    packet[157]=clients.client[connection]->potion_exp % 256;
    packet[158]=clients.client[connection]->potion_exp / 256 % 256;
    packet[159]=clients.client[connection]->potion_exp / 256 / 256 % 256;
    packet[160]=clients.client[connection]->potion_exp / 256 / 256 / 256 % 256;
    packet[161]=clients.client[connection]->max_potion_exp % 256;
    packet[162]=clients.client[connection]->max_potion_exp / 256 % 256;
    packet[163]=clients.client[connection]->max_potion_exp / 256 / 256 % 256;
    packet[164]=clients.client[connection]->max_potion_exp / 256 / 256 / 256 % 256;

    packet[165]=clients.client[connection]->book_id % 256;
    packet[166]=clients.client[connection]->book_id / 256;
    packet[167]=clients.client[connection]->max_book_time % 256;
    packet[168]=clients.client[connection]->max_book_time / 256;

   //packet[169]=10; summoning lvl

    send(connection, packet, 229, 0);
}

void send_partial_stats(int connection, int attribute_type, int attribute_level){

    unsigned char packet[1024];

    packet[0]=SEND_PARTIAL_STATS;
    packet[1]=6;
    packet[2]=0;
    packet[3]=attribute_type;
    packet[4]=attribute_level % 256;
    packet[5]=attribute_level / 256 % 256;
    packet[6]=attribute_level / 256 / 256 % 256;
    packet[7]=attribute_level / 256 / 256 / 256 % 256;

    send(connection, packet, 8, 0);
}

void process_packet(int connection, unsigned char *packet, struct ev_loop *loop){

    int i=0;

    unsigned char data[1024];
    char text[1024]="";
    char text_out[1024]="";
    char char_name[1024]="";
    char password[1024]="";

    //int char_id=clients.client[connection]->character_id;
    int current_tile=clients.client[connection]->map_tile;

    int other_connection;
    int map_id=clients.client[connection]->map_id;
    //int guild_id=clients.client[connection]->guild_id;
    int protocol=packet[0], lsb=packet[1], msb=packet[2], data_length=lsb+(msb*256)-1;
    int x_dest=0, y_dest=0, tile_dest=0;
    int map_object_id=0;
    int use_with_position=0;
    int result=0;
    int image_id=0;
    int amount=0;
    char receiver_name[80]="";
    int move_to_slot=0, move_from_slot=0, bag_id=0, bag_slot=0, inventory_slot=0;

    // extract data from packet
    for(i=0; i<data_length; i++){
        data[i]=packet[i+3]; // unsigned char array for bit manipulation
        text[i]=packet[i+3]; // signed char array for text manipulation
    }

    text[data_length]='\0';

    /*when the client connects to the server, it will send a SIT_DOWN message if there is a delay in logging in which
    unless trapped, results in a floating point error exception that causes the server to crash.*/
    if(clients.client[connection]->status==CONNECTED && protocol ==SIT_DOWN) return;

/***************************************************************************************************/

    if(protocol==RAW_TEXT) {

        #ifdef DEBUG
        printf("RAW_TEXT [%s]\n", text);
        #endif

        // trim off excess left hand space
        str_trim_left(text);

        if(text[0]=='@'){ // chan chat

            result=process_chat(connection, text);

            if(result==CHAR_NOT_IN_CHAN){
                sprintf(text_out, "%cyou have not joined a channel yet", c_red3+127);
                send_server_text(connection, CHAT_SERVER, text_out);
                return;
            }
            else if(result==CHAN_CHAT_SENT){
                //reserved for debug purposes
                return;
            }

            log_event(EVENT_ERROR, "unknown result from function process_chat");
            return;
        }
        else if(text[0]=='#'){ // hash commands

            result=process_hash_commands(connection, text);

            if(result==HASH_CMD_ABORTED){
                //reserved for debug purposes
                return;
            }

            return;
        }

        //local chat
        sprintf(text_out, "%c%s: %s", c_grey1+127, clients.client[connection]->char_name, text);
        send_raw_text_packet(connection, CHAT_LOCAL, text_out);
        broadcast_local_chat(connection, text_out);
    }
/***************************************************************************************************/

    else if(protocol==MOVE_TO) {

        //returns 2x 2byte integers indicating the x/y axis of the destination

        x_dest=Uint16_to_dec(data[0], data[1]);
        y_dest=Uint16_to_dec(data[2], data[3]);
        tile_dest=x_dest+(y_dest*maps.map[clients.client[connection]->map_id]->map_axis);

        #ifdef DEBUG
        printf("MOVE_TO position x[%i] y[%i] tile[%i]\n", x_dest, y_dest, tile_dest);
        #endif

        start_char_move(connection, tile_dest, loop);
    }
/***************************************************************************************************/

    else if(protocol==SEND_PM) {

        #ifdef DEBUG
        printf("SEND_PM %i %i\n", lsb, msb);
        #endif

        //check that pm packet is properly formed
        if(count_str_island(text)<2) {

            sprintf(text_out, "%cno text in message", c_red1+127);
            send_server_text(connection, CHAT_PERSONAL, text_out);
            return;
        }

        //extract recipients name and message from pm packet
        get_str_island(text, receiver_name, 1);
        get_str_island(text, text_out, 2);

        send_pm(connection, receiver_name, text_out);
    }
/***************************************************************************************************/

    else if(protocol==SIT_DOWN){

        #ifdef DEBUG
        printf("SIT_DOWN %i\n", data[0]);
        #endif

        if(data[0]==1){
            clients.client[connection]->frame=sit_down;
        }
        else clients.client[connection]->frame=stand_up;

        //broadcast to clients
        broadcast_actor_packet(connection, clients.client[connection]->frame, clients.client[connection]->map_tile);

        //save frame to database
        update_db_char_frame(connection);
    }
/***************************************************************************************************/

    else if(protocol==GET_PLAYER_INFO){

        #ifdef DEBUG
        printf("GET_PLAYER_INFO %i %i\n", lsb, msb);
        #endif

        other_connection=Uint32_to_dec(data[0], data[1], data[2], data[3]);

        sprintf(text_out, "You see %s", clients.client[other_connection]->char_name);
        send_server_text(connection, CHAT_SERVER, text_out);
   }
/***************************************************************************************************/

    else if(protocol==SEND_ME_MY_ACTORS){

        #ifdef DEBUG
        printf("SEND_ME_MY_ACTORS %i %i \n", lsb, msb);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==SEND_OPENING_SCREEN){

        #ifdef DEBUG
        printf("SEND OPENING SCREEN %i %i \n", lsb, msb);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==SEND_VERSION){

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

        #ifdef DEBUG
        printf("SEND VERSION lsb [%i] msb [%i] maj [%i] min [%i] version [%i.%i.%i.%i]\n",
               lsb, msb,
               first_digit, second_digit,
               major, minor,
               release,
               patch);

        printf("server host [%i.%i.%i.%i] port [%i]\n", host1, host2, host3, host4, port);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==HEARTBEAT){

        //returns nothing

        #ifdef DEBUG
        printf("HEARTBEAT %i %i \n", lsb, msb);
        #endif

        /* dont need to do anything on this message as the receipt of any data automatically updates the heartbeat
        via the recv_data function in main.c */
    }
/***************************************************************************************************/

    else if(protocol==USE_OBJECT){

        //returns a 4byte integer indicating the threed object id, followed by a 4byte integer indicating ????

        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        use_with_position=Uint32_to_dec(data[4], data[5], data[6], data[7]);

        #ifdef DEBUG
        printf("USE_OBJECT - map object [%i] position [%i]\n", map_object_id, use_with_position);
        #endif

        //if char is moving when protocol arrives, cancel rest of path
        clients.client[connection]->path_count=0;

        //travel from IP to Ravens Isle
        if(map_object_id==520 && clients.client[connection]->map_id==1) move_char_between_maps(connection, 2, 64946);

        //travel from Ravens Isle to IP
        if(map_object_id==5416 && clients.client[connection]->map_id==2) move_char_between_maps(connection, 1, 4053);

        //travel from Ravens Isle to neiva
        if(map_object_id==4986 && clients.client[connection]->map_id==2 && clients.client[connection]->map_tile==108627){
            move_char_between_maps(connection, 3, 3000);
        }
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_INVENTORY_ITEM){

        //returns a Uint8 giving the slot number looked at

        inventory_slot=(int)data[0];
        image_id=clients.client[connection]->client_inventory[inventory_slot].image_id;

        #ifdef DEBUG
        printf("LOOK_AT_INVENTORY_ITEM - slot [%i]\n", inventory_slot);
        #endif

        sprintf(text_out, "%c%s", c_green3+127, item[image_id].item_name);
        send_server_text(connection, CHAT_SERVER, text_out);
    }
/***************************************************************************************************/

    else if(protocol==MOVE_INVENTORY_ITEM){

        //returns 2 Uint8 indicating the slots to be moved from and to
        //if an attempt is made to move to an occupied slot or, to move from an empty slot, the client will automatically block

        move_from_slot=(int)data[0];
        move_to_slot=(int)data[1];

        #ifdef DEBUG
        printf("MOVE_INVENTORY_ITEM - slot [%i] to slot [%i]\n", move_from_slot, move_to_slot);
        #endif

        image_id=clients.client[connection]->client_inventory[move_from_slot].image_id;
        amount=clients.client[connection]->client_inventory[move_from_slot].amount;

        //zero the 'from slot'
        clients.client[connection]->client_inventory[move_from_slot].image_id=0;
        clients.client[connection]->client_inventory[move_from_slot].amount=0;
        send_get_new_inventory_item(connection, 0, 0, move_from_slot);

        //save to database
        update_db_char_slot(connection, move_from_slot);

        //place item in the 'to slot'
        clients.client[connection]->client_inventory[move_to_slot].image_id=image_id;
        clients.client[connection]->client_inventory[move_to_slot].amount=amount;
        send_get_new_inventory_item(connection, image_id, amount, move_to_slot);

        //save to the database
        update_db_char_slot(connection, move_to_slot);
    }
/***************************************************************************************************/

    else if(protocol==HARVEST){

        //returns a integer corresponding to the id of an object in the map 3d object list

        map_object_id=Uint16_to_dec(data[0], data[1]);

        #ifdef DEBUG
        printf("HARVEST object_id %i\n", map_object_id);
        #endif

        start_harvesting2(connection, map_object_id, loop);
    }
/***************************************************************************************************/

    else if(protocol==DROP_ITEM){

        //returns a byte indicating the slot number followed by a 32bit integer indicating the amount to be dropped

        inventory_slot=data[0];
        amount=Uint32_to_dec(data[1], data[2], data[3], data[4]);
        image_id=clients.client[connection]->client_inventory[inventory_slot].image_id;

        #ifdef DEBUG
        printf("DROP_ITEM image_id [%i] drop amount [%i]\n", image_id, amount);
        #endif

        drop_from_inventory(connection, inventory_slot, amount, loop);
    }
/***************************************************************************************************/

    else if(protocol==PICK_UP_ITEM){

        //returns a 4byte integer indicating quantity followed by 1 byte indicating bag slot position

        bag_slot=data[0];
        amount=Uint32_to_dec(data[1], data[2], data[3], data[4]);

        #ifdef DEBUG
        printf("PICK_UP_ITEM lsb [%i] msb [%i] amount [%i] slot [%i]\n", lsb, msb, amount, bag_slot);
        #endif

        pick_up_from_bag(connection, bag_slot, loop);
    }
/***************************************************************************************************/

    else if(protocol==INSPECT_BAG){

        //returns a Unit8 indicating the bag_id

        bag_id=data[0];

        #ifdef DEBUG
        printf("INSPECT_BAG - lsb [%i] msb [%i] bag id [%i]\n", lsb, msb, bag_id);
        #endif

        if(bag_list[bag_id].tile_pos==current_tile) {

            //standing on the bag so we can open it
            send_here_your_ground_items(connection, bag_id);
            clients.client[connection]->bag_open=TRUE;
        }
        else {

            //not standing on bag so move towards it

            //find the bag id in the bag array
            if(bag_exists(map_id, current_tile, &bag_id)==FALSE){

                log_event2(EVENT_ERROR, "bag [%i] does not exist in bag_list", bag_id);
                return;
            }

            start_char_move(connection, bag_list[bag_id].tile_pos, loop);
        }
    }
/***************************************************************************************************/

    else if(protocol==LOOK_AT_MAP_OBJECT){

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

        #ifdef DEBUG
        printf("LOOK_AT_MAP_OBJECT - map object [%i] [%s]\n", map_object_id, item[map_object.image_id].item_name);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==PING_RESPONSE){

        #ifdef DEBUG
        printf("PING_RESPONSE %i %i \n", lsb, msb);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==SET_ACTIVE_CHANNEL){

        #ifdef DEBUG
        printf("SET_ACTIVE_CHANNEL %i %i\n", lsb, msb);
        #endif

        //set the active channel
        clients.client[connection]->active_chan=data[0]-32;

        //update the database
        update_db_char_channels(connection);
    }
/***************************************************************************************************/

    else if(protocol==LOG_IN){

        #ifdef DEBUG
        printf("LOG_IN connection [%i]\n", connection);
        #endif

        process_log_in(connection, text);
    }
/***************************************************************************************************/

    else if(protocol==CREATE_CHAR){

        #ifdef DEBUG
        printf("CREATE_CHAR connection [%i]\n", connection);
        #endif

        //detect and handle malformed packet
        if(count_str_island(text)!=2){

            send_create_char_not_ok(connection);

            sprintf(text_out, "malformed login attempt for new char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_ERROR, text_out);
            return;
        }

        //get the char name and password from the packet
        get_str_island(text, char_name, 1);
        get_str_island(text, password, 2);

        //check if char name is already used
        if(get_char_data_from_db(char_name)==FOUND){

            send_create_char_not_ok(connection);

            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            sprintf(text_out, "Attempt to create new char with existing char name [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);

            return;
        }

        clients.client[connection]->status=LOGGED_IN;

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

        character.char_created=time(NULL);

        //set starting channel
        character.active_chan=0;
        character.chan[0]=1; //nub chan

        //set starting map and tile
        character.map_id=START_MAP_ID;
        character.map_tile=START_MAP_TILE;

        //add character entry to database
        add_char(character);

        //once the char has been added to the database, find its char_id
        clients.client[connection]->character_id=get_max_char_id();

int slot=0;
add_item_to_inventory(connection, 612, 1, &slot);
add_item_to_inventory(connection, 613, 1, &slot);
add_item_to_inventory(connection, 614, 1, &slot);

        //update game data for chars created
        race[character.char_type].char_count++;
        update_db_race_count(character.char_type);

        //update game data for MOTD
        strcpy(game_data.name_last_char_created, char_name);
        game_data.date_last_char_created=character.char_created;

        //notify client that character has been created
        sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        send_create_char_ok(connection);

        //log character creation event
        sprintf(text_out, "[%s] password [%s]\n", char_name, password);
        log_event(EVENT_NEW_CHAR, text_out);
    }
/***************************************************************************************************/

    else if(protocol==GET_DATE){

        #ifdef DEBUG
        printf("GET DATE %i %i \n", lsb, msb);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==GET_TIME){

        #ifdef DEBUG
        printf("GET TIME %i %i \n", lsb, msb);
        #endif
    }
/***************************************************************************************************/

    else if(protocol==SERVER_STATS){

        #ifdef DEBUG
        printf("SERVER_STATS %i %i \n", lsb, msb);
        #endif

        send_motd_header(connection);
    }
/***************************************************************************************************/

    else {

        // catch unknown protocols
        sprintf(text_out, "unknown protocol [%i]\n", protocol);
        log_event(EVENT_ERROR, text_out);
    }
}
