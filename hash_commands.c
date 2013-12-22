#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "global.h"
#include "string_functions.h"
#include "protocol.h"
#include "broadcast.h"
#include "files.h"
#include "chat.h"
#include "datetime_functions.h"


int char_type[12]={HUMAN_FEMALE, HUMAN_MALE, ELF_FEMALE, ELF_MALE, DWARF_FEMALE, DWARF_MALE, GNOME_FEMALE, GNOME_MALE, ORCHAN_FEMALE, ORCHAN_MALE, DRAEGONI_FEMALE, DRAEGONI_MALE};
int char_race[12]={HUMAN, HUMAN, ELF, ELF, DWARF, DWARF, GNOME, GNOME, ORCHAN, ORCHAN, DRAEGONI, DRAEGONI};
int char_gender[12]={FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE};

char race_type[12][20]={"HUMAN", "ELF", "DWARF", "GNOME", "ORCHAN", "DRAEGONI"};
char gender_type[2][10]={"FEMALE", "MALE"};
char char_status_type[3][10]={"ALIVE", "DEAD", "BANNED"};

void save_data(int connection){

    int sock=connection;
    int char_id=clients.client[connection]->character_id;
    char text_out[1024]="";

    if(clients.client[connection]->status==LOGGED_IN){

        save_character(characters.character[char_id]->char_name, char_id);

        sprintf(text_out, "%cyour character data was saved", c_green3+127);
        send_server_text(sock, CHAT_SERVER, text_out);
    }
}

