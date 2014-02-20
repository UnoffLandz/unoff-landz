#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for usec time
#include <time.h>
#include <unistd.h>

#include "global.h"
#include "string_functions.h"
#include "numeric_functions.h"
#include "protocol.h"
#include "hash_commands.h"
#include "broadcast.h"
#include "files.h"
#include "character_movement.h" //needed for get_move_command_vector
#include "chat.h"
#include "debug.h"
#include "maps.h"
#include "motd.h"
#include "pathfinding.h"
#include "chat.h"
#include "log_in.h"
#include "database.h"

void send_here_your_inventory(int connection){

    int i=0;
    unsigned char packet[12];
    int char_id=clients.client[connection]->character_id;
    int inventory_count=characters.character[char_id]->inventory[0];

    int data_length=1+(8*inventory_count);

    packet[0]=HERE_YOUR_INVENTORY;
    packet[1]=(data_length+1) % 256;
    packet[2]=(data_length+1) / 256;

    for(i=0; i<data_length; i++){
        packet[3+i]=characters.character[char_id]->inventory[i];
    }

    send(connection, packet, data_length+2,0);
}

void send_server_text(int sock, int channel, char *text){

    /* function definition sends to sock rather than connection so that we can reach clients when a connection has not been
    allocated. This happens when a client tries to connect after the maximum number of connections has been exceeded, in
    which case, we need to be able to send a message to that client telling it to wait until a connection becomes available.
    See the 'find free connection' block in main.c */

    unsigned char packet[1024];
    int text_length;
    int message_length;
    int packet_length;

    text_length=strlen(text);
    message_length=text_length+2; /* add 1 for the channel byte and 1 for the msb byte */

    packet[0]=0;
    packet[1]=message_length % 256;
    packet[2]=message_length / 256;
    packet[3]=channel;

    memmove(packet+4, text, text_length);

    packet_length=message_length+2; /* add 1 for the protocol byte and 1 for the lsb byte */

    send(sock, packet, packet_length, 0);
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

void send_change_map(int connection, char *elm_filename){

    unsigned char packet[1024];

    int i;

    /* calculate msb/lsb */
    int filename_length=strlen(elm_filename)+1; // +1 to include null terminator
    int msb=(filename_length) / 256;
    int lsb=(filename_length) % 256;
    lsb++; // +1 as required by EL protocol

    // calculate packet length
    int packet_length=filename_length+3;

    // construct packet header
    packet[0]=CHANGE_MAP;
    packet[1]=lsb;
    packet[2]=msb;

    // TODO (derekl#2#): convert loop to memcpy    // add packet content
    for(i=3; i<3+filename_length; i++){
        packet[i]=elm_filename[i-3];
    }

    send(connection, packet, packet_length, 0);
}

void send_here_your_stats(int connection){

    unsigned char packet[1024];
    int char_id=clients.client[connection]->character_id;

    int i=0;

    packet[0]=HERE_YOUR_STATS;
    packet[1]=227;
    packet[2]=0;

    for(i=3; i<230; i++){
        packet[i]=0;
    }

    packet[3]=characters.character[char_id]->physique % 256;
    packet[4]=characters.character[char_id]->physique / 256;
    packet[5]=characters.character[char_id]->max_physique % 256;
    packet[6]=characters.character[char_id]->max_physique / 256;
    packet[7]=characters.character[char_id]->coordination % 256;
    packet[8]=characters.character[char_id]->coordination / 256;
    packet[9]=characters.character[char_id]->max_coordination % 256;
    packet[10]=characters.character[char_id]->max_coordination / 256;
    packet[11]=characters.character[char_id]->reasoning % 256;
    packet[12]=characters.character[char_id]->reasoning / 256;
    packet[13]=characters.character[char_id]->max_reasoning % 256;
    packet[14]=characters.character[char_id]->max_reasoning / 256;
    packet[15]=characters.character[char_id]->will % 256;
    packet[16]=characters.character[char_id]->will / 256;
    packet[17]=characters.character[char_id]->max_will % 256;
    packet[18]=characters.character[char_id]->max_will / 256;
    packet[19]=characters.character[char_id]->instinct % 256;
    packet[20]=characters.character[char_id]->instinct / 256;
    packet[21]=characters.character[char_id]->max_instinct % 256;
    packet[22]=characters.character[char_id]->max_instinct / 256;
    packet[23]=characters.character[char_id]->vitality % 256;
    packet[24]=characters.character[char_id]->vitality / 256;
    packet[25]=characters.character[char_id]->max_vitality % 256;
    packet[26]=characters.character[char_id]->max_vitality / 256;

    packet[27]=characters.character[char_id]->human % 256;
    packet[28]=characters.character[char_id]->human / 256;
    packet[29]=characters.character[char_id]->max_human % 256;
    packet[30]=characters.character[char_id]->max_human / 256;
    packet[31]=characters.character[char_id]->animal % 256;
    packet[32]=characters.character[char_id]->animal / 256;
    packet[33]=characters.character[char_id]->max_animal % 256;
    packet[34]=characters.character[char_id]->max_animal / 256;
    packet[35]=characters.character[char_id]->vegetal % 256;
    packet[36]=characters.character[char_id]->vegetal / 256;
    packet[37]=characters.character[char_id]->max_vegetal % 256;
    packet[38]=characters.character[char_id]->max_vegetal / 256;
    packet[39]=characters.character[char_id]->inorganic % 256;
    packet[40]=characters.character[char_id]->inorganic / 256;
    packet[41]=characters.character[char_id]->max_inorganic % 256;
    packet[42]=characters.character[char_id]->max_inorganic / 256;
    packet[43]=characters.character[char_id]->artificial % 256;
    packet[44]=characters.character[char_id]->artificial / 256;
    packet[45]=characters.character[char_id]->max_artificial % 256;
    packet[46]=characters.character[char_id]->max_artificial / 256;
    packet[47]=characters.character[char_id]->magic % 256;
    packet[48]=characters.character[char_id]->magic / 256;
    packet[49]=characters.character[char_id]->max_magic % 256;
    packet[50]=characters.character[char_id]->max_magic / 256;

    packet[51]=characters.character[char_id]->manufacturing_lvl % 256;
    packet[52]=characters.character[char_id]->manufacturing_lvl / 256;
    packet[53]=characters.character[char_id]->max_manufacturing_lvl % 256;
    packet[54]=characters.character[char_id]->max_manufacturing_lvl / 256;
    packet[55]=characters.character[char_id]->harvest_lvl % 256;
    packet[56]=characters.character[char_id]->harvest_lvl / 256;
    packet[57]=characters.character[char_id]->max_harvest_lvl % 256;
    packet[58]=characters.character[char_id]->max_harvest_lvl / 256;
    packet[59]=characters.character[char_id]->alchemy_lvl % 256;
    packet[60]=characters.character[char_id]->alchemy_lvl / 256;
    packet[61]=characters.character[char_id]->max_alchemy_lvl % 256;
    packet[62]=characters.character[char_id]->max_alchemy_lvl / 256;
    packet[63]=characters.character[char_id]->overall_lvl % 256;
    packet[64]=characters.character[char_id]->overall_lvl / 256;
    packet[65]=characters.character[char_id]->max_overall_lvl % 256;
    packet[66]=characters.character[char_id]->max_overall_lvl / 256;
    packet[67]=characters.character[char_id]->attack_lvl % 256;
    packet[68]=characters.character[char_id]->attack_lvl / 256;
    packet[69]=characters.character[char_id]->max_attack_lvl % 256;
    packet[70]=characters.character[char_id]->max_attack_lvl / 256;
    packet[71]=characters.character[char_id]->defence_lvl % 256;
    packet[72]=characters.character[char_id]->defence_lvl / 256;
    packet[73]=characters.character[char_id]->max_defence_lvl % 256;
    packet[74]=characters.character[char_id]->max_defence_lvl / 256;
    packet[75]=characters.character[char_id]->magic_lvl % 256;
    packet[76]=characters.character[char_id]->magic_lvl / 256;
    packet[77]=characters.character[char_id]->max_magic_lvl % 256;
    packet[78]=characters.character[char_id]->max_magic_lvl / 256;
    packet[79]=characters.character[char_id]->potion_lvl % 256;
    packet[80]=characters.character[char_id]->potion_lvl / 256;
    packet[81]=characters.character[char_id]->max_potion_lvl % 256;
    packet[82]=characters.character[char_id]->max_potion_lvl / 256;

    //packet[83]=; Unused
    //packet[84]=; Unused
    //packet[85]=; Unused
    //packet[86]=; Unused

    packet[87]=characters.character[char_id]->material_pts % 256;
    packet[88]=characters.character[char_id]->material_pts / 256;
    packet[89]=characters.character[char_id]->max_material_pts % 256;
    packet[90]=characters.character[char_id]->max_material_pts / 256;

    packet[91]=characters.character[char_id]->ethereal_pts % 256;
    packet[92]=characters.character[char_id]->ethereal_pts / 256;
    packet[93]=characters.character[char_id]->max_ethereal_pts % 256;
    packet[94]=characters.character[char_id]->max_ethereal_pts / 256;

    packet[95]=characters.character[char_id]->food_lvl % 256;
    packet[96]=characters.character[char_id]->food_lvl / 256;
    packet[97]=characters.character[char_id]->elapsed_book_time % 256;
    packet[98]=characters.character[char_id]->elapsed_book_time / 256;

    //packet[99]=; Unused
    //packet[100]=; Unused

    packet[101]=characters.character[char_id]->manufacture_exp % 256;
    packet[102]=characters.character[char_id]->manufacture_exp / 256 % 256;
    packet[103]=characters.character[char_id]->manufacture_exp / 256 / 256 % 256;
    packet[104]=characters.character[char_id]->manufacture_exp / 256 / 256 / 256 % 256;
    packet[105]=characters.character[char_id]->max_manufacture_exp % 256;
    packet[106]=characters.character[char_id]->max_manufacture_exp / 256 % 256;
    packet[107]=characters.character[char_id]->max_manufacture_exp / 256 / 256 % 256;
    packet[108]=characters.character[char_id]->max_manufacture_exp / 256 / 256 / 256 % 256;
    packet[109]=characters.character[char_id]->harvest_exp % 256;
    packet[110]=characters.character[char_id]->harvest_exp / 256 % 256;
    packet[111]=characters.character[char_id]->harvest_exp / 256 / 256 % 256;
    packet[112]=characters.character[char_id]->harvest_exp / 256 / 256 / 256 % 256 ;
    packet[113]=characters.character[char_id]->max_harvest_exp % 256;
    packet[114]=characters.character[char_id]->max_harvest_exp / 256 % 256;
    packet[115]=characters.character[char_id]->max_harvest_exp / 256 / 256 % 256;
    packet[116]=characters.character[char_id]->max_harvest_exp / 256 / 256 / 256 % 256;
    packet[117]=characters.character[char_id]->alchemy_exp % 256;
    packet[118]=characters.character[char_id]->alchemy_exp / 256 % 256;
    packet[119]=characters.character[char_id]->alchemy_exp / 256 / 256 % 256;
    packet[120]=characters.character[char_id]->alchemy_exp / 256 / 256 / 256 % 256;
    packet[121]=characters.character[char_id]->max_alchemy_exp % 256;
    packet[122]=characters.character[char_id]->max_alchemy_exp / 256 % 256;
    packet[123]=characters.character[char_id]->max_alchemy_exp / 256 / 256 % 256;
    packet[124]=characters.character[char_id]->max_alchemy_exp / 256 / 256 / 256 % 256;
    packet[125]=characters.character[char_id]->overall_exp % 256;
    packet[126]=characters.character[char_id]->overall_exp / 256 % 256;
    packet[127]=characters.character[char_id]->overall_exp / 256 / 256 % 256;
    packet[128]=characters.character[char_id]->overall_exp / 256 / 256 / 256 % 256;
    packet[129]=characters.character[char_id]->max_overall_exp % 256;
    packet[130]=characters.character[char_id]->max_overall_exp / 256 % 256;
    packet[131]=characters.character[char_id]->max_overall_exp / 256 / 256 % 256;
    packet[132]=characters.character[char_id]->max_overall_exp / 256 / 256 / 256 % 256;
    packet[133]=characters.character[char_id]->attack_exp % 256;
    packet[134]=characters.character[char_id]->attack_exp / 256 % 256;
    packet[135]=characters.character[char_id]->attack_exp / 256 / 256 % 256;
    packet[136]=characters.character[char_id]->attack_exp / 256 / 256 / 256 % 256;
    packet[137]=characters.character[char_id]->max_attack_exp % 256;
    packet[138]=characters.character[char_id]->max_attack_exp / 256 % 256;
    packet[139]=characters.character[char_id]->max_attack_exp / 256 / 256 % 256;
    packet[140]=characters.character[char_id]->max_attack_exp / 256 / 256 / 256 % 256;
    packet[141]=characters.character[char_id]->defence_exp % 256;
    packet[142]=characters.character[char_id]->defence_exp / 256 % 256;
    packet[143]=characters.character[char_id]->defence_exp / 256 / 256 % 256;
    packet[144]=characters.character[char_id]->defence_exp / 256 / 256 / 256 % 256;
    packet[145]=characters.character[char_id]->max_defence_exp % 256;
    packet[146]=characters.character[char_id]->max_defence_exp / 256 % 256;
    packet[147]=characters.character[char_id]->max_defence_exp / 256 / 256 % 256;
    packet[148]=characters.character[char_id]->max_defence_exp / 256 / 256 / 256 % 256;
    packet[149]=characters.character[char_id]->magic_exp % 256;
    packet[150]=characters.character[char_id]->magic_exp / 256 % 256;
    packet[151]=characters.character[char_id]->magic_exp / 256 / 256 % 256;
    packet[152]=characters.character[char_id]->magic_exp / 256 / 256 / 256 % 256;
    packet[153]=characters.character[char_id]->max_magic_exp % 256;
    packet[154]=characters.character[char_id]->max_magic_exp / 256 % 256;
    packet[155]=characters.character[char_id]->max_magic_exp / 256 / 256 % 256;
    packet[156]=characters.character[char_id]->max_magic_exp / 256 / 256 / 256 % 256;
    packet[157]=characters.character[char_id]->potion_exp % 256;
    packet[158]=characters.character[char_id]->potion_exp / 256 % 256;
    packet[159]=characters.character[char_id]->potion_exp / 256 / 256 % 256;
    packet[160]=characters.character[char_id]->potion_exp / 256 / 256 / 256 % 256;
    packet[161]=characters.character[char_id]->max_potion_exp % 256;
    packet[162]=characters.character[char_id]->max_potion_exp / 256 % 256;
    packet[163]=characters.character[char_id]->max_potion_exp / 256 / 256 % 256;
    packet[164]=characters.character[char_id]->max_potion_exp / 256 / 256 / 256 % 256;

    packet[165]=characters.character[char_id]->book_id % 256;
    packet[166]=characters.character[char_id]->book_id / 256;
    packet[167]=characters.character[char_id]->max_book_time % 256;
    packet[168]=characters.character[char_id]->max_book_time / 256;

   //packet[169]=10; summoning lvl

    send(connection, packet, 229, 0);
}

void send_partial_stats(int connection, int attribute_type, int attribute_level){

    unsigned char packet[1024];

    // construct packet header
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

void send_actors_to_client(int connection){

    /** RESULT  : make other actors in proximity visible to this actor

       RETURNS : void

       PURPOSE : ensures our actor can see other actors after log on or a map jump

       USAGE   : protocol.c process_packet
    */

    int i;
    unsigned char packet[1024];
    int packet_length;
    int map_id=clients.client[connection]->map_id;
    int char_tile=clients.client[connection]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;
    int char_visual_proximity=clients.client[connection]->visual_proximity;

    int other_client_id;
    int other_char_tile;

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        other_client_id=maps.map[map_id]->client_list[i];
        other_char_tile=clients.client[other_client_id]->map_tile;

        // restrict to characters other than self
        if(connection!=other_client_id){

            //restrict to characters within visual proximity
            if(get_proximity(char_tile, other_char_tile, map_axis)<char_visual_proximity){

                add_new_enhanced_actor_packet(clients.client[connection]->character_id, packet, &packet_length);//0
                send(connection, packet, packet_length, 0);
            }
        }
    }
}

void process_packet(int connection, unsigned char *packet){

    int i=0;

    unsigned char data[1024];
    char text[1024]="";

    char text_out[1024]="";

    char char_name[1024]="";
    char password[1024]="";

    int char_id;
    int current_tile=clients.client[connection]->map_tile;
    int other_connection;
    int map_id=clients.client[connection]->map_id;
    int guild_id=clients.client[connection]->guild_id;
    int protocol=packet[0];
    int lsb=packet[1];
    int msb=packet[2];
    int data_length=lsb+(msb*256)-1;
    int x_dest=0, y_dest=0, tile_dest=0;
    int map_object_id=0;
    int use_with_position=0;
    int result=0;
    int item=0;

    // extract data from packet
    for(i=0; i<data_length; i++){
        data[i]=packet[i+3]; // unsigned char array for bit manipulation
        text[i]=packet[i+3]; // signed char array for text manipulation
    }

    text[data_length]='\0';
    int text_len=strlen(text);

    switch(protocol){

/*
        case RAW_TEXT:

            printf("RAW_TEXT [%s]\n", text);

            // trim off excess left hand space
            str_trim_left(text);

            switch(text[0]){

                case '@': // chat

                    switch(process_chat(connection, text)){

                        case CHAR_NOT_IN_CHAN:
                            sprintf(text_out, "%cyou have not joined a channel yet", c_red3+127);
                            send_server_text(connection, CHAT_SERVER, text_out);
                        break;

                        case CHAN_CHAT_SENT:
                            //this case is reserved for debug purposes
                        break;

                        default:
                            log_event(EVENT_ERROR, "unknown result from function process_chat");
                        break;
                    }
                break;

                case '#':// hash commands

                    switch(process_hash_commands(connection, text, text_len)){

                        case HASH_CMD_UNSUPPORTED:
                            //this case is reserved for debug purposes
                            return;
                        break;

                        case HASH_CMD_UNKNOWN:
                            sprintf(text_out, "%cThat command isn't supported yet. You may want to tell the game administrator", c_red3+127);
                            send_server_text(connection, CHAT_SERVER, text_out);
                            return;
                        break;

                        case HASH_CMD_EXECUTED:
                             //this case is reserved for debug purposes
                             return;
                        break;

                        case HASH_CMD_ABORTED:
                            // debug purposes
                            return;
                        break;

                        case HASH_CMD_FAILED:
                             //this case is reserved for debug purposes
                            return;
                        break;

                        default:
                            sprintf(text_out, "unknown result from function process_hash_command [%s]", text);
                            log_event(EVENT_ERROR, text_out);
                        return;
                    }
                break;

                default://local chat
                    sprintf(text_out, "%c%s: %s", c_grey1+127, clients.client[connection]->char_name, text);

                    //echo to sender
                    send_raw_text_packet(connection, CHAT_LOCAL, text_out);

                    //broadcast to receivers
                    broadcast_local_chat(connection, text_out);
                    return;
                break;
            }

        break;
*/
        case MOVE_TO:

        //if char is harvesting then stop
        if(clients.client[connection]->harvest_flag==TRUE){

            sprintf(text_out, "%cYou stopped harvesting. %s", c_red3+127, harvestables[item].name);
            send_server_text(connection, CHAT_SERVER, text_out);

            clients.client[connection]->harvest_flag=FALSE;
            break;
        }

        //if char is sitting then stand before moving
        if(clients.client[connection]->frame==13){
            clients.client[connection]->frame=14;
            broadcast_actor_packet(connection, clients.client[connection]->frame, clients.client[connection]->map_tile);
        }

        //calculate destination
        x_dest=Uint16_to_dec(data[0], data[1]);
        y_dest=Uint16_to_dec(data[2], data[3]);
        tile_dest=x_dest+(y_dest*maps.map[clients.client[connection]->map_id]->map_axis);

        printf("MOVE_TO position x[%i] y[%i] tile[%i]\n", x_dest, y_dest, tile_dest);

        if(maps.map[map_id]->height_map[tile_dest]<MIN_TRAVERSABLE_VALUE){
            sprintf(text_out, "%cThe tile you clicked on can't be walked on", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            break;
        }

        if(current_tile!=tile_dest){

            result=get_astar_path(connection, current_tile, tile_dest);

            if(result==ASTAR_UNREACHABLE) {
                printf("path unreachable ???\n");
                exit(EXIT_FAILURE);
            }

            if(result==ASTAR_ABORT) {
                printf("path aborted ???\n");
                exit(EXIT_FAILURE);
            }

            if(result==ASTAR_UNKNOWN) {
                printf("path unknown\n");
                exit(EXIT_FAILURE);
            }

            printf("got new path\n");
            for(i=0; i<clients.client[connection]->path_count; i++){
                printf("%i %i\n", i, clients.client[connection]->path[i]);
            }

            //reset time of last move to zero so the movement is processed without delay
            clients.client[connection]->time_of_last_move=0;
            break;
        }

        printf("current tile = destination (ignored)\n");

        break;

        case SEND_PM:
        printf("SEND_PM %i %i\n", lsb, msb);
        send_pm(connection, text);
        break;

        case SIT_DOWN:
        printf("SIT_DOWN %i\n", data[0]);

        if(data[0]==0) {
            printf("stand\n");
            //stand
            clients.client[connection]->frame=14;
            broadcast_actor_packet(connection, clients.client[connection]->frame, clients.client[connection]->map_tile);
        }

        if(data[0]==1) {
            printf("sit\n");
            //sit
            clients.client[connection]->frame=13;
            broadcast_actor_packet(connection, clients.client[connection]->frame, clients.client[connection]->map_tile);
        }

        //save_character(characters.character[char_id]->char_name, char_id);

        break;

        case GET_PLAYER_INFO:
        printf("GET_PLAYER_INFO %i %i\n", lsb, msb);
        other_connection=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        sprintf(text_out, "You see %s", clients.client[other_connection]->char_name);
        send_server_text(connection, CHAT_SERVER, text_out);
        break;

        case SEND_ME_MY_ACTORS:
        printf("SEND_ME_MY_ACTORS %i %i \n", lsb, msb);
        break;

        case SEND_OPENING_SCREEN:
        printf("SEND OPENING SCREEN %i %i \n", lsb, msb);
        break;

        case SEND_VERSION:
        printf("SEND VERSION %i %i \n", lsb, msb);
        break;

        case HEARTBEAT:
        printf("HEARTBEAT %i %i \n", lsb, msb);

        gettimeofday(&time_check, NULL);
        clients.client[i]->time_of_last_heartbeat=time_check.tv_sec;
        //save_data(connection);//needs to be on a separate timer
        break;

        case USE_OBJECT:

        printf("USE_OBJECT %i %i \n", lsb, msb);

        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        use_with_position=Uint32_to_dec(data[4], data[5], data[6], data[7]);
        printf("map object id %i use with position %i\n", map_object_id, use_with_position);

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
        break;

        case HARVEST:

        item=Uint16_to_dec(data[0], data[1]);

        printf("HARVEST item %i\n", item);

        if(harvestables[item].exp==0){
            sprintf(text_out, "%cYou tried to harvest an unknown item", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            break;
        }

        if(clients.client[connection]->harvest_flag==TRUE){

            sprintf(text_out, "%cYou stopped harvesting. %s", c_red3+127, harvestables[item].name);
            send_server_text(connection, CHAT_SERVER, text_out);

            clients.client[connection]->harvest_flag=FALSE;
            break;
        }

        clients.client[connection]->harvest_item=item;
        clients.client[connection]->harvest_flag=TRUE;

        sprintf(text_out, "%cYou started to harvest %s", c_green3+127, harvestables[item].name);
        send_server_text(connection, CHAT_SERVER, text_out);

        break;

        case PING_RESPONSE:
        printf("PING_RESPONSE %i %i \n", lsb, msb);
        break;

        case SET_ACTIVE_CHANNEL:
        printf("SET_ACTIVE_CHANNEL %i %i\n", lsb, msb);
        clients.client[connection]->active_chan=data[0]-32;
        break;

        case LOG_IN:
        printf("LOG_IN connection [%i]\n", connection);
        process_log_in(connection, text);
        break;

        case CREATE_CHAR:

        printf("CREATE_CHAR connection [%i]\n", connection);

        if(count_str_island(text)!=2){

            send_create_char_not_ok(connection);

            printf("create char not ok\n");

            sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            sprintf(text_out, "malformed login attempt for new char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_ERROR, text_out);
            return;
        }

        //get the char name and password from the packet
        get_str_island(text, char_name, 1);
        get_str_island(text, password, 2);

        //check if char name is already used
        if(get_char_id(char_name)==1) {

            send_create_char_not_ok(connection);

            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            sprintf(text_out, "Attempt to create new char with existing char name [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);

            return;
        }
/*
        // check if we'll exceed maximum number of characters
        if(characters.count+1==characters.max){
            send_create_char_not_ok(connection);
            sprintf(text_out, "%cSorry, but the maximum number of characters on the server has been reached", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            sprintf(text_out, "maximum number of characters on server has been reached\n");
            log_event(EVENT_ERROR, text_out);
            return;
        }
*/
        //create new character id
        char_id=get_max_char_id();
        if(char_id==0) char_id=1; else char_id++;

        clients.client[connection]->character_id=char_id;
        clients.client[connection]->status=LOGGED_IN;

        //copy data to the struct which will be passed to the database
        new_character.char_id=char_id;
        strcpy(new_character.char_name, char_name);
        strcpy(new_character.password, password);

        i=strlen(char_name)+strlen(password)+2;
        new_character.skin_type=data[i++];
        new_character.hair_type=data[i++];
        new_character.shirt_type=data[i++];
        new_character.pants_type=data[i++];
        new_character.boots_type=data[i++];
        new_character.char_type=data[i++];
        new_character.head_type=data[i++];

        new_character.visual_proximity=15;
        new_character.local_text_proximity=12;
        new_character.char_created=time(NULL);

        add_char(new_character);

        sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        send_create_char_ok(connection);

        sprintf(text_out, "[%s] password [%s]\n", char_name, password);
        log_event(EVENT_NEW_CHAR, text_out);

        break;

        case GET_DATE:
        printf("GET DATE %i %i \n", lsb, msb);
        break;

        case GET_TIME:
        printf("GET TIME %i %i \n", lsb, msb);
        break;

        case SERVER_STATS:
        printf("SERVER_STATS %i %i \n", lsb, msb);
        send_motd_header(connection);
        break;

        default: // UNKNOWN
        printf("UNKNOWN PROTOCOL %i %i %i \n", protocol, lsb, msb);
        sprintf(text_out, "unknown protocol [%i]\n", protocol);
        log_event(EVENT_ERROR, text_out);
        break;
    }

}
