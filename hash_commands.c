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
#include "character_movement.h"
#include "motd.h"
#include "hash_commands.h"
#include "log_in.h"
#include "database.h"

int char_type[12]={HUMAN_FEMALE, HUMAN_MALE, ELF_FEMALE, ELF_MALE, DWARF_FEMALE, DWARF_MALE, GNOME_FEMALE, GNOME_MALE, ORCHAN_FEMALE, ORCHAN_MALE, DRAEGONI_FEMALE, DRAEGONI_MALE};
int char_race[12]={HUMAN, HUMAN, ELF, ELF, DWARF, DWARF, GNOME, GNOME, ORCHAN, ORCHAN, DRAEGONI, DRAEGONI};
int char_gender[12]={FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE, FEMALE, MALE};

//char race_type[6][20]={"HUMAN", "ELF", "DWARF", "GNOME", "ORCHAN", "DRAEGONI"};
char gender_type[2][10]={"FEMALE", "MALE"};
char char_status_type[3][10]={"ALIVE", "DEAD", "BANNED"};

void save_data(int connection){

    int sock=connection;
    char text_out[1024]="";

    if(clients.client[connection]->status==LOGGED_IN){

        //save_character(clients.client[connection]->char_name, char_id);

        sprintf(text_out, "%cyour character data was saved", c_green3+127);
        send_server_text(sock, CHAT_SERVER, text_out);
    }
}

int rename_char(int connection, char *new_char_name){

    //check that no existing char has the new name
    if(get_char_data_from_db(new_char_name)!=NOT_FOUND) return CHAR_RENAME_FAILED_DUPLICATE;

    //update char name and save
    strcpy(clients.client[connection]->char_name, new_char_name);
    update_db_char_name(connection);

    // add this char to each connected client
    broadcast_add_new_enhanced_actor_packet(connection);

    return CHAR_RENAME_SUCCESS;
}

