#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> //needed for send function

#include "global.h"
#include "protocol.h"
#include "broadcast.h"
#include "string_functions.h"
#include "debug.h"
#include "files.h"
#include "chat.h"
#include "log_in.h"
#include "database.h"

void send_get_active_channels(int connection){

    unsigned char packet[1024];
    int i=0, j=0;

    packet[0]=71;
    packet[1]=14;
    packet[2]=0;
    packet[3]=clients.client[connection]->active_chan;


    for(i=0; i<3; i++){

        j=i*4;

        packet[j+4]=clients.client[connection]->chan[i] % 256;
        packet[j+5]=clients.client[connection]->chan[i]/256 % 256;
        packet[j+6]=clients.client[connection]->chan[i]/256/256 % 256;
        packet[j+7]=clients.client[connection]->chan[i]/256/256/256 % 256;
    }

    send(connection, packet, 16, 0);
}

int get_char_connection(char char_id){

    /** RESULT  : finds the character id from a character name

        RETURNS : character id

        PURPOSE : allows private messages to be vectored by char name

        USAGE   : send_pm
    */

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

void add_client_to_channel(int connection, int chan){

    char text_out[1024]="";

    channels.channel[chan]->client_list[channels.channel[chan]->client_list_count]=connection;
    channels.channel[chan]->client_list_count++;

    sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[connection]->char_name, channels.channel[chan]->channel_name);
    broadcast_channel_event(chan, connection, text_out);
}

void remove_client_from_channel(int connection, int chan){

    int i=0, j=0;
    int found=0;
    char text_out[1024]="";
    int char_id=clients.client[connection]->character_id;

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        if(channels.channel[chan]->client_list[i]==connection) {
            found=1;
            break;
        }
    }

    if(found==0) {

        sprintf(text_out, "unable to find char id [%i] in function remove_client_from_channel", char_id);
        log_event(EVENT_ERROR, text_out);

        perror(text_out);
        exit(EXIT_FAILURE);
    }

    for(j=i; i<channels.channel[chan]->client_list_count-1; i++){

        channels.channel[chan]->client_list[j]=channels.channel[chan]->client_list[j+1];
   }

    channels.channel[chan]->client_list_count--;

    sprintf(text_out, "%c%s has left channel %s", c_yellow1+127, clients.client[connection]->char_name, channels.channel[chan]->channel_name);
    broadcast_channel_event(chan, connection, text_out);
}

int get_chan_slot(int connection, int chan) {

    int i;

    for(i=0; i<3; i++){
        if(clients.client[connection]->chan[i]==chan) return i;
    }

    return -1;
}

int get_free_chan_slot(int connection) {

    int i;

    for(i=0; i<3; i++){
        if(clients.client[connection]->chan[i]==0) return i;
    }

    return -1;
}

int get_used_chan_slot(int connection) {

    int i;

    for(i=0; i<3; i++){
        if(clients.client[connection]->chan[i]>0) return i;
    }

    return -1;
}

void list_clients_in_chan(int connection, int chan){

    int i=0;
    char text_out[1024];

    sprintf(text_out, "%cListing for channel [%i]: %s", c_blue1+127, chan, channels.channel[chan]->channel_name);
    send_server_text(connection, CHAT_SERVER, text_out);
    sprintf(text_out, "%cDescription: %s", c_blue1+127, channels.channel[chan]->description);
    send_server_text(connection, CHAT_SERVER, text_out);
    sprintf(text_out, "%cCharacters in channel...", c_blue1+127);
    send_server_text(connection, CHAT_SERVER, text_out);

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        sprintf(text_out, "%c%s ", c_blue1+127, clients.client[connection]->char_name);
        send_server_text(connection, CHAT_SERVER, text_out);
    }
}

