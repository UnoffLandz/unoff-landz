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
#include "chat.h"

int get_char_chat_proximity(int connection){

    int race_id=clients.client[connection]->char_type;
    int initial_chat_proximity=race[race_id].initial_chat_proximity;
    int chat_proximity_multiplier=race[race_id].chat_proximity_multiplier;

    return initial_chat_proximity + (chat_proximity_multiplier * clients.client[connection]->will);
}

void send_get_active_channels(int connection){

    unsigned char packet[1024];
    int i=0, j=0;

    packet[0]=71;
    packet[1]=14;
    packet[2]=0;
    packet[3]=clients.client[connection]->active_chan;

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        j=i*4;

        packet[j+4]=clients.client[connection]->chan[i] % 256;
        packet[j+5]=clients.client[connection]->chan[i]/256 % 256;
        packet[j+6]=clients.client[connection]->chan[i]/256/256 % 256;
        packet[j+7]=clients.client[connection]->chan[i]/256/256/256 % 256;
    }

    send(connection, packet, 16, 0);
}

void add_client_to_channel(int connection, int chan_id){

    char text_out[1024]="";
    int channel_client_count=channels.channel[chan_id]->client_list_count;

    channels.channel[chan_id]->client_list[channel_client_count]=connection;

    if(channels.channel[chan_id]->client_list_count+1<MAX_CHANNELS) {

        channels.channel[chan_id]->client_list_count++;
    }
    else {

        //should never happen as we already check this bounds on the channel struct array
        sprintf(text_out, "channel client list range [%i] exceeded in function add_client_to_channel: module chat.c", MAX_CHANNELS);
        log_event(EVENT_ERROR, text_out);
        exit(EXIT_FAILURE);
    }

    sprintf(text_out, "%c%s has joined channel %s", c_yellow2+127, clients.client[connection]->char_name, channels.channel[chan_id]->channel_name);
    broadcast_channel_event(chan_id, connection, text_out);
}

void remove_client_from_channel(int connection, int chan){

    int i=0, j=0;
    int found=0;
    char text_out[1024]="";

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        if(channels.channel[chan]->client_list[i]==connection) {
            found=1;
            break;
        }
    }

    if(found==0) {
        sprintf(text_out, "unable to find connection [%i] in function remove_client_from_channel", connection);
        log_event(EVENT_ERROR, text_out);
        exit(EXIT_FAILURE);
    }

    for(j=i; i<channels.channel[chan]->client_list_count-1; i++){

        channels.channel[chan]->client_list[j]=channels.channel[chan]->client_list[j+1];
    }

    channels.channel[chan]->client_list_count--;

    sprintf(text_out, "%c%s has left channel %s", c_yellow1+127, clients.client[connection]->char_name, channels.channel[chan]->channel_name);
    broadcast_channel_event(chan, connection, text_out);
}

