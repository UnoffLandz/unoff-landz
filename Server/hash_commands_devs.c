#include <stdio.h> //supports functions sscanf, printf
#include <string.h> //supports function strlen

#include "clients.h"
#include "maps.h"
#include "server_protocol_functions.h"
#include "server_messaging.h"
#include "colour.h"
#include "string_functions.h"
#include "idle_buffer2.h"


int hash_list_maps(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;

    for(int i=0; i<MAX_MAPS; i++){

        if(strlen(maps.map[i].elm_filename)>0){

            send_text(socket, CHAT_SERVER, "%c%i %s", c_green3+127, i, maps.map[i].elm_filename);
        }
    }

    return 0;
}


int hash_map(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char map_name[80]="";
    int map_id;

    if(sscanf(text, "%*s *s")==-1){

        //if no tail specified in command, default to current map
        map_id=clients.client[actor_node].map_id;
    }
    else if(sscanf(text, "%*s %[^\n]", map_name)==1){

        map_id=get_map_id(map_name);

        if(map_id==-1){

            send_text(socket, CHAT_SERVER, "%cMap does not exist", c_red3+127);
            return 0;
        }
    }
    else {

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #MAP [map_name] or #MAP", c_red3+127);
        return 0;
    }

    get_map_details(actor_node, map_id);
    get_map_developer_details(actor_node, map_id);

    return 0;
}


int hash_set_map_name(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char map_name[80]="";
    int map_id;

    if(sscanf(text, "%*s %i %[^\n]", &map_id, map_name)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SET_MAP_NAME [map id] [map name]", c_red3+127);
        return 0;
    }

    //check map exists
    if(strlen(maps.map[map_id].elm_filename)==0){

        send_text(socket, CHAT_SERVER, "%cthat map id does not exist", c_red3+127);
        return 0;
    }

    //update map array
    strcpy(maps.map[map_id].map_name, map_name);

    //update database
    push_sql_command("UPDATE MAP_TABLE SET MAP_NAME='%s' WHERE MAP_ID=%i", maps.map[map_id].map_name, map_id);

    send_text(socket, CHAT_SERVER, "%cyou changed the name of map %i to %s", c_green3+127, map_id, maps.map[map_id].map_name);

    return 0;
}


int hash_set_map_description(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char map_description[160]="";
    int map_id;

    if(sscanf(text, "%*s %i %[^\n]", &map_id, map_description)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SET_MAP_DESCRIPTION [map id] [map description]", c_red3+127);
        return 0;
    }

    //check map exists
    if(strlen(maps.map[map_id].elm_filename)==0){

        send_text(socket, CHAT_SERVER, "%cthat map id does not exist", c_red3+127);
        return 0;
    }

    //update map array
    strcpy(maps.map[map_id].description, map_description);

    //update database
    push_sql_command("UPDATE MAP_TABLE SET MAP_DESCRIPTION='%s' WHERE MAP_ID=%i", maps.map[map_id].description, map_id);

    send_text(socket, CHAT_SERVER, "%cyou changed the description of map %i to %s", c_green3+127, map_id, maps.map[map_id].description);

    return 0;
}


int hash_set_map_author(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char map_author[80]="";
    int map_id;

    //extract data from text string
    if(sscanf(text, "%*s %i %[^\n]", &map_id, map_author)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SET_MAP_AUTHOR [map id] [map author name]", c_red3+127);
        return 0;
    }

    //check map exists
    if(strlen(maps.map[map_id].elm_filename)==0){

        send_text(socket, CHAT_SERVER, "%cthat map id does not exist", c_red3+127);
        return 0;
    }

    //update map array
    strcpy(maps.map[map_id].author, map_author);

    //update database
    push_sql_command("UPDATE MAP_TABLE SET AUTHOR='%s' WHERE MAP_ID=%i", maps.map[map_id].author, map_id);

    send_text(socket, CHAT_SERVER, "%cyou changed the author of map %i to %s", c_green3+127, map_id, maps.map[map_id].author);

    return 0;
}


int hash_set_map_author_email(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char map_author_email[80]="";
    int map_id;

    //extract data from text string
    if(sscanf(text, "%*s %i %s", &map_id, map_author_email)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SET_MAP_AUTHOR_EMAIL [map id] [map author email address]", c_red3+127);
        return 0;
    }

    //check map exists
    if(strlen(maps.map[map_id].elm_filename)==0){

        send_text(socket, CHAT_SERVER, "%cthat map id does not exist", c_red3+127);
        return 0;
    }

    //update map array
    strcpy(maps.map[map_id].author_email, map_author_email);

    //update database
    push_sql_command("UPDATE MAP_TABLE SET AUTHOR_EMAIL='%s' WHERE MAP_ID=%i", maps.map[map_id].author_email, map_id);

    send_text(socket, CHAT_SERVER, "%cyou changed the author email of map %i to %s", c_green3+127, map_id, maps.map[map_id].author_email);

    return 0;
}


