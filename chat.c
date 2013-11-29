#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "protocol.h"
#include "broadcast.h"
#include "string_functions.h"

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

    int i=0;
    int id;
    char text_out[1024];
    int sock=clients.client[connection]->sock;

    sprintf(text_out, "%cListing for channel [%i]: %s", c_blue1+127, chan, channels.channel[chan]->channel_name);
    send_server_text(sock, CHAT_SERVER, text_out);
    sprintf(text_out, "%cDescription: %s", c_blue1+127, channels.channel[chan]->description);
    send_server_text(sock, CHAT_SERVER, text_out);
    sprintf(text_out, "%cCharacters in channel...", c_blue1+127);
    send_server_text(sock, CHAT_SERVER, text_out);

    for(i=0; i<clients.max; i++){

        id=clients.client[i]->character_id;

        if (clients.client[i]->status==LOGGED_IN) {

            if (get_chan_slot(id, chan)>-1){

                sprintf(text_out, "%c%s ", c_blue1+127, characters.character[id]->char_name);

                send_server_text(sock, CHAT_SERVER, text_out);
            }
        }
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
    if(channels.channel[chan]->chan_type==CHAN_SYSTEM) {
        return CHANNEL_SYSTEM;
    }
    else if(channels.channel[chan]->chan_type==CHAN_VACANT) {
        return CHANNEL_NOT_OPEN;
    }
    else if(channels.channel[chan]->chan_type==CHAN_PERMANENT) {
        //place entry conditions here
    }
    else if(channels.channel[chan]->chan_type==CHAN_GUILD) {
        return CHANNEL_BARRED;
    }
    else if(channels.channel[chan]->chan_type==CHAN_CHAT) {
        //place entry conditions here
    }
    else return CHANNEL_UNKNOWN;

    //find an empty channel slot
    slot=get_free_chan_slot(char_id);
    if(slot==-1) return NO_FREE_CHANNEL_SLOTS;

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

    // check if char is in a channel
    if(get_used_chan_slot(char_id)==-1) return -1;

    // get the active slot
    chan_slot=characters.character[char_id]->active_chan;

    // find the channel number for the active slot
    chan=characters.character[char_id]->chan[chan_slot];

    // send text over channel
    sprintf(text_out, "%c[%s:] %s", c_grey1, characters.character[char_id]->char_name, text_in);

    broadcast_raw_text_packet(connection, chan, CHAT_SERVER, text_out);
    printf("Chat sent from %s to channel %s: %s\n", characters.character[char_id]->char_name, channels.channel[chan]->channel_name, text_in);

    return 0;
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
    int chan=0;
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
    chan=characters.character[char_id]->chan[3];
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#GM from %s: %s", chan_colour, characters.character[char_id]->char_name, hash_command_tail);

    broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);

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
    int chan=0;

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
    chan=characters.character[char_id]->chan[0];
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [sent %s to %s]: %s", chan_colour, characters.character[char_id]->char_name, guilds.guild[guild_number]->guild_tag, hash_command_tail);
    broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);

    // broadcast to receiver guild channel
    chan=guilds.guild[guild_number]->guild_chan_number;
    chan_colour=guilds.guild[guild_number]->guild_chan_text_colour;

    sprintf(text_out, "%c#IG [from %s of %s]: %s", chan_colour, characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, hash_command_tail);
    broadcast_raw_text_packet(connection, chan, CHAT_GM, text_out);

    return IG_SENT;
}
