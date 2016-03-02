#include <stdio.h> //support sscanf function

#include "clients.h"
#include "server_messaging.h"
#include "colour.h"
#include "maps.h"
#include "guilds.h"
#include "server_protocol_functions.h"

int hash_jump(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    int map_id, x, y, map_tile=0;

    if(sscanf(text, "%*s %i %i %i", &map_id, &x, &y)==3){

        //player specifies location

        if(map_exists(map_id)==false){

            send_text(socket, CHAT_SERVER, "%cmap id does not exist", c_red3+127);
            return 0;
        }

        //check that coordinates are in bounds
        int map_axis=maps.map[map_id].map_axis;
        if(x>map_axis || y>map_axis || x<0 || y<0){

            send_text(socket, CHAT_SERVER, "%ccordinates must be between 0 and %i", c_red3+127, map_axis);
            return 0;
        }

        //calculate map tile
        map_tile=get_tile(x, y, map_id);
    }
    else if(sscanf(text, "%*s %i", &map_id)==1){

        //find first walkable location

        if(map_exists(map_id)==false){

            send_text(socket, CHAT_SERVER, "%cmap id does not exist", c_red3+127);
            return 0;
        }

        bool found;

        for(int i=0; i<(int)maps.map[map_id].height_map_size; i++){

            if(maps.map[map_id].height_map[i]>0){

                map_tile=i;
                found=true;
                break;
            }
        }

        if(found==false){

            send_text(socket, CHAT_SERVER, "%cThere are no walkable tiles on that map", c_red3+127);
            return 0;
        }
    }
    else {

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #JUMP [map id] [x] [y] or #JUMP [map id]", c_red3+127);
        return 0;
    }

    //move char
    if(move_char_between_maps(actor_node, map_id, map_tile)==false){

        send_text(socket, CHAT_SERVER, "%cjump failed", c_red3+127);
        return 0;
    }

    send_text(socket, CHAT_SERVER, "%cYou jumped to map %s tile %i", c_green3+127, maps.map[map_id].map_name, map_tile);
    return 0;
}


int hash_ops_create_guild(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char guild_name[80];
    char guild_tag[GUILD_TAG_LENGTH];
    int permission_level=0;

    if(sscanf(text, "%*s %s %s %i", guild_name, guild_tag, &permission_level)!=3){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_CREATE_GUILD [guild name][guild tag][permission level]", c_red3+127);
        return 0;
    }

    create_guild(socket, guild_name, guild_tag, permission_level);
    return 0;
}


int hash_ops_appoint_guild_member(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char char_name[MAX_CHAR_NAME_LEN]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", char_name, guild_tag)!=2){

        send_text(socket, CHAT_SERVER, "you need to use the format #OPS_APPOINT_GUILD_MEMBER [character name][guild tag]", c_red3+127);
        return 0;
    }

    join_guild(actor_node, char_name, guild_tag);
    return 0;
}


int hash_server_message(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char message[1024]="";

    //scans the string from the second element onwards
    if(sscanf(text, "%*s %[^\n]", message)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SERVER_MESSAGE [message] or #SM [message]", c_red3+127);
        return 0;
    }

    //broadcast message to all clients
    broadcast_server_message(message);

    return 0;
}


int hash_ops_change_guild_member_rank(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char char_name[MAX_CHAR_NAME_LEN]="";
    char guild_tag[GUILD_TAG_LENGTH]="";
    int guild_rank=0;

    if(sscanf(text, "%*s %s %s %i", char_name, guild_tag, &guild_rank)!=3){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_CHANGE_GUILD_MEMBER_RANK [character_name][guild_tag][rank]", c_red3+127);
        return 0;
    }

    change_guild_rank(actor_node, char_name, guild_tag, guild_rank);
    return 0;
}


int hash_ops_change_guild_permission(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char guild_tag[GUILD_TAG_LENGTH]="";
    int permission_level=0;

    if(sscanf(text, "%*s %s %i", guild_tag, &permission_level)!=2){

        send_text(socket, CHAT_SERVER, "you need to use the format #OPS_CHANGE_GUILD_PERMISSION [guild tag][permission level]", c_red3+127);
        return 0;
    }

    change_guild_permission(actor_node, guild_tag, permission_level);
    return 0;
}


int hash_ops_kick_guild_member(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char char_name[MAX_CHAR_NAME_LEN]="";
    char guild_tag[GUILD_TAG_LENGTH]="";

    if(sscanf(text, "%*s %s %s", guild_tag, char_name)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OPS_KICK_GUILD_MEMBER [guild tag][character name]", c_red3+127);
        return 0;
    }

    kick_guild_member(actor_node, guild_tag, char_name);
    return 0;
}