int hash_set_map_development_status(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    int development_status=0;
    int map_id;

    //extract data from text string
    if(sscanf(text, "%*s %i %i", &map_id, &development_status)!=2){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #SET_MAP_DEVELOPMENT_STATUS [map id] [map development status 0-2]", c_red3+127);
        return 0;
    }

    //check map exists
    if(strlen(maps.map[map_id].elm_filename)==0){

        send_text(socket, CHAT_SERVER, "%cthat map id does not exist", c_red3+127);
        return 0;
    }

    //update map array
    maps.map[map_id].development_status=development_status;

    //update database
    push_sql_command("UPDATE MAP_TABLE SET STATUS=%i WHERE MAP_ID=%i", maps.map[map_id].development_status, map_id);

    send_text(socket, CHAT_SERVER, "%cyou changed the development status of map %i to %i", c_green3+127, map_id, maps.map[map_id].development_status);

    return 0;
}


int hash_track(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    char on_off[4]="";

    if(sscanf(text, "%*s %s", on_off)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #TRACK [ON / OFF]", c_red3+127);
        return 0;
    }

    str_conv_upper(on_off);

    if(strcmp(on_off, "ON")==0) {

        send_text(socket, CHAT_SERVER, "%cposition tracking 'on'", c_green3+127);
        clients.client[actor_node].debug_status=DEBUG_TRACK;
    }

    else if(strcmp(on_off, "OFF")==0) {

        send_text(socket, CHAT_SERVER, "%cposition tracking 'off'", c_green3+127);
        clients.client[actor_node].debug_status=DEBUG_OFF;
    }

    else {

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #TRACK [ON / OFF]", c_red3+127);
        return 0;
    }

    return 0;
}


int hash_trace(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;

    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    send_text(socket, CHAT_SERVER, "%cwalkable tile trace (@=%i/%i)", c_green3+127, pos_x, pos_y);

    for(int x=pos_x-20; x<pos_x+20; x++){

        char str[81]="";

        for(int y=pos_y-40; y<pos_y+40; y++){

            int z=(y * map_axis) + x;

            if(z==clients.client[actor_node].map_tile){

                fprintf(stderr, "@");
                strcat(str, "@");
            }
            else {

                if(y>=0 && y<map_axis && x>=0 && x<map_axis){

                    if(maps.map[map_id].height_map[z]==NON_TRAVERSABLE_TILE){

                        fprintf(stderr, "#");
                        strcat(str, "#");
                    }
                    else{

                        fprintf(stderr, ".");
                        strcat(str, ".");
                    }
                }
                else{
                    fprintf(stderr, ":");
                    strcat(str, ":");
                }
            }
        }

        send_text(socket, CHAT_SERVER, "%c%s", c_grey1+127, str);
        fprintf(stderr, "\n");
    }

    return 0;
}


int hash_trace_explore(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    send_text(socket, CHAT_SERVER, "%cexplorable tile trace (@=%i/%i)", c_green3+127, pos_x, pos_y);

    clients.client[actor_node].debug_status=DEBUG_EXPLORE;

    return 0;
}


int hash_trace_path(int actor_node, char *text) {

    /** public function - see header */

    (void)(text);

    int socket=clients.client[actor_node].socket;
    int map_id=clients.client[actor_node].map_id;
    int map_axis=maps.map[map_id].map_axis;
    int pos_x=clients.client[actor_node].map_tile % map_axis;
    int pos_y=clients.client[actor_node].map_tile / map_axis;

    send_text(socket, CHAT_SERVER, "%cpath tile trace (@=%i/%i)", c_green3+127, pos_x, pos_y);

    clients.client[actor_node].debug_status=DEBUG_PATH;

    return 0;
}

int hash_object(int actor_node, char *text) {

    /** public function - see header */

    int socket=clients.client[actor_node].socket;
    int object_id=0;

    //extract data from text string
    if(sscanf(text, "%*s %i", &object_id)!=1){

        send_text(socket, CHAT_SERVER, "%cyou need to use the format #OBJECT [object id]]", c_red3+127);
        return 0;
    }

    //put item in slot 0
    add_to_inventory(actor_node, object_id, 1, 0);

    return 0;
}

