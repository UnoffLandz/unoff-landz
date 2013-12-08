#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "protocol.h"
#include "broadcast.h"
#include "string_functions.h"

void add_client_to_channel(int connection, int chan){

    char text_out[1024]="";
    int char_id=clients.client[connection]->character_id;

    channels.channel[chan]->client_list[channels.channel[chan]->client_list_count]=connection;
    channels.channel[chan]->client_list_count++;

    sprintf(text_out, "%c%s has joined channel %s", c_yellow1+127, characters.character[char_id]->char_name, channels.channel[chan]->channel_name);
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
        perror("unable to find character in function remove_char_from_map");
        exit(EXIT_FAILURE);
    }

    for(j=i; i<channels.channel[chan]->client_list_count-1; i++){

        channels.channel[chan]->client_list[j]=channels.channel[chan]->client_list[j+1];
   }

    channels.channel[chan]->client_list_count--;

    sprintf(text_out, "%c%s has left channel %s", c_yellow1+127, characters.character[char_id]->char_name, channels.channel[chan]->channel_name);
    broadcast_channel_event(chan, connection, text_out);
}

int get_chan_slot(int char_id, int chan) {

    int i;

    for(i=0; i<3; i++){
        if(characters.character[char_id]->chan[i]==chan) return i;
    }

    return -1;
}

int get_free_chan_slot(int char_id) {

    int i;

    for(i=0; i<3; i++){
        if(characters.character[char_id]->chan[i]==0) return i;
    }

    return -1;
}

int get_used_chan_slot(int char_id) {

    int i;

    for(i=0; i<3; i++){
        if(characters.character[char_id]->chan[i]>0) return i;
    }

    return -1;
}

void list_clients_in_chan(int connection, int chan){

    int i=0, j=0;
    char text_out[1024];
    int sock=clients.client[connection]->sock;
    int char_id;

    sprintf(text_out, "%cListing for channel [%i]: %s", c_blue1+127, chan, channels.channel[chan]->channel_name);
    send_server_text(sock, CHAT_SERVER, text_out);
    sprintf(text_out, "%cDescription: %s", c_blue1+127, channels.channel[chan]->description);
    send_server_text(sock, CHAT_SERVER, text_out);
    sprintf(text_out, "%cCharacters in channel...", c_blue1+127);
    send_server_text(sock, CHAT_SERVER, text_out);

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        j=channels.channel[chan]->client_list[i];
        char_id=clients.client[j]->character_id;

        sprintf(text_out, "%c%s ", c_blue1+127, characters.character[char_id]->char_name);
        send_server_text(sock, CHAT_SERVER, text_out);
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
    characters.character[char_id]->chan[slot]=chan;

    characters.character[char_id]->active_chan=slot;

    send_get_active_channels(clients.client[connection]->sock,
        characters.character[char_id]->active_chan,
        characters.character[char_id]->chan[0],
        characters.character[char_id]->chan[1],
        characters.character[char_id]->chan[2]);

    return CHANNEL_JOINED;
}

int leave_channel(int connection, int chan){

    int slot=0;
    int char_id=clients.client[connection]->character_id;

    // is channel number valid
    if(chan<0 || chan>=channels.max){
        return CHANNEL_INVALID;
    }

    // check char is actually in the channel it wants to leave
    slot=get_chan_slot(char_id, chan);
    if(slot==-1) return NOT_IN_CHANNEL;

    // remove char from channel char list
    remove_client_from_channel(connection, chan);

    // reset chan slot to empty
    characters.character[char_id]->chan[slot]=0;

    // reset active channel
    slot=get_used_chan_slot(char_id);

    if(slot==-1) {
        //there are no other open channels
        characters.character[char_id]->active_chan=0;
    }
    else {
        //set active channel to next open channel
        characters.character[char_id]->active_chan=slot;
    }

    send_get_active_channels(clients.client[connection]->sock,
        characters.character[char_id]->active_chan,
        characters.character[char_id]->chan[0],
        characters.character[char_id]->chan[1],
        characters.character[char_id]->chan[2]);

    return CHANNEL_LEFT;
}

int process_chat(int connection, char *text_in){

    int char_id=clients.client[connection]->character_id;
    int chan_slot=0;
    int chan=0;
    char text_out[1024]="";
    int text_len=strlen(text_in);

    //remove the @ from text
    memcpy(text_in, text_in+1, text_len-1);
    text_in[text_len-1]='\0';

    // check if char is in a channel
    if(get_used_chan_slot(char_id)==-1) return CHAR_NOT_IN_CHAN;

    // get the active slot
    chan_slot=characters.character[char_id]->active_chan;

    // find the channel number for the active slot
    chan=characters.character[char_id]->chan[chan_slot];

    // echo to sender
    sprintf(text_out, "%c[%s:] %s", c_grey1, characters.character[char_id]->char_name, text_in);
    send_raw_text_packet(clients.client[connection]->sock, CHAT_SERVER, text_out);

    //send to others in chan (don't send colour as thats added in function broadcast_channel_chat
    sprintf(text_out, "[%s:] %s", characters.character[char_id]->char_name, text_in);
    broadcast_channel_chat(chan, char_id, text_out);

    printf("Chat sent from %s to channel %s: %s\n", characters.character[char_id]->char_name, channels.channel[chan]->channel_name, text_in);

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

    int char_id=clients.client[connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
    char hash_command_tail[1024]="";
    //int chan=0;
    int chan_colour=0;
    char text_out[1024]="";
    int command_parts=count_str_island(text);

    // check that #GM command is properly formed (should have 2 parts delimited by space)
    if(command_parts<2) return GM_INVALID;

    // check if char is in a guild
    if(characters.character[char_id]->guild_id>0) return GM_NO_GUILD;

    // check if char has permission to use GM
    if(characters.character[char_id]->gm_permission==1) return GM_NO_PERMISSION;

    //get the #GM command message element
    get_str_island(text, hash_command_tail, 2);
    str_conv_upper(hash_command_tail);

    // send to senders guild channel
    //chan=guilds.guild[guild_id]->channel_number;
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#GM from %s: %s", chan_colour, characters.character[char_id]->char_name, hash_command_tail);

    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_id, text_out);

    return GM_SENT;
}

int process_inter_guild_chat(int connection, char *text){

    int char_id=clients.client[connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
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
    if(characters.character[char_id]->guild_id==0) return IG_NOT_AVAILABLE;

    // check if char has permission to use IG
    if(characters.character[char_id]->ig_permission==1) return IG_NO_PERMISSION;

    // check if message is to be sent to a valid guild
    if(guild_number==-1) return IG_INVALID_GUILD;

    // broadcast to senders guild channel
    //chan=characters.character[char_id]->chan[0];
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [sent %s to %s]: %s", chan_colour, characters.character[char_id]->char_name, guilds.guild[guild_number]->guild_tag, hash_command_tail);
    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_id, text_out);

    // broadcast to receiver guild channel
    chan_colour=guilds.guild[guild_number]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [from %s of %s]: %s", chan_colour, characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, hash_command_tail);
    //broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);
    broadcast_guild_channel_chat(guild_number, text_out);

    return IG_SENT;
}

