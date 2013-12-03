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
#include "maps.h"

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

        // match char_name with an existing char
        if(strcmp(char_name, characters.character[i]->char_name)==0) return i;
     }

    return CHAR_NOT_FOUND;
}

int validate_password(int char_id, char *password){

    if(strcmp(password, characters.character[char_id]->password)==0) return PASSWORD_CORRECT;

    return PASSWORD_INCORRECT;
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

    // TODO (derekl#2#): convert loop to memcpy    /* add packet content */
    for(i=3; i<3+filename_length; i++){
        packet[i]=elm_filename[i-3];
    }

    send(sock, packet, packet_length, 0);
}

void transport_char(int connection, int tile, int map_id, int transport_type){

    /*  RESULT  : Move a char to a new map/initialises a char on a map

        PURPOSE : Consolidate all required operations into a resuable function that can be called
                 on both the login and chan_map protocol

        USAGE   : protocol.c process_packet
    */
/*
    int char_id=clients.client[connection]->character_id;

    if(transport_type==CHANGE_MAP) {

        //    check for ticket

        //broadcast actor removal to other chars on map
        broadcast_remove_actor_packet(connection);

        //remove from local map list
        remove_client_from_map_list(connection, map_id);

        //      implement 'stay' timer.
    }

    //         check for residency status
    //      advise actor of new map ownership and policies

    //adjust char map and position
    characters.character[char_id]->map_id=map_id;
    characters.character[char_id]->map_tile=JUMP_POINT;

    //send new char map to client
    send_change_map(sock, maps.map[characters.character[char_id]->map_id]->elm_filename);

    // add in-game chars to this clients ######## need to amend packet to include actor pos
    send_actors_to_client(connection);

    // add this char to each connected client ######### need amend packet to include actor pos
    broadcast_add_new_enhanced_actor_packet(connection);

    //add client to local map list
    add_client_to_map(connection, map_id);

    // add other chars to this client
    send_actors_to_client(connection);

    // broadcast this char to other clients
    broadcast_add_new_enhanced_actor_packet(connection);

    //      log client map move (time / char_id / originating map id)
*/
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
    int chan_colour=0;
    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int guild_id=characters.character[char_id]->guild_id;
    int sock=clients.client[connection]->sock;
    int protocol=packet[0];
    int lsb=packet[1];
    int msb=packet[2];
    int data_length=lsb+(msb*256)-1;
    int x_dest=0, y_dest=0, tile_dest=0;
    int map_object_id=0;
    int use_with_position=0;

    // extract data from packet
    for(i=0; i<data_length; i++){
        data[i]=packet[i+3]; // unsigned char array for bit manipulation
         text[i]=packet[i+3]; // signed char array for text manipulation
    }

    text[data_length]='\0';
    int text_len=strlen(text);

    switch(protocol){

        case RAW_TEXT:

            printf("RAW_TEXT [%s]\n", text);

            // trim off excess left hand space
            str_trim_left(text);

            //debug_raw_text(connection, text, text_len);

            switch(text[0]){

                case '@': // chat

                    //memcpy(text, text+1, text_len-1);
                    //text[text_len-1]='\0';

                    switch(process_chat(connection, text)){

                        case CHAR_NOT_IN_CHAN:
                            sprintf(text_out, "%cyou have not joined a channel yet", c_red3+127);
                            send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);
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
                        break;

                        case HASH_CMD_UNKNOWN:
                            sprintf(text_out, "%cThat command isn't supported yet. You may want to tell the game administrator", c_red3+127);
                            send_server_text(clients.client[connection]->sock, CHAT_MOD, text_out);
                        break;

                        case HASH_CMD_EXECUTED:
                             //this case is reserved for debug purposes
                        break;

                        case HASH_CMD_FAILED:
                             //this case is reserved for debug purposes
                        break;

                        default:
                            log_event(EVENT_ERROR, "unknown result from function process_hash_command");
                        break;
                    }

                break;

                default://local chat
                    sprintf(text_out, "%c[%s:] %s", c_grey1, characters.character[char_id]->char_name, text);
                    broadcast_local_chat(connection, map_id, text_out);
                    //broadcast_raw_text_packet(connection, 0, CHAT_LOCAL, text_out);
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
        gettimeofday(&time_check, NULL);
        //printf("lag %i\n",time_check.tv_sec-clients.client[i]->time_of_last_heartbeat);
        clients.client[i]->time_of_last_heartbeat=time_check.tv_sec;
        save_data(connection);//needs to be on a separate timer
        break;

        case USE_OBJECT:
        printf("USE_OBJECT %i %i \n", lsb, msb);
        map_object_id=Uint32_to_dec(data[0], data[1], data[2], data[3]);
        use_with_position=Uint32_to_dec(data[4], data[5], data[6], data[7]);
        printf("map object id %i use with position %i\n", map_object_id, use_with_position);
/*
        float x_pos, y_pos;

        get_threed_object_pos(map_object_id, map_id, &x_pos, &y_pos);

        printf("x_pos %f y_pos %f\n", x_pos, y_pos );
*/
        break;

        case PING_RESPONSE:
        printf("PING_RESPONSE %i %i \n", lsb, msb);
        break;

        case LOG_IN:

        printf("LOG_IN connection [%i]\n", connection);

        if(count_str_island(text)!=2){
            sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "malformed login attempt for existing char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_ERROR, text_out);
            return;
        }

        get_str_island(text, char_name, 1);
        get_str_island(text, password, 2);
        printf("character_name[%s] password[%s]\n", char_name, password);

        char_id=get_char_id(char_name);

        if(char_id==CHAR_NOT_FOUND) {
            send_you_dont_exist(sock);
            sprintf(text_out, "%cSorry, but that character name does not exist", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "login attempt with unknown char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_SESSION, text_out);
            return;
        }

        if(validate_password(char_id, password)==PASSWORD_INCORRECT){
            send_login_not_ok(sock);
            sprintf(text_out, "%cSorry, but that password is incorrect", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "login attempt with incorrect password char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_SESSION, text_out);
            return;
        }

        //update client details with char
        clients.client[connection]->character_id=char_id;
        clients.client[connection]->status=LOGGED_IN;

        //check char status
        if(characters.character[char_id]->char_status==CHAR_DEAD) {
            send_login_not_ok(sock);
            sprintf(text_out, "%cSorry. Your character died on %s. May they rest in peace.", c_red1+127, "");
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "login attempt for dead char [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);
            return;
        }

        if(characters.character[char_id]->char_status==CHAR_BANNED) {
            send_login_not_ok(sock);
            sprintf(text_out, "%cSorry. Your character has been locked", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "login attempt for locked char [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);
            return;
        }

        //if we get this far then  char is alive and not banned or dead
        send_login_ok(sock);
        send_you_are(sock, clients.client[connection]->character_id);

        //set char map to client
        send_change_map(sock, maps.map[characters.character[char_id]->map_id]->elm_filename);

        //add client to local map list
        add_client_to_map(connection, map_id);

        //send char channels to client
        send_get_active_channels(sock,
            characters.character[char_id]->active_chan,
            characters.character[char_id]->chan[0],
            characters.character[char_id]->chan[1],
            characters.character[char_id]->chan[2]);

        //add client to local channel lists
        for(i=0; i<3; i++){
            if(characters.character[char_id]->chan[i]>0){
                add_client_to_channel(connection, characters.character[char_id]->chan[i]);
            }
        }

        // add in-game chars to this clients
        send_actors_to_client(connection);

        // add this char to each connected client
        broadcast_add_new_enhanced_actor_packet(connection);

        // notify guild that char has logged on
        if(guild_id>0) {
            chan_colour=guilds.guild[guild_id]->log_on_notification_colour;
            sprintf(text_out, "%c%s JOINED THE GAME", chan_colour, characters.character[char_id]->char_name);
            broadcast_guild_channel_chat(guild_id, text_out);
        }

        sprintf(text_out, "login succesful char [%s]\n", char_name);
        log_event(EVENT_ERROR, text_out);

        //SendStatsToClient($client[$x]['SOCK'], $client[$x]['CHAR_ID']);
        //SendMessage($client[$x]['SOCK'], HERE_YOUR_INVENTORY, $client[$x]['INVENTORY']);

        break;

        case CREATE_CHAR:

        printf("CREATE_CHAR connection [%i]\n", connection);

        if(count_str_island(text)!=2){
            sprintf(text_out, "%cSorry, but that caused an error", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);
            send_create_char_not_ok(sock);

            sprintf(text_out, "malformed login attempt for new char name [%s] password [%s]\n", char_name, password);
            log_event(EVENT_ERROR, text_out);
            return;
        }

        //get the char name and password from the packet
        get_str_island(text, char_name, 1);
        get_str_island(text, password, 2);

        i=strlen(char_name)+strlen(password)+2;

        char_id=get_char_id(char_name);

        if(char_id!=CHAR_NOT_FOUND) {
            send_create_char_not_ok(sock);
            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "login attempt for new char with existing char name [%s]\n", char_name);
            log_event(EVENT_SESSION, text_out);
            return;
        }

        // check if we'll exceed maximum number of characters
        if(characters.count+1==characters.max){
            send_create_char_not_ok(sock);
            sprintf(text_out, "%cSorry, but the maximum number of characters on the server has been reached", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "maximum number of characters on server has been reached\n");
            log_event(EVENT_ERROR, text_out);
            return;
        }

        id=characters.count; // count of chars runs from 1 whereas char_id's run from 0, hence
        characters.count++;  // we don't increase the count until the char record has been set up
                             // otherwise the char_id will be wrong

        clients.client[connection]->character_id=id;
        clients.client[connection]->status=LOGGED_IN;

        strcpy(characters.character[id]->char_name, char_name);
        strcpy(characters.character[id]->password, password);   //1
        characters.character[id]->time_played=0;                //2
        characters.character[id]->char_status=CHAR_ALIVE;       //3
        characters.character[id]->active_chan=1;                //4 automatically set chan nub
        characters.character[id]->chan[0]=1; // chan 0          //5 automatically set chan nub
        characters.character[id]->chan[1]=0; // chan 1          //6
        characters.character[id]->chan[2]=0; // chan 2          //7
        characters.character[id]->chan[3]=0; // chan 3 (reserved)   //8
        characters.character[id]->gm_permission=0;              //9
        characters.character[id]->ig_permission=0;              //10
        characters.character[id]->map_id=DEFAULT_MAP;            //11
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

        //send map to client
        send_change_map(sock, maps.map[characters.character[id]->map_id]->elm_filename);

        //add client to local map list
        add_client_to_map(connection, characters.character[id]->map_id);

        //send channels to client
        send_get_active_channels(sock,
            characters.character[id]->active_chan,
            characters.character[id]->chan[0],
            characters.character[id]->chan[1],
            characters.character[id]->chan[2]);

        //add client to local channel lists
        for(i=0; i<3; i++){
            if(characters.character[char_id]->chan[i]>0){
                add_client_to_channel(connection, characters.character[char_id]->chan[i]);
            }
        }

        //send_here_your_inventory(sock);

        // add in-game chars to this clients
        send_actors_to_client(connection);

        // add this char to each connected client
        broadcast_add_new_enhanced_actor_packet(connection);

        sprintf(text_out, "%cCongratulations. You've created your new game character.", c_green3+127);
        send_server_text(sock, CHAT_SERVER, text_out);

        send_create_char_ok(sock);

        sprintf(text_out, "New character created name [%s] password [%s]\n", char_name, password);
        log_event(EVENT_NEW_CHAR, text_out);

        break;

        case GET_DATE:
        printf("GET DATE %i %i \n", lsb, msb);
        break;

        case GET_TIME:
        printf("GET TIME %i %i \n", lsb, msb);
        break;

        default: // UNKNOWN
        printf("UNKNOWN PROTOCOL %i %i %i \n", protocol, lsb, msb);
        sprintf(text_out, "unknown protocol [%i]\n", protocol);
        log_event(EVENT_ERROR, text_out);
        break;
    }

}
