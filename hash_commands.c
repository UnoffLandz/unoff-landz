#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "string_functions.h"
#include "protocol.h"
#include "broadcast.h"
#include "files.h"
#include "chat.h"

void save_data(int connection){

    int sock=clients.client[connection]->sock;
    int char_id=clients.client[connection]->character_id;
    char text_out[1024]="";

    if(clients.client[connection]->status==LOGGED_IN){

        save_character(characters.character[char_id]->char_name, char_id);

        sprintf(text_out, "%cyour character data was saved", c_green3+127);
        send_server_text(sock, CHAT_SERVER, text_out);
    }
}

int process_hash_commands(int connection, char *text){

    int i=0;
    char hash_command[1024]="";
    char hash_command_tail[1024]="";
    char text_out[1024]="";
    int sock=clients.client[connection]->sock;
    int char_id=clients.client[connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
    int channel_number=0;

    int command_parts=count_str_island(text);

    get_str_island(text, hash_command, 1);
    str_conv_upper(hash_command);

    printf("HASH COMMAND [%s]\n", text);

/***************************************************************************************************/

    if(strcmp(hash_command, "#IL")==0) {
        //printf("#IL\n");
        return HASH_CMD_UNSUPPORTED;
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#SAVE")==0){
        //printf("#SAVE\n");
        save_data(connection);
        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#EXPIRE")==0){
        //printf("#EXPIRE\n");
        return HASH_CMD_UNSUPPORTED;
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#GM")==0){

        switch(process_guild_chat(connection, text)){

            case GM_INVALID:
                //malformed #GM attempt
                sprintf(text_out, "%cyou need to use the format #GM [message]", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case GM_NO_GUILD:
                // GM attempt by non guild member
                sprintf(text_out, "%cyou need to be a member of a guild to use #GM", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case GM_NO_PERMISSION:
                //GM attempt by muted guild member
                sprintf(text_out, "%cyou have been muted on this guild channel", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
             break;

            case GM_SENT:
                printf("#GM sent from %s to guild %s: %s\n", characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, text);
            break;

            default:
                printf("unknown result from function process_guild_chat\n");
            break;
        }

        return HASH_CMD_FAILED;

     }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#IG")==0){

        switch(process_inter_guild_chat(connection, text)){

            case IG_NOT_AVAILABLE:
                sprintf(text_out, "%cyou cannot use #IG if you are not a member of a guild", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case IG_NO_PERMISSION:
                sprintf(text_out, "%cyou do not have #IG permission in this guild", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case IG_INVALID_GUILD:
                sprintf(text_out, "%cthe guild you are attempting to contact does not exist", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case IG_MALFORMED:
                sprintf(text_out, "%cyou need to use the format #IG [guild tag] [message]", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            break;

            case IG_SENT:
                printf("#IG sent from %s of %s to guild %s: %s\n", characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, guilds.guild[i]->guild_tag, hash_command_tail);
            break;

            default:
                printf("unknown result from function process_inter_guild_chat\n");
            break;
        }

        return HASH_CMD_FAILED;
    }

/***************************************************************************************************/

    else if(strcmp(hash_command, "#JC")==0){

        //check that #JC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts==2) {

            // split the #JC command into channel number element
            get_str_island(text, hash_command_tail, 2);

            //convert command number to an integer value
            channel_number=atoi(hash_command_tail);

            switch (join_channel(connection, channel_number)){

                case NO_FREE_CHANNEL_SLOTS:
                    sprintf(text_out, "%cyou can only have three open channels", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case CHANNEL_BARRED:
                    sprintf(text_out, "%cyou are barred from this channel", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                 break;

                case CHANNEL_NOT_OPEN:
                    sprintf(text_out, "%cchannel is not open", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case CHANNEL_SYSTEM:
                    sprintf(text_out, "%cchannel is reserved for system use", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case CHANNEL_INVALID:
                    sprintf(text_out, "%cinvalid channel number", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case CHANNEL_JOINED:

                    // need to echo back to player which channel was just joined and its description etc
                    sprintf(text_out, "%cYou joined channel %s", c_green3+127, channels.channel[channel_number]->channel_name);
                    send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);

                    sprintf(text_out, "%cDescription : %s", c_green2+127, channels.channel[channel_number]->description);
                    send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);

                    sprintf(text_out, "%cIn channel :", c_green1+127);
                    send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);

                    //echo which other players are in the chan
                    list_clients_in_chan(connection, channel_number);

                    // notify other channel users that player has joined on
                    sprintf(text_out, "%c%s JOINED THE CHANNEL", c_green3+127, characters.character[char_id]->char_name);
                    broadcast_channel_chat(channel_number, text_out);
                    //broadcast_raw_text_packet(connection, channel_number, CHAT_SERVER, text_out);

                break;

                default:
                    //Should catch CHANNEL_UNKNOWN
                    sprintf(text_out, "%cyou tried to join an unknown channel type", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);

                    printf("attempt to join channel number [%i]. Type unknown\n", channel_number);
                break;
            }

        }
        else {
            sprintf(text_out, "%cyou need to use the format #JC [channel number]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);
        }

        //debug_channels(char_id);

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#LC")==0){

        //check that #LC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts==2) {

            // split the #JC command into channel number element
            get_str_island(text, hash_command_tail, 2);

            //convert command number to an integer value
            channel_number=atoi(hash_command_tail);

            switch (leave_channel(connection, channel_number)){

                case CHANNEL_INVALID:
                    sprintf(text_out, "%cinvalid channel number", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case NOT_IN_CHANNEL:
                    sprintf(text_out, "%cyou aren't in that channels", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                break;

                case CHANNEL_LEFT:
                    // need to echo back to player which channel was left
                    sprintf(text_out, "%cyou left channel %s", c_green3+127, channels.channel[channel_number]->channel_name);
                    send_server_text(clients.client[connection]->sock, CHAT_SERVER, text_out);

                    // notify other channel users that player has left channel
                    sprintf(text_out, "%c%s LEFT THE CHANNEL", c_green3+127, characters.character[char_id]->char_name);
                    broadcast_channel_chat(channel_number, text_out);
                    //broadcast_raw_text_packet(connection, channel_number, CHAT_SERVER, text_out);
                break;
            }
        }
        else {
            sprintf(text_out, "%cyou need to use the format #LC [channel number]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);
        }

        //debug_channels(char_id);

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    /*
        else if (strcmp(hash_command, "#SHUT_DOWN")==0){
            sprintf(text_out, "%cSHUTTING DOWN", c_red3+127);
        broadcast_raw_text_packet(connection, 0, CHAT_SERVER, text_out, clients, characters, maps, messages, guilds);
    }*/
/***************************************************************************************************/

    else if (strcmp(hash_command, "#LCD")==0){

        sprintf(text_out, "/n%cNumber  Channel              Description", c_blue1+127);
        send_raw_text_packet(sock, CHAT_SERVER, text_out);

        for(i=0; i<channels.max; i++){

            if(channels.channel[i]->chan_type!=CHAN_VACANT) {
                sprintf(text_out, "%c -%20s -%40s", c_blue1+127, channels.channel[i]->channel_name, channels.channel[i]->description);
                send_raw_text_packet(sock, CHAT_SERVER, text_out);
            }
        }
        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#LCC")==0){

        channel_number=characters.character[char_id]->chan[characters.character[char_id]->active_chan];

        list_clients_in_chan(connection, channel_number);
        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    printf("UNKNOWN HASH COMMAND [%s] [%s]\n", hash_command, hash_command_tail);
    return HASH_CMD_UNKNOWN;
}