int rename_char(int connection, char *new_char_name){

    int i=0;

    char old_file_name[80]="";
    char new_file_name[80]="";
    int char_id=clients.client[connection]->character_id;
    FILE *file;

    //check that no existing char has the new name
    if(get_char_id(new_char_name)!=CHAR_NOT_FOUND) return CHAR_RENAME_FAILED_DUPLICATE;

    //rename the ply file name
    sprintf(old_file_name, "%s.ply", characters.character[char_id]->char_name);
    sprintf(new_file_name, "%s.ply", new_char_name);

    rename(old_file_name, new_file_name); // *** need to test result to check for errors

    //change name
    strcpy(characters.character[char_id]->char_name, new_char_name);

    //create a temp character.lst file
    if((file=fopen(TEMP_FILE, "w"))==NULL){
        perror("unable to open temp file in function rename_char");
        exit(EXIT_FAILURE);
    }

    for(i=0; i<characters.max; i++){

        if(!fprintf(file, "%s\n", characters.character[i]->char_name)){
            printf("error adding to temp character list\n");
            perror("rename_char");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    //delete old character list file
    remove(CHARACTER_LIST_FILE); // *** TO DO test result for error

    //rename the temp character.lst file
    rename(TEMP_FILE, CHARACTER_LIST_FILE); // *** TO DO need to test result to check for errors

    // add this char to each connected client
    broadcast_add_new_enhanced_actor_packet(connection);

    return CHAR_RENAME_SUCCESS;
}

int process_hash_commands(int connection, char *text){

    int i=0;
    char hash_command[1024]="";
    char hash_command_tail[1024]="";
    char text_out[1024]="";
    int sock=connection;
    int char_id=clients.client[connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
    int channel_number=0;
    int new_map_tile=0;
    char old_char_name[80]="";
    char time_stamp_str[9]="";
    char date_stamp_str[11]="";

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

    else if(strcmp(hash_command, "#BEAM_ME")==0){

        if(command_parts!=1){
            sprintf(text_out, "%cyou need to use the format #BEAM_ME", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        new_map_tile=get_nearest_unoccupied_tile(BEAM_ME_MAP, BEAM_ME_TILE);

        move_char_between_maps(connection, BEAM_ME_MAP, new_map_tile);

        return HASH_CMD_EXECUTED;
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

    else if(strcmp(hash_command, "#NAME_CHANGE")==0){

        //check that #NAME_CHANGE command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #NAME_CHANGE [new name]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        get_str_island(text, hash_command_tail, 2);

        strcpy(old_char_name, characters.character[char_id]->char_name);

        if(rename_char(char_id, hash_command_tail)==CHAR_RENAME_FAILED_DUPLICATE){
            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(sock, CHAT_SERVER, text_out);

            sprintf(text_out, "invalid name change attempt for char[%s] to [%s]\n", old_char_name, hash_command_tail);
            log_event(EVENT_SESSION, text_out);

            return HASH_CMD_ABORTED;
        }

        sprintf(text_out, "%cIn the future you'll need to purchase a name change token to do this", c_yellow1+127);
        send_raw_text_packet(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGratz. You just changed your character name to %s", c_green1+127, characters.character[char_id]->char_name);
        send_raw_text_packet(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "name change for char[%s] to [%s]\n", old_char_name, characters.character[char_id]->char_name);
        log_event(EVENT_SESSION, text_out);

        return HASH_CMD_EXECUTED;
    }

/***************************************************************************************************/

    else if(strcmp(hash_command, "#DETAILS")==0){

        //check that #NAME_CHANGE command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #DETAILS [character name]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        get_str_island(text, hash_command_tail, 2);

        //check that the char exists
        i=get_char_id(hash_command_tail);

        if(i==CHAR_NOT_FOUND) {
            sprintf(text_out, "%cthat character does not exist", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        //send details to client
        sprintf(text_out, "\n");
        send_server_text(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "%cCharacter    :%s", c_green3+127, characters.character[i]->char_name);
        send_server_text(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "%cRace         :%s", c_green3+127, race_type[char_race[characters.character[i]->char_type]]);
        send_server_text(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGender       :%s", c_green3+127, gender_type[char_gender[characters.character[i]->char_type]]);
        send_server_text(sock, CHAT_SERVER, text_out);

        get_time_stamp_str(characters.character[i]->char_created, time_stamp_str);
        get_date_stamp_str(characters.character[i]->char_created, date_stamp_str);
        sprintf(text_out, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_server_text(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "%cStatus       :%s", c_green3+127, char_status_type[characters.character[i]->char_status]);
        send_server_text(sock, CHAT_SERVER, text_out);

        get_time_stamp_str(characters.character[i]->last_in_game, time_stamp_str);
        get_date_stamp_str(characters.character[i]->last_in_game, date_stamp_str);
        sprintf(text_out, "%cLast in-game :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_server_text(sock, CHAT_SERVER, text_out);

        get_time_stamp_str(characters.character[i]->joined_guild, time_stamp_str);
        get_date_stamp_str(characters.character[i]->joined_guild, date_stamp_str);
        sprintf(text_out, "%cGuild        :%s joined %s %s", c_green3+127, guilds.guild[characters.character[i]->guild_id]->guild_name, date_stamp_str, time_stamp_str);
        send_server_text(sock, CHAT_SERVER, text_out);

        sprintf(text_out, "\n");
        send_server_text(sock, CHAT_SERVER, text_out);

        return HASH_CMD_EXECUTED;

    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#GM")==0){

        switch(process_guild_chat(connection, text)){

            case GM_INVALID:
                //malformed #GM attempt
                sprintf(text_out, "%cyou need to use the format #GM [message]", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_EXECUTED;
            break;

            case GM_NO_GUILD:
                // GM attempt by non guild member
                sprintf(text_out, "%cyou need to be a member of a guild to use #GM", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_EXECUTED;
            break;

            case GM_NO_PERMISSION:
                //GM attempt by muted guild member
                sprintf(text_out, "%cyou have been muted on this guild channel", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                 return HASH_CMD_EXECUTED;
             break;

            case GM_SENT:
                printf("#GM sent from %s to guild %s: %s\n", characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, text);
                return HASH_CMD_EXECUTED;
            break;
        }

        sprintf(text_out, "#JC [%s] failed. Unknown result from function process_guild_chat by char [%s]", text, characters.character[char_id]->char_name);
        log_event(EVENT_ERROR, text_out);

        return HASH_CMD_FAILED;
     }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#IG")==0){

        switch(process_inter_guild_chat(connection, text)){

            case IG_NOT_AVAILABLE:
                sprintf(text_out, "%cyou cannot use #IG if you are not a member of a guild", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case IG_NO_PERMISSION:
                sprintf(text_out, "%cyou do not have #IG permission in this guild", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case IG_INVALID_GUILD:
                sprintf(text_out, "%cthe guild you are attempting to contact does not exist", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case IG_MALFORMED:
                sprintf(text_out, "%cyou need to use the format #IG [guild tag] [message]", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case IG_SENT:
                printf("#IG sent from %s of %s to guild %s: %s\n", characters.character[char_id]->char_name, guilds.guild[guild_id]->guild_tag, guilds.guild[i]->guild_tag, hash_command_tail);
                return HASH_CMD_EXECUTED;
            break;
        }

        sprintf(text_out, "#IG [%s] failed. Unknown result from function process_inter_guild_chat by char [%s]", text, characters.character[char_id]->char_name);
        log_event(EVENT_ERROR, text_out);

        return HASH_CMD_FAILED;
    }

/***************************************************************************************************/

    else if(strcmp(hash_command, "#JC")==0){

        //check that #JC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #JC [channel number]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        // split the #JC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert command number to an integer value
        channel_number=atoi(hash_command_tail);

        switch (join_channel(connection, channel_number)){

            case NO_FREE_CHANNEL_SLOTS:
                sprintf(text_out, "%cyou can only have three open channels", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_BARRED:
                sprintf(text_out, "%cyou are barred from that channel", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_NOT_OPEN:
                sprintf(text_out, "%cThat channel is not open", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_SYSTEM:
                sprintf(text_out, "%cchannel is reserved for system use", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_INVALID:
                sprintf(text_out, "%cyou tried to join an unknown channel type", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_JOINED:

                // need to echo back to player which channel was just joined and its description etc
                sprintf(text_out, "%cYou joined channel %s", c_green3+127, channels.channel[channel_number]->channel_name);
                send_server_text(connection, CHAT_SERVER, text_out);

                sprintf(text_out, "%cDescription : %s", c_green2+127, channels.channel[channel_number]->description);
                send_server_text(connection, CHAT_SERVER, text_out);

                sprintf(text_out, "%cIn channel :", c_green1+127);
                send_server_text(connection, CHAT_SERVER, text_out);

                //echo which other players are in the chan
                list_clients_in_chan(connection, channel_number);

                return HASH_CMD_EXECUTED;
            break;
        }

        sprintf(text_out, "#JC [%s] failed. Unknown result from function join_channel by char [%s]", text, characters.character[char_id]->char_name);
        log_event(EVENT_ERROR, text_out);

        return HASH_CMD_FAILED;
    }
/***************************************************************************************************/

    else if(strcmp(hash_command, "#LC")==0){

        //check that #LC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #LC [channel number]", c_red3+127);
            send_server_text(sock, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        // split the #JC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert command number to an integer value
        channel_number=atoi(hash_command_tail);

        switch (leave_channel(connection, channel_number)){

            case CHANNEL_INVALID:
                sprintf(text_out, "%cinvalid channel number", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case NOT_IN_CHANNEL:
                sprintf(text_out, "%cyou aren't in that channels", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);

                return HASH_CMD_ABORTED;
            break;

            case CHANNEL_LEFT:

                // need to echo back to player which channel was left
                sprintf(text_out, "%cyou left channel %s", c_green3+127, channels.channel[channel_number]->channel_name);
                send_server_text(connection, CHAT_SERVER, text_out);

                return HASH_CMD_EXECUTED;
            break;
        }

        sprintf(text_out, "#LC [%s] failed. Unknown result from function leave_channel by char [%s]", text, characters.character[char_id]->char_name);
        log_event(EVENT_ERROR, text_out);

        return HASH_CMD_FAILED;
    }
/***************************************************************************************************/

    /*
        else if (strcmp(hash_command, "#SHUT_DOWN")==0){
            sprintf(text_out, "%cSHUTTING DOWN", c_red3+127);
        broadcast_raw_text_packet(connection, 0, CHAT_SERVER, text_out, clients, characters, maps, messages, guilds);
    }*/
/***************************************************************************************************/

    else if (strcmp(hash_command, "#LCD")==0){

        sprintf(text_out, "\n%cNo   Channel    Description", c_blue1+127);
        send_raw_text_packet(sock, CHAT_SERVER, text_out);

        for(i=0; i<channels.max; i++){

            if(channels.channel[i]->chan_type!=CHAN_VACANT) {
                sprintf(text_out, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channels.channel[i]->channel_name, channels.channel[i]->description);
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

    sprintf(text_out, "Unknown #command [%s] by char [%s]", text, characters.character[char_id]->char_name);
    log_event(EVENT_SESSION, text_out);

    return HASH_CMD_UNKNOWN;
}