int join_channel(int connection, int chan){

    int slot=0;
    int char_id=clients.client[connection]->character_id;

    // is channel number valid
    if(chan<0 || chan>=channels.max){
        return CHANNEL_INVALID;
    }

    // is channel type valid
    switch (channels.channel[chan]->chan_type){

        case CHAN_SYSTEM:
            return CHANNEL_SYSTEM;
        break;

        case CHAN_VACANT:
            return CHANNEL_NOT_OPEN;
        break;

        case CHAN_PERMANENT:
            //place entry conditions here
        break;

        case CHAN_GUILD:
            return CHANNEL_BARRED;
        break;

        case CHAN_CHAT:
            //reserved for debug
        break;

        default:
            return CHANNEL_UNKNOWN;
        break;
    }

    //find an empty channel slot
    slot=get_free_chan_slot(char_id);
    if(slot==-1) return NO_FREE_CHANNEL_SLOTS;

    //add char to chan char_list
    add_client_to_channel(connection, chan);

    // send revised chan data to client
    clients.client[connection]->chan[slot]=chan;

    clients.client[connection]->active_chan=slot;

    send_get_active_channels(connection);

/*
    send_get_active_channels(clients.client[connection]->sock,
        characters.character[char_id]->active_chan,
        characters.character[char_id]->chan[0],
        characters.character[char_id]->chan[1],
        characters.character[char_id]->chan[2]);
*/
    return CHANNEL_JOINED;
}

int leave_channel(int connection, int chan){

    int slot=0;

    // is channel number valid
    if(chan<0 || chan>=channels.max){
        return CHANNEL_INVALID;
    }

    // check char is actually in the channel it wants to leave
    slot=get_chan_slot(connection, chan);
    if(slot==-1) return NOT_IN_CHANNEL;

    // remove char from channel char list
    remove_client_from_channel(connection, chan);

    // reset chan slot to empty
    clients.client[connection]->chan[slot]=0;

    // reset active channel
    slot=get_used_chan_slot(connection);

    if(slot==-1) {
        //there are no other open channels
        clients.client[connection]->active_chan=0;
    }
    else {
        //set active channel to next open channel
        clients.client[connection]->active_chan=slot;
    }

    send_get_active_channels(connection);

/*
    send_get_active_channels(clients.client[connection]->sock,
        characters.character[char_id]->active_chan,
        characters.character[char_id]->chan[0],
        characters.character[char_id]->chan[1],
        characters.character[char_id]->chan[2]);
*/
    return CHANNEL_LEFT;
}

int process_chat(int connection, char *text_in){

    int char_id=clients.client[connection]->character_id;
    int chan_slot=0;
    int chan=0;
    char text_out[1024]="";
    int text_len=strlen(text_in);

    //remove the @ from text string
    memcpy(text_in, text_in+1, text_len-1);
    text_in[text_len-1]='\0';

    // check if char is in a channel
    if(get_used_chan_slot(char_id)==-1) return CHAR_NOT_IN_CHAN;

    // get the active slot
    chan_slot=clients.client[connection]->active_chan;

    // find the channel number for the active slot
    chan=clients.client[connection]->chan[chan_slot];

    printf("active chan slot %i\n",clients.client[connection]->active_chan);
    printf("active chan %i\n", clients.client[connection]->chan[chan_slot]);

    //create the chat string
    sprintf(text_out, "%c[%s @ %i(%s)] %s", c_grey1+127, clients.client[connection]->char_name, chan, channels.channel[chan]->channel_name, text_in);

    // echo to sender
    send_raw_text_packet(connection, CHAT_SERVER, text_out);

    //send to others in chan (don't send colour as thats added in function broadcast_channel_chat
    broadcast_channel_chat(chan, connection, text_in);

    printf("Chat sent from %s to channel %s: %s\n", clients.client[connection]->char_name, channels.channel[chan]->channel_name, text_in);

    sprintf(text_out, "[%s @ %i(%s)] %s", clients.client[connection]->char_name, chan, channels.channel[chan]->channel_name, text_in);
    log_event(EVENT_CHAT, text_out);

    return CHAN_CHAT_SENT;
}

int get_guild_number(char *guild_tag){

    int i;

    for(i=1; i<guilds.max; i++){

        if(strcmp(guild_tag, guilds.guild[i]->guild_tag)==0) return i;
    }

    return -1;
}