int get_chan_slot(int connection, int chan, int *slot) {

    int i;

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[connection]->chan[i]==chan) {
            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_free_chan_slot(int connection, int *slot) {

    int i;

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[connection]->chan[i]==0) {
            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int get_used_chan_slot(int connection, int *slot) {

    int i;

    for(i=0; i<MAX_CHAN_SLOTS; i++){

        if(clients.client[connection]->chan[i]>0) {
            *slot=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
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
    char text_out[80]="";

    // check for valid channel number
    if(chan<0 || chan>=channels.max){

        sprintf(text_out, "%cyou tried to join an invalid channel", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_JOINED;
    }

    // check for vaid channel type
   if(channels.channel[chan]->chan_type==CHAN_SYSTEM){

        sprintf(text_out, "%cchannel is reserved for system use", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_JOINED;
    }
    else if(channels.channel[chan]->chan_type==CHAN_VACANT){

        sprintf(text_out, "%cThat channel is not open", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_JOINED;
    }
    else if(channels.channel[chan]->chan_type==CHAN_GUILD){

        sprintf(text_out, "%cThat channel is for a guild", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_JOINED;
    }
    else {

        sprintf(text_out, "Unknown chan type in function join_channel: module chat.c");
        log_event(EVENT_ERROR, text_out);
        exit(EXIT_FAILURE);
    }

    //find an empty channel slot
    if(get_free_chan_slot(connection, &slot)==NOT_FOUND){

        sprintf(text_out, "%cyou can only have three open channels", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        return CHANNEL_NOT_JOINED;
    }

    //add char to chan char_list
    add_client_to_channel(connection, chan);

    // send revised chan data to client
    clients.client[connection]->chan[slot]=chan;
    clients.client[connection]->active_chan=slot;
    send_get_active_channels(connection);

    //update database
    update_db_char_channels(connection);

    // need to echo back to player which channel was just joined and its description etc
    sprintf(text_out, "%cYou joined channel %s", c_green3+127, channels.channel[chan]->channel_name);
    send_server_text(connection, CHAT_SERVER, text_out);

    sprintf(text_out, "%cDescription : %s", c_green2+127, channels.channel[chan]->description);
    send_server_text(connection, CHAT_SERVER, text_out);

    sprintf(text_out, "%cIn channel :", c_green1+127);
    send_server_text(connection, CHAT_SERVER, text_out);
    list_clients_in_chan(connection, chan);

    return CHANNEL_JOINED;
}

int leave_channel(int connection, int chan){

    int slot=0;
    char text_out[80]="";

    // is channel number valid
    if(chan<0 || chan>=channels.max){

        sprintf(text_out, "%cinvalid channel number", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_LEFT;
    }

    // check char is actually in the channel it wants to leave
    if(get_chan_slot(connection, chan, &slot)==NOT_FOUND){

        sprintf(text_out, "%cyou aren't in that channels", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return CHANNEL_NOT_LEFT;
    }

    // remove char from channel char list
    remove_client_from_channel(connection, chan);

    // reset chan slot to empty
    clients.client[connection]->chan[slot]=0;

    // reset active channel
    if(get_used_chan_slot(connection, &slot)==NOT_FOUND){

        //there are no other open channels
        clients.client[connection]->active_chan=0;
    }
    else {

        //set active channel to next open channel
        clients.client[connection]->active_chan=slot;
    }

    //send new active chan to client
    send_get_active_channels(connection);

    //update database
    update_db_char_channels(connection);

    sprintf(text_out, "%cyou left channel %s", c_green3+127, channels.channel[chan]->channel_name);
    send_server_text(connection, CHAT_SERVER, text_out);

    return CHANNEL_LEFT;
}

int process_chat(int connection, char *text_in){

    int slot=0;
    int chan=0;
    char text_out[1024]="";
    int text_len=strlen(text_in);

    //remove the @ from text string
    memcpy(text_in, text_in+1, text_len-1);
    text_in[text_len-1]='\0';

    // check if char is in a channel
    if(get_used_chan_slot(connection, &slot)==NOT_FOUND) return CHAR_NOT_IN_CHAN;

    // get the active slot
    slot=clients.client[connection]->active_chan;

    // find the channel number for the active slot
    chan=clients.client[connection]->chan[slot];

    printf("active chan slot %i\n",clients.client[connection]->active_chan);
    printf("active chan %i\n", clients.client[connection]->chan[slot]);

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

int get_guild_id(char *guild_tag, int *guild_id){

    int i;

    for(i=1; i<guilds.max; i++){

        if(strcmp(guild_tag, guilds.guild[i]->guild_tag)==0) {
            *guild_id=i;
            return FOUND;
        }
    }

    return NOT_FOUND;
}

int process_guild_chat(int connection, char *message){

    int guild_id=clients.client[connection]->guild_id;
    int chan_colour=0;
    char text_out[1024]="";

    //filter blank messages
    if(strlen(message)==0){

        sprintf(text_out, "%cyou forgot to send a message", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    //check sender is in a guild
    if(guild_id==0){

        sprintf(text_out, "%cyou cannot use #GM if you are not a member of a guild", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    //check sender has permission to send #IG
    if(clients.client[connection]->gm_permission==FALSE){

        sprintf(text_out, "%cyou do not have #GM permission in this guild", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    // send to senders guild channel
    chan_colour=guilds.guild[guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#GM from %s: %s", chan_colour, clients.client[connection]->char_name, message);
    broadcast_guild_channel_chat(guild_id, text_out);

    return SENT;
}

int process_inter_guild_chat(int connection, char *guild_tag, char *message){

    int receiver_guild_id=0;
    int sender_guild_id=clients.client[connection]->guild_id;
    char hash_command_tail[1024]="";
    int chan_colour=0;
    char text_out[1024]="";

    //filter blank messages
    if(strlen(message)==0){

        sprintf(text_out, "%cyou forgot to send a message", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    //check sender is in a guild
    if(sender_guild_id==0){

        sprintf(text_out, "%cyou cannot use #IG if you are not a member of a guild", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    //check sender has permission to send #IG
    if(clients.client[connection]->ig_permission==FALSE){

        sprintf(text_out, "%cyou do not have #IG permission in this guild", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    // convert the guild tag to upper case
    str_conv_upper(guild_tag);

    // check if receiving guild exists
    if(get_guild_id(guild_tag, &receiver_guild_id)==NOT_FOUND) {

        sprintf(text_out, "%cguild [%s] does not exist", c_red3+127, guild_tag);
        send_server_text(connection, CHAT_SERVER, text_out);
        return NOT_SENT;
    }

    // broadcast to senders guild channel
    chan_colour=guilds.guild[sender_guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#IG [sent by %s to %s]: %s", chan_colour, clients.client[connection]->char_name, guilds.guild[receiver_guild_id]->guild_tag, hash_command_tail);
    broadcast_guild_channel_chat(sender_guild_id, text_out);

    // broadcast to receiver guild channel
    chan_colour=guilds.guild[receiver_guild_id]->guild_chan_text_colour;
    sprintf(text_out, "%c#IG [sent by %s from %s]: %s", chan_colour, clients.client[connection]->char_name, guilds.guild[sender_guild_id]->guild_tag, hash_command_tail);
    broadcast_guild_channel_chat(receiver_guild_id, text_out);

    return SENT;
}

int char_in_game(int char_id){

    int i;

    //check if message recipient is in game
    for(i=0; i<clients.max; i++){

        //if message recipient in game then send message
        if(clients.client[i]->character_id==char_id && clients.client[i]->char_status==LOGGED_IN){
            return IN_GAME;
        }
    }

    return NOT_IN_GAME;
}


void send_pm(int connection, char *receiver_name, char *message) {

    char text_out[1024]="";
    int recipient_connection=0;
    char sender_name[80]="";

    // echo message back to sender
    sprintf(text_out, "%c[PM to %s: %s]", c_orange1+127, receiver_name, message);
    send_server_text(connection, CHAT_PERSONAL, text_out);

    //check that message recipient exits
    if(get_char_data_from_db(receiver_name)==NOT_FOUND) {
        sprintf(text_out, "%ccharacter does not exist\n", c_red1+127);
        send_server_text(connection, CHAT_PERSONAL, text_out);
        return;
    }

    //check if message recipient is in game
    if(char_in_game(character.character_id)==NOT_IN_GAME){

        sprintf(text_out, "%ccharacter is not currently logged on. To send a letter use #LETTER [name] [message]", c_red2+127);
        send_server_text(connection, CHAT_PERSONAL, text_out);
        printf("SEND_PM from [%s] to [%s] message [%s] - recipient not in-game\n", sender_name, receiver_name, message);
    }

    sprintf(text_out, "%c[PM from %s: %s]", c_orange1+127, clients.client[connection]->char_name, message);
    send_server_text(recipient_connection, CHAT_PERSONAL, text_out);

    log_event(EVENT_CHAT, text_out);

    return;
}
