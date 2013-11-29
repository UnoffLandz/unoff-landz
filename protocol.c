#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //needed for send function
#include <sys/time.h> //needed for usec time

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
//#include "files.h"

/*
void send_here_your_inventory(int sock){

    unsigned char packet[12];

    packet[0]=19;
    packet[1]=10;
    packet[2]=0;

    packet[3]=0; //number of items
    packet[4]=0; //item id lsb
    packet[5]=0; //item id msb
    packet[6]=0; //quantity lsb
    packet[7]=0; //quantity msb
    packet[8]=0; //quantity lsb2
    packet[9]=0; //quantity msb2
    packet[10]=0; //position in inventory
    packet[11]=0; //flag

    send(sock, packet, 12, 0);
}
*/

void send_get_active_channels(int sock, int active_chan, int chan1, int chan2, int chan3){

    unsigned char packet[1024];

    packet[0]=71;
    packet[1]=14;
    packet[2]=0;
    packet[3]=active_chan;

    if(chan1>0) {
        packet[4]=chan1 % 256;
        packet[5]=chan1/256 % 256;
        packet[6]=chan1/256/256 % 256;
        packet[7]=chan1/256/256/256 % 256;
    }
    else {
        packet[4]=0;
        packet[5]=0;
        packet[6]=0;
        packet[7]=0;
    }

    if(chan2>0) {
        packet[8]=chan2 % 256;
        packet[9]=chan2/256 % 256;
        packet[10]=chan2/256/256 % 256;
        packet[11]=chan2/256/256/256 % 256;
    }
    else {
        packet[8]=0;
        packet[9]=0;
        packet[10]=0;
        packet[11]=0;
    }

    if(chan3>0) {
        packet[12]=chan3 % 256;
        packet[13]=chan3/256 % 256;
        packet[14]=chan3/256/256 % 256;
        packet[15]=chan3/256/256/256 % 256;
    }
    else {
        packet[12]=0;
        packet[13]=0;
        packet[14]=0;
        packet[15]=0;
    }

    send(sock, packet, 16, 0);
}