int process_guild_chat(int connection, char *text){

    int guild_id=clients.client[connection]->guild_id;
    char hash_command_tail[1024]="";
    //int chan=0;
    int chan_colour=0;
    char text_out[1024]="";
    int command_parts=count_str_island(text);

    // check that #GM command is properly formed (should have 2 parts delimited by space)
    if(command_parts<2) return GM_INVALID;

    // check if char is in a guild
    if(clients.client[connection]->guild_id>0) return GM_NO_GUILD;

    // check if char has permission to use GM
    if(clients.client[connection]->gm_permission==1) return GM_NO_PERMISSION;

    //get the #GM command message element
    get_str_island(text, hash_command_tail, 2);
    str_conv_upper(hash_command_tail);

    // send to senders guild channel
    //chan=guilds.guild[guild_id]->channel_number;
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#GM from %s: %s", chan_colour, clients.client[connection]->char_name, hash_command_tail);

    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_id, text_out);

    return GM_SENT;
}

int process_inter_guild_chat(int connection, char *text){

    int guild_id=clients.client[connection]->guild_id;
    int command_parts=count_str_island(text);
    char guild_tag[4]="";
    char hash_command_tail[1024]="";
    int guild_number=0;
    int chan_colour=0;
    char text_out[1024]="";

    //check that #IG command is properly formed (should have 3 parts delimited by spaces)
    if(command_parts<3) return IG_MALFORMED;

    // split the #IG command into target guild and message elements
    get_str_island(text, guild_tag, 2);
    str_conv_upper(guild_tag);

    guild_number=get_guild_number(guild_tag);

    get_str_island(text, hash_command_tail, 3);

    // check if char is in a guild
    if(clients.client[connection]->guild_id==0) return IG_NOT_AVAILABLE;

    // check if char has permission to use IG
    if(clients.client[connection]->ig_permission==1) return IG_NO_PERMISSION;

    // check if message is to be sent to a valid guild
    if(guild_number==-1) return IG_INVALID_GUILD;

    // broadcast to senders guild channel
    //chan=characters.character[char_id]->chan[0];
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [sent %s to %s]: %s", chan_colour, clients.client[connection]->char_name, guilds.guild[guild_number]->guild_tag, hash_command_tail);
    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_id, text_out);

    // broadcast to receiver guild channel
    chan_colour=guilds.guild[guild_number]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [from %s of %s]: %s", chan_colour, clients.client[connection]->char_name, guilds.guild[guild_id]->guild_tag, hash_command_tail);
    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_number, text_out);

    return IG_SENT;
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

        strcpy(sender_name, clients.client[connection]->char_name);

        printf("PM from [%s] to [%s]: %s\n", receiver_name, sender_name, msg);

        // echo message back to sender
        sprintf(text_out, "%c[PM to %s: %s]", c_orange1+127, receiver_name, msg);
        send_server_text(receiver_connection, CHAT_PERSONAL, text_out);

        // determine id of the receiver char
        receiver_id=get_char_data(receiver_name);

        if(receiver_id!=CHAR_NOT_FOUND) {

            // determine if the receiver char exists by seeing if it has a connection
            receiver_connection=get_char_connection(receiver_id);

            if(receiver_connection!=-1) {

                // receiver char is in-game
                sprintf(text_out, "%c[PM from %s: %s]", c_orange1+127, sender_name, msg);
                send_server_text(connection, CHAT_PERSONAL, text_out);
            }
            else {
                // receiver char is not in-game
                sprintf(text_out, "%ccharacter is not currently logged on. To send a letter use #LETTER [name] [message]", c_red2+127);
                send_server_text(connection, CHAT_PERSONAL, text_out);
                printf("SEND_PM from [%s] to [%s] message [%s] - receiver not in-game\n", sender_name, receiver_name, msg);
            }
        }
        else {

            // receiver char does not exist
            sprintf(text_out, "%ccharacter does not exist\n", c_red1+127);
            send_server_text(connection, CHAT_PERSONAL, text_out);
            printf("SEND_PM from [%s] to [%s] message [%s] - receiver does not exist\n", sender_name, receiver_name, msg);
        }
    }
    else {
        // tried to send a zero length message
        sprintf(text_out, "%cno text in message", c_red1+127);
        send_server_text(connection, CHAT_PERSONAL, text_out);
        printf("SEND_PM from [%s] to [%s] - zero length message\n", sender_name, receiver_name);
    }

}