int process_hash_commands(int connection, char *text){

    int i=0;
    char hash_command[80]="";
    char hash_command_tail[80]="";
    char text_out[80]="";
    char guild_tag[80]="";
    int chan_id=0;
    int new_map_tile=0;
    char old_char_name[80]="";
    char time_stamp_str[9]="";
    char date_stamp_str[11]="";
    char message[80]="";

    int command_parts=count_str_island(text);

    get_str_island(text, hash_command, 1);
    str_conv_upper(hash_command);

    //log all hash commands
    sprintf(text_out, "client [%i] character [%s] command [%s]", connection, clients.client[connection]->char_name, text);
    log_event(EVENT_SESSION, text_out);

/***************************************************************************************************/
    if(strcmp(hash_command, "#IL")==0) {

        return HASH_CMD_UNSUPPORTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#MOTD")==0){

        send_motd_file(connection);
        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#BEAM_ME")==0){

        if(command_parts!=1){
            sprintf(text_out, "%cyou need to use the format #BEAM_ME or #BEAM ME", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        //if char is moving when protocol arrives, cancel rest of path
        clients.client[connection]->path_count=0;

        //ensure char doesn't beam on top of another char
        new_map_tile=get_nearest_unoccupied_tile(START_MAP_ID, START_MAP_TILE);

        move_char_between_maps(connection, START_MAP_ID, new_map_tile);

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#BEAM")==0){

        if(command_parts!=2){
            sprintf(text_out, "%cyou need to use the format #BEAM_ME or #BEAM ME", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        get_str_island(text, hash_command_tail, 2);
        str_conv_upper(hash_command_tail);

        if(strcmp(hash_command_tail, "ME")!=0) return HASH_CMD_ABORTED;

        //if char is moving when protocol arrives, cancel rest of path
        clients.client[connection]->path_count=0;

        //ensure char doesn't beam on top of another char
        new_map_tile=get_nearest_unoccupied_tile(START_MAP_ID, START_MAP_TILE);

        move_char_between_maps(connection, START_MAP_ID, new_map_tile);

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    /*
    else if(strcmp(hash_command, "#SAVE")==0){
        //printf("#SAVE\n");
        save_data(connection);
        return HASH_CMD_EXECUTED;
    }
    */
/***************************************************************************************************/

    else if(strcmp(hash_command, "#EXPIRE")==0){

        return HASH_CMD_UNSUPPORTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#NAME_CHANGE")==0){

        //check that #NAME_CHANGE command is properly formed (should have 2 parts delimited by an underline).
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #NAME_CHANGE [new name]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        get_str_island(text, hash_command_tail, 2);

        strcpy(old_char_name, clients.client[connection]->char_name);

        if(rename_char(connection, hash_command_tail)==CHAR_RENAME_FAILED_DUPLICATE){

            sprintf(text_out, "%cSorry, but that character name already exists", c_red1+127);
            send_raw_text_packet(connection, CHAT_SERVER, text_out);

            sprintf(text_out, "invalid name change attempt for char[%s] to [%s]\n", old_char_name, hash_command_tail);
            log_event(EVENT_SESSION, text_out);

            return HASH_CMD_ABORTED;
        }

        sprintf(text_out, "%cIn the future you'll need to purchase a name change token to do this", c_yellow1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGratz. You just changed your character name to %s", c_green1+127, clients.client[connection]->char_name);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "name change for char[%s] to [%s]\n", old_char_name, clients.client[connection]->char_name);
        log_event(EVENT_SESSION, text_out);

        return HASH_CMD_EXECUTED;
    }

/***************************************************************************************************/

    else if(strcmp(hash_command, "#DETAILS")==0){

        //check that #NAME_CHANGE command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {

            sprintf(text_out, "%cyou need to use the format #DETAILS [character name]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        get_str_island(text, hash_command_tail, 2);

        //check that the char exists
        i=get_char_data_from_db(hash_command_tail);

        if(i==NOT_FOUND) {

            sprintf(text_out, "%cthat character does not exist", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        //send details to client
        sprintf(text_out, "\n");
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cCharacter    :%s", c_green3+127, character.char_name);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cRace         :%s", c_green3+127, race[character.char_type].race_name);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cDescription  :%s", c_green3+127, race[character.char_type].race_description);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGender       :%s", c_green3+127, gender_type[char_gender[character.char_type]]);
        send_server_text(connection, CHAT_SERVER, text_out);

        get_time_stamp_str(character.char_created, time_stamp_str);
        get_date_stamp_str(character.char_created, date_stamp_str);
        sprintf(text_out, "%cDate Created :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cStatus       :%s", c_green3+127, char_status_type[character.char_status]);
        send_server_text(connection, CHAT_SERVER, text_out);

        get_time_stamp_str(character.last_in_game, time_stamp_str);
        get_date_stamp_str(character.last_in_game, date_stamp_str);
        sprintf(text_out, "%cLast in-game :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cCharacter Age:%i", c_green3+127, character.time_played / GAME_YEAR);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "%cGuild        :%s", c_green3+127, guilds.guild[character.guild_id]->guild_name);
        send_server_text(connection, CHAT_SERVER, text_out);

        get_time_stamp_str(character.joined_guild, time_stamp_str);
        get_date_stamp_str(character.joined_guild, date_stamp_str);
        sprintf(text_out, "%cJoined       :%s %s", c_green3+127, date_stamp_str, time_stamp_str);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "\n");
        send_server_text(connection, CHAT_SERVER, text_out);

        return HASH_CMD_EXECUTED;
    }

/***************************************************************************************************/
    else if(strcmp(hash_command, "#GM")==0){

        // check that #GM command is properly formed (should have 2 parts delimited by space)
        if(command_parts<2) {

            sprintf(text_out, "%cyou need to use the format #GM [message]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        //split the #GM command into separate elements
        get_str_island(text, hash_command_tail, 2);

        //send the message
        if(process_guild_chat(connection, message)==NOT_SENT){
            return HASH_CMD_ABORTED;
        }

        return HASH_CMD_EXECUTED;
    }

/***************************************************************************************************/
    else if(strcmp(hash_command, "#IG")==0){

        //check that #IG command is properly formed (should have 3 parts delimited by spaces)
        if(command_parts<3) {

            sprintf(text_out, "%cyou need to use the format #IG [guild tag] [message]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return HASH_CMD_ABORTED;
        }

        // split the #IG command into separate elements
        get_str_island(text, guild_tag, 2);
        get_str_island(text, message, 3);

        //send the message
        if(process_inter_guild_chat(connection, guild_tag, message)==NOT_SENT){
            return HASH_CMD_ABORTED;
        }

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#JC")==0 || strcmp(hash_command, "#JOIN_CHANNEL")==0){

        //check that #JC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #JC [channel number]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        // split the #JC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert channel number into an integer value
        chan_id=atoi(hash_command_tail);

        //join the channel
        if(join_channel(connection, chan_id)==CHANNEL_NOT_JOINED) {
            return HASH_CMD_ABORTED;
        }

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    else if(strcmp(hash_command, "#LC")==0 || strcmp(hash_command, "#LEAVE_CHANNEL")==0){

        //check that #LC command is properly formed (should have 2 parts delimited by a space)
        if(command_parts!=2) {
            sprintf(text_out, "%cyou need to use the format #LC [channel number]", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);

            return HASH_CMD_ABORTED;
        }

        // split the #LC command into channel number element
        get_str_island(text, hash_command_tail, 2);

        //convert channel number into an integer value
        chan_id=atoi(hash_command_tail);

        //leave the channel
        if(leave_channel(connection, chan_id)==CHANNEL_NOT_LEFT){
            return HASH_CMD_ABORTED;
        }

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#TEST")==0){

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#LCD")==0 || strcmp(hash_command, "#LIST_CHANNEL_DATA")==0){

        sprintf(text_out, "\n%cNo   Channel    Description", c_blue1+127);
        send_raw_text_packet(connection, CHAT_SERVER, text_out);

        for(i=0; i<channels.max; i++){

            if(channels.channel[i]->chan_type!=CHAN_VACANT) {
                sprintf(text_out, "%c%i %s %-10s %-30s", c_blue1+127, i, "  ", channels.channel[i]->channel_name, channels.channel[i]->description);
                send_raw_text_packet(connection, CHAT_SERVER, text_out);
            }
        }

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/

    else if (strcmp(hash_command, "#LCC")==0 || strcmp(hash_command, "#LIST_CHANNEL_CHARACTERS")==0){

        chan_id=clients.client[connection]->chan[clients.client[connection]->active_chan];

        list_clients_in_chan(connection, chan_id);

        return HASH_CMD_EXECUTED;
    }
/***************************************************************************************************/
    else {

        sprintf(text_out, "%cThat command isn't supported yet. You may want to tell the game administrator", c_red3+127);
        send_server_text(connection, CHAT_SERVER, text_out);

        sprintf(text_out, "unknown #command [%s]\n", hash_command);
        log_event(EVENT_SESSION, text_out);

        return HASH_CMD_ABORTED;
    }
 }