void send_server_text(int sock, int channel, char *text){

/* function definition sends to sock rather than connection so that we can reach clients when a connection has not been allocated
   This happens when a client tries to connect after the maximum number of connections has been exceeded, in which case, we need
   to be able to send a message to that client telling it to wait until a connection becomes available. See the 'find free connection'
   block in main.c */

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

int get_char_connection(char char_id){

    int i=0;

    /* loop through client list */
    for(i=0; i<clients.count; i++){

        /* match username with an existing user */
        if(char_id==clients.client[i]->character_id){
            return i;
        }

    }

    /* return -1 to show when char is not in-game */
    return -1;

}

int get_char_id(char *char_name){

    int i=0;

    // loop through character list
    for(i=0; i<characters.count; i++){

        //printf("Searching character list [%s]\n", characters.character[i]->char_name);

        // match char_name with an existing char
        if(strcmp(char_name, characters.character[i]->char_name)==0) {
            //printf("found character name [%s]\n", characters.character[i]->char_name);
            return i;
        }
     }

    return CHARNAME_NOT_FOUND;

}

int validate_password(char *char_name, char *password){

    int char_id=get_char_id(char_name);

    if(char_id>=0) {
        if(strcmp(password, characters.character[char_id]->password)==0) {
            return char_id;
        }
        else {
            return WRONG_PASSWORD;
        }
    }

    return CHARNAME_NOT_FOUND;
}

void send_pm(int connection, char *text) {

    char msg[1024]="";
    char text_out[1024]="";
    int receiver_id=0;
    int receiver_connection=0;
    char sender_name[1024]="";
    char receiver_name[1024]="";

    if(count_str_island(text)>=2) {

        get_str_island(text, receiver_name, 1);
        get_str_island(text, msg, 2);

        strcpy(sender_name, characters.character[clients.client[connection]->character_id]->char_name);

        printf("PM from [%s] to [%s]: %s\n", receiver_name, sender_name, msg);

        // echo message back to sender
        sprintf(text_out, "%c[PM to %s: %s]", c_orange1+127, receiver_name, msg);
        send_server_text(clients.client[receiver_connection]->sock, CHAT_PERSONAL, text_out);

        // determine id of the receiver char
        receiver_id=get_char_id(receiver_name);

        if(receiver_id>=0) {
            // determine if the receiver char exists by seeing if it has a connection
            receiver_connection=get_char_connection(receiver_id);

            if(receiver_connection!=-1) {
                // receiver char is in-game
                sprintf(text_out, "%c[PM from %s: %s]", c_orange1+127, sender_name, msg);
                send_server_text(clients.client[connection]->sock, CHAT_PERSONAL, text_out);
            }
            else {
                // receiver char is not in-game
                sprintf(text_out, "%ccharacter is not currently logged on. To send a letter use #LETTER [name] [message]", c_red2+127);
                send_server_text(clients.client[connection]->sock, CHAT_PERSONAL, text_out);
                printf("SEND_PM from [%s] to [%s] message [%s] - receiver not in-game\n", sender_name, receiver_name, msg);
            }
        }
        else {

            // receiver char does not exist
            sprintf(text_out, "%ccharacter does not exist\n", c_red1+127);
            send_server_text(clients.client[connection]->sock, CHAT_PERSONAL, text_out);
            printf("SEND_PM from [%s] to [%s] message [%s] - receiver does not exist\n", sender_name, receiver_name, msg);
        }
    }
    else {
        // tried to send a zero length message
        sprintf(text_out, "%cno text in message", c_red1+127);
        send_server_text(clients.client[connection]->sock, CHAT_PERSONAL, text_out);
        printf("SEND_PM from [%s] to [%s] - zero length message\n", sender_name, receiver_name);
    }

}

void send_login_ok(int sock){

    unsigned char packet[3];

    packet[0]=250;
    packet[1]=1;
    packet[2]=0;

    send(sock, packet, 3, 0);
}

void send_login_not_ok(int sock){

    unsigned char packet[3];

    packet[0]=251;
    packet[1]=1;
    packet[2]=0;

    send(sock, packet, 3, 0);
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

void send_you_dont_exist(int sock){

    unsigned char packet[3];

    packet[0]=249;
    packet[1]=1;
    packet[2]=0;

    send(sock, packet, 3, 0);
}

void send_you_are(int sock, int id){

    unsigned char packet[5];
    int id_msb=id / 256;
    int id_lsb=id % 256;

    packet[0]=3;
    packet[1]=3;
    packet[2]=0;
    packet[3]=id_lsb;
    packet[4]=id_msb;

    send(sock, packet, 5, 0);
}

void send_change_map(int sock, char *elm_filename){

    unsigned char packet[1024];

    int i;

    /* calculate msb/lsb */
    int filename_length=strlen(elm_filename)+1; /* +1 to include null terminator */
    int msb=(filename_length) / 256;
    int lsb=(filename_length) % 256;
    lsb++; /* +1 as required by EL protocol */

    /* calculate packet length */
    int packet_length=filename_length+3;

    /* construct packet header */
    packet[0]=7;
    packet[1]=lsb;
    packet[2]=msb;

    // TODO (derekl#2#): convert loop to memcpy
    for(i=3; i<3+filename_length; i++){
        packet[i]=elm_filename[i-3];
    }

    send(sock, packet, packet_length, 0);
}

void send_actors_to_client(int connection){

    int i;
    unsigned char packet[1024];
    int packet_length;
    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;
    int char_visual_proximity=characters.character[char_id]->visual_proximity;

    int other_char_id=0;
    int other_char_tile=0;

    for(i=0; i<clients.max; i++){

        // restrict to clients that are logged in
        if(clients.client[i]->status==LOGGED_IN) {

            other_char_id=clients.client[i]->character_id;
            other_char_tile=characters.character[other_char_id]->map_tile;

            /* restrict to characters on the same map */
            if(characters.character[other_char_id]->map_id==map_id){

                printf("add %s char to client %i\n", characters.character[other_char_id]->char_name, connection);

                // restrict to characters other than self
                if(connection!=i){

                     //restrict to characters within visual proximity
                    if(get_proximity(char_tile, other_char_tile, map_axis)<char_visual_proximity){

                        add_new_enhanced_actor_packet(clients.client[i]->character_id, packet, &packet_length);
                        send(clients.client[connection]->sock, packet, packet_length, 0);
                    }
                }
            }
        }
    }
}

void process_packet(int connection, unsigned char *packet){

    int i=0;

    unsigned char data[1024];
    char text[1024]="";
    char text_out[1024];

    char char_name[1024]="";
    char password[1024]="";

    int id;
    int channel_number=0;
    int chan_colour=0;
    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int sock=clients.client[connection]->sock;
    int protocol=packet[0];
    int lsb=packet[1];
    int msb=packet[2];
    int data_length=lsb+(msb*256)-1;
    int x_dest=0, y_dest=0, tile_dest=0;

    // extract data from packet
    for(i=0; i<data_length; i++){
        data[i]=packet[i+3]; // unsigned char array for bit manipulation
        text[i]=packet[i+3]; // signed char array for text manipulation
    }
    text[data_length]='\0';
    int text_len=strlen(text);

    switch(protocol){

        case RAW_TEXT:

            // trim off excess left hand space
            str_trim_left(text);

            //debug_raw_text(connection, text, text_len);

            switch(text[0]){

                case '@': // chat
                    //remove @ ------- need to move this to process_chat function
                    memcpy(text, text+1, text_len-1);
                    text[text_len-1]='\0';

                    switch(process_chat(connection, text)){

                        case -1:
                            sprintf(text_out, "%cyou have not joined a channel yet", c_red3+127);
                            send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);
                        break;

                        default:
                            //log_event(EVENT_ERROR, "unknown result from function process_chat");
                        break;
                    }
                break;

                case '#':// hash commands

                    switch(process_hash_commands(connection, text, text_len)){

                        case HASH_CMD_UNSUPPORTED:
                            //for debug
                        break;

                        case HASH_CMD_UNKNOWN:
                            sprintf(text_out, "%cThat command isn't supported yet. You may want to tell the game administrator", c_red3+127);
                            send_server_text(clients.client[connection]->sock, CHAT_MOD, text_out);
                        break;

                        case HASH_CMD_EXECUTED:
                            //for debug
                        break;

                        case HASH_CMD_FAILED:
                            //for debug
                        break;

                        default:
                            log_event(EVENT_ERROR, "unknown result from function process_hash_command");
                        break;
                    }

                break;

                default://local chat
                    sprintf(text_out, "%c[%s:] %s", c_grey1, characters.character[char_id]->char_name, text);
                    broadcast_raw_text_packet(connection, 0, CHAT_LOCAL, text_out);
                break;
            }

        break;

        case MOVE_TO:

        x_dest=Uint16_to_dec(data[0], data[1]);
        y_dest=Uint16_to_dec(data[2], data[3]);

        tile_dest=x_dest+(y_dest*maps.map[characters.character[char_id]->map_id]->map_axis);

        printf("MOVE_TO position x[%i] y[%i] tile[%i]\n", x_dest, y_dest, tile_dest);

        /*We buffer MOVE_TO commands as they are received, rather than sending them directly to the client. This
        is because the client breaks down long journeys into a series of interim destination, which are sent
        in-turn to the server. Whilst the client will continually repeat a destination to the server until it is
        reached, the timing means that, unless buffered, the char has to hang around waiting for the next one to
        be received (resulting in the char movement being jerky). Hence, we buffer MOVE_TO's and process them as
        necessary. */

        if(maps.map[map_id]->height_map[tile_dest]<MIN_TRAVERSABLE_VALUE){

            //printf("illegal destination\n");

            sprintf(text_out, "%cThe tile you clicked on can't be walked on", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);
        }
        else {
            enqueue_move_to(connection, tile_dest);
        }

        break;

        case SEND_PM:
        send_pm(connection, text);
        break;

        case SEND_ME_MY_ACTORS:
        printf("SEND OPENING SCREEN %i %i \n", lsb, msb);
        break;

        case SEND_OPENING_SCREEN:
        printf("SEND OPENING SCREEN %i %i \n", lsb, msb);
        break;

        case SEND_VERSION:
        printf("SEND VERSION %i %i \n", lsb, msb);
        break;

        case HEARTBEAT:
        printf("HEARTBEAT %i %i \n", lsb, msb);
        save_data(connection);
        break;

        case PING_RESPONSE:
        printf("PING_RESPONSE %i %i \n", lsb, msb);
        break;

        case LOG_IN:

        if(count_str_island(text)==2){

            get_str_island(text, char_name, 1);
            get_str_island(text, password, 2);

            char_id=validate_password(char_name, password);

            if(char_id>=0){

                printf("LOG_IN connection [%i] character_name[%s] password[%s]\n", connection, char_name, password);

                //update client details with char
                clients.client[connection]->character_id=char_id;
                clients.client[connection]->status=LOGGED_IN;

                //check char status
                switch (characters.character[char_id]->char_status) {

                    case CHAR_DEAD:
                        send_login_not_ok(sock);
                        //printf("Char rejected as dead\n");
                        sprintf(text_out, "%cSorry. Your character died on %s. May they rest in peace.", c_red1+127, "");
                        send_raw_text_packet(sock, CHAT_SERVER, text_out);
                    break;

                    case CHAR_BANNED:
                        send_login_not_ok(sock);
                        //printf("Char rejected as banned\n");
                        sprintf(text_out, "%cSorry. Your character has been locked", c_red1+127);
                        send_raw_text_packet(sock, CHAT_SERVER, text_out);
                    break;

                    case CHAR_ALIVE:
                        // send initial data to client
                        send_login_ok(sock);
                        //printf("Login successful\n");
                        send_you_are(sock, clients.client[connection]->character_id);
                        send_change_map(sock, maps.map[characters.character[char_id]->map_id]->elm_filename);

                        //debug_channels(char_id);

                        send_get_active_channels(sock,
                            characters.character[char_id]->active_chan,
                            characters.character[char_id]->chan[0],
                            characters.character[char_id]->chan[1],
                            characters.character[char_id]->chan[2]);

                        // add in-game chars to this clients
                        send_actors_to_client(connection);

                        // add this char to each connected client
                        broadcast_add_new_enhanced_actor_packet(connection);

                        // notify guild that char has logged on
                        if(characters.character[char_id]->guild_id>0) {
                            channel_number=guilds.guild[characters.character[char_id]->guild_id]->guild_chan_number;
                            chan_colour=guilds.guild[characters.character[char_id]->guild_id]->log_on_notification_colour;
                            sprintf(text_out, "%c%s JOINED THE GAME", chan_colour, characters.character[char_id]->char_name);
                            broadcast_raw_text_packet(connection, channel_number, CHAT_GM, text_out);
                        }

                        log_event(EVENT_SESSION, char_name);

                        //SendStatsToClient($client[$x]['SOCK'], $client[$x]['CHAR_ID']);
                        //SendMessage($client[$x]['SOCK'], HERE_YOUR_INVENTORY, $client[$x]['INVENTORY']);
                    break;
                }
            }
            else {

                //if our char name and password fails to validate, here's where return an indication of why

                switch(char_id){

                    case WRONG_PASSWORD:
                    send_login_not_ok(sock);
                    printf("Login failed - incorrect password\n");
                    break;

                    case CHARNAME_NOT_FOUND:
                    send_you_dont_exist(sock);
                    printf("Login failed - character does not exist\n");
                    break;

                    default:
                    log_event(EVENT_ERROR, "unknown password validation result in function process_packet");
                    break;
                }
            }
        }
        else {
            log_event(EVENT_ERROR, "malformed login packet");
        }
        break;

        case CREATE_CHAR:

            printf("CREATE_CHAR connection [%i]\n", connection);

            //debug_char_packet(packet);

            if(count_str_island(text)==2) {

                //get the char name and password from the packet
                get_str_island(text, char_name, 1);
                get_str_island(text, password, 2);

                i=strlen(char_name)+strlen(password)+2;

                //printf("created new character [%s] password [%s]\n", char_name, password);

                // check if a character already exists with that name
                if(get_char_id(char_name)==CHARNAME_NOT_FOUND){

                    // check if we'll exceed maximum number of characters
                    if(characters.count+1<characters.max){

                        id=characters.count; // count of chars runs from 1 whereas char_id's run from 0, hence
                        characters.count++;  // we don't increase the count until the char record has been set up
                                             // otherwise the char_id will be wrong

                        clients.client[connection]->character_id=id;
                        clients.client[connection]->status=LOGGED_IN;

                        strcpy(characters.character[id]->char_name, char_name);
                        strcpy(characters.character[id]->password, password);   //1
                        characters.character[id]->time_played=0;                //2
                        characters.character[id]->char_status=CHAR_ALIVE;       //3
                        characters.character[id]->active_chan=0;                //4
                        characters.character[id]->chan[0]=0; // chan 0          //5
                        characters.character[id]->chan[1]=0; // chan 1          //6
                        characters.character[id]->chan[2]=0; // chan 2          //7
                        characters.character[id]->chan[3]=0; // chan 3 (reserved)   //8
                        characters.character[id]->gm_permission=0;              //9
                        characters.character[id]->ig_permission=0;              //10
                        characters.character[id]->map_id=0;                     //11
                        characters.character[id]->map_tile=4236;                //12
                        characters.character[id]->guild_id=0;                   //13
                        characters.character[id]->skin_type=data[i++];          //14
                        characters.character[id]->hair_type=data[i++];          //15
                        characters.character[id]->shirt_type=data[i++];         //16
                        characters.character[id]->pants_type=data[i++];         //17
                        characters.character[id]->boots_type=data[i++];         //18
                        characters.character[id]->char_type=data[i++];          //19
                        characters.character[id]->head_type=data[i++];          //20
                        characters.character[id]->shield_type=SHIELD_NONE;      //21
                        characters.character[id]->weapon_type=WEAPON_NONE;      //22
                        characters.character[id]->cape_type=CAPE_NONE;          //23
                        characters.character[id]->helmet_type=HELMET_NONE;      //24
                        characters.character[id]->neck_type=0;                  //25
                        characters.character[id]->max_health=0;                 //26
                        characters.character[id]->current_health=0;             //27
                        characters.character[id]->visual_proximity=15;          //28
                        characters.character[id]->local_text_proximity=12;      //29

                        // save the character data to file
                        save_new_character(characters.character[id]->char_name, id);

                        // send initial data to client
                        send_login_ok(sock);
                        send_you_are(sock, clients.client[connection]->character_id);
                        send_change_map(sock, maps.map[characters.character[id]->map_id]->elm_filename);
                        send_get_active_channels(sock,
                            characters.character[id]->active_chan,
                            characters.character[id]->chan[0],
                            characters.character[id]->chan[1],
                            characters.character[id]->chan[2]);

                        //send_here_your_inventory(sock);

                        // add in-game chars to this clients
                        send_actors_to_client(connection);

                        // add this char to each connected client
                        broadcast_add_new_enhanced_actor_packet(connection);

                        sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
                        send_server_text(sock, CHAT_SERVER, text_out);

                        send_create_char_ok(sock);

                        log_event(EVENT_NEW_CHAR, char_name);
                    }
                    else {
                        //printf("Character rejected - maximum game characters exceeded\n");
                        sprintf(text_out, "%cSorry. The maximum characters supported by the server has been exceeded.", c_red3+127);
                        send_server_text(sock, CHAT_SERVER, text_out);
                        sprintf(text_out, "%cPlease report the issue to the game administrator.", c_red3+127);
                        send_server_text(sock, CHAT_SERVER, text_out);

                        send_create_char_not_ok(sock);

                        log_event(EVENT_ERROR, "server character max exceeded");
                    }
                }
                else {
                    //printf("Character rejected - duplication of existing character name\n");
                    sprintf(text_out, "%cSorry. We can't create your character as one with the same name", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                    sprintf(text_out, "%c already exists. Please try another name.", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);

                    send_create_char_not_ok(sock);
                }
             }
            else {
                log_event(EVENT_ERROR, "malformed character creation string in function process_packet");
            }

        break;

        case GET_DATE:
        printf("GET DATE %i %i \n", lsb, msb);
        break;

        case GET_TIME:
        printf("GET TIME %i %i \n", lsb, msb);
        break;

        default: // UNKNOWN
        printf("UNKNOWN PROTOCOL %i %i %i \n", protocol, lsb, msb);
        break;
    }

}