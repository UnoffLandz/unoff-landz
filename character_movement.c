#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> //needed for usec time
#include <string.h>
#include <sys/socket.h> //needed for send function

#include "global.h"
#include "broadcast.h"
#include "protocol.h"
#include "debug.h"
#include "files.h"
#include "maps.h"
#include "protocol.h"
#include "pathfinding.h"
#include "database.h"

void send_change_map(int connection, char *elm_filename){

    unsigned char packet[1024];

    int i;

    int filename_length=strlen(elm_filename)+1; // +1 to include null terminator
    int msb=(filename_length) / 256;
    int lsb=(filename_length) % 256;
    lsb++; // +1 as required by EL protocol

    // calculate packet length
    int packet_length=filename_length+3;

    // construct packet header
    packet[0]=CHANGE_MAP;
    packet[1]=lsb;
    packet[2]=msb;

    // TODO (derekl#2#): convert loop to memcpy    // add packet content
    for(i=3; i<3+filename_length; i++){
        packet[i]=elm_filename[i-3];
    }

    send(connection, packet, packet_length, 0);
}

int get_move_command_vector(int cmd, int tile_pos, int map_axis){

    //returns the new tile position after a move_cmd from tile_pos

    int i=0;

    for(i=0; i<8; i++){
         if(cmd==vector[i].move_cmd) return tile_pos+vector[i].x + (vector[i].y*map_axis);
        //if(cmd==movement_cmd[i]) return tile_pos+vector_x[i] + (vector_y[i]*map_axis);
    }

    return 0;
}

int get_move_command(int tile_pos, int tile_dest, int map_axis){

    //returns the char movement command to move from tile_pos to tile_dest

    int i=0;
    int move=0;
    int move_x=0, move_y=0;
    char text_out[1024]="";

    move=tile_dest-tile_pos;

    if(move==map_axis) {
        move_x=0;
        move_y=1;
    }

    if(move==map_axis+1) {
        move_x=1;
        move_y=1;
    }

    if(move==map_axis-1) {
        move_x=-1;
        move_y=1;
    }

    if(move==(map_axis*-1)) {
        move_x=0;
        move_y=-1;
    }

    if(move==(map_axis*-1)+1) {
        move_x=1;
        move_y=-1;
    }

    if(move==(map_axis*-1)-1) {
        move_x=-1;
        move_y=-1;
    }

    if(move==1) {
        move_x=1;
        move_y=0;
    }

    if(move==-1) {
        move_x=-1;
        move_y=0;
    }

    for(i=0; i<8; i++){

        if(vector[i].x==move_x && vector[i].y==move_y){
        //if(vector_x[i]==move_x && vector_y[i]==move_y){
            //return movement_cmd[i];
            return vector[i].move_cmd;
        }
    }

    sprintf(text_out, "illegal move in function get_move_command position[%i] destination[%i] move[%i]", tile_pos, tile_dest, move);
    log_event(EVENT_ERROR, text_out);

    perror(text_out);
    exit(EXIT_FAILURE);

    return 0;
}

void process_char_move(int connection, time_t current_utime){

    int map_id=clients.client[connection]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int current_tile=clients.client[connection]->map_tile;
    int next_tile=0;
    int move_cmd=0;

    // move actor one step along the path
    if(clients.client[connection]->path_count>0){

        //adjust timer to compensate for wrap-around>
        if(clients.client[connection]->time_of_last_move>current_utime) current_utime+=1000000;

        // check for time of next movement
        if(current_utime>clients.client[connection]->time_of_last_move+290000) {

            //get destination tile from the path queue
            next_tile=clients.client[connection]->path[clients.client[connection]->path_count-1];

            clients.client[connection]->path_count--;

            // filter out moves where position and destination are the same
            if(current_tile!=next_tile){

                printf("move from %i to %i\n", current_tile, next_tile);

                //update the time of move
                gettimeofday(&time_check, NULL);
                clients.client[connection]->time_of_last_move=time_check.tv_usec;

                //calculate the move_cmd and broadcast to clients
                move_cmd=get_move_command(current_tile, next_tile, map_axis);

                broadcast_actor_packet(connection, move_cmd, next_tile);

                //update char current position and save
                clients.client[connection]->map_tile=next_tile;
                update_db_char_position(connection);
             }
        }
    }
}

int remove_char_from_map(int connection){

    /** RESULT  : Removes actor from map

        RETURNS : 0=sucess / -1=fail

        PURPOSE : Consolidate all required operations into a resuable function that can be called
                  at login and on map change

        USAGE   : protocol.c process_packet
    */

    int map_id=clients.client[connection]->map_id;
    char text_out[1024]="";

    //check for illegal map
    if(map_id>maps.max || map_id<START_MAP_ID) return ILLEGAL_MAP;

    //broadcast actor removal to other chars on map
    broadcast_remove_actor_packet(connection);

    //remove from local map list
    remove_client_from_map_list(connection, map_id);

    sprintf(text_out, "char %s removed from map %s", clients.client[connection]->char_name, maps.map[map_id]->map_name);
    log_event(EVENT_SESSION, text_out);

    return LEGAL_MAP;
}

int is_map_tile_occupied(int map_id, int map_tile){

    /**  RESULT  : finds nearest unoccupied tile

        RETURNS : MAP_TILE_UNOCCUPIED / MAP_TILE_OCCUPIED /MAP_TILE_UNTRAVERSABLE

        PURPOSE : To ensure that actors don't jump to occupied tiles

        USAGE   : protocol.c add_char_to_map
    */

    int i=0;
    int client_id=0;

    // return if tile is not traversable
    if(maps.map[map_id]->height_map[map_tile]<MIN_TRAVERSABLE_VALUE) {
        return TILE_NON_TRAVERSABLE;
    }

    //cycle through actors on map list and see if any are on tile
    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        client_id=maps.map[map_id]->client_list[i];

        if(clients.client[client_id]->map_tile==map_tile) return TILE_OCCUPIED;
    }

    return TILE_UNOCCUPIED;
}

int get_nearest_unoccupied_tile(int map_id, int map_tile){

    /** public function - see header */

    int i=0, j=0;
    int next_tile=0;
    int map_axis=maps.map[map_id]->map_axis;

    if(is_map_tile_occupied(map_id, map_tile)==TILE_UNOCCUPIED) {
        return map_tile;
    }

    do{

        //examine all adjacent tiles
        for(i=0; i<8; i++){

            //next_tile=map_tile+vector_x[i]+(vector_y[i]*map_axis*j);
            next_tile=map_tile+vector[i].x+(vector[i].y*map_axis*j);

            // keep with bounds of map
            if(next_tile>0 && next_tile<maps.map[map_id]->height_map_size) {

                //check next best tile
                if(is_map_tile_occupied(map_id, next_tile)==TILE_UNOCCUPIED) {
                    return next_tile;
                }
            }
        }

        //widen search of unoccupied tile
        j++;

    } while(j<10);// give up if no unoccupied tiles within 10 tiles of target

    log_event(EVENT_ERROR, "no unoccupied tile within 10 tiles of target tile");
    exit(EXIT_FAILURE);

    return 0; //Dummy. we should never reach here
}

int add_char_to_map(int connection, int new_map_id, int map_tile){

    /** public function - see header */

    char text_out[1024]="";

    //check for illegal maps
    if(new_map_id>maps.max || new_map_id<START_MAP_ID) {
        sprintf(text_out, "attempt to access illegal map (id[%i] map name [%s]) in function add_char_to_map", new_map_id, maps.map[new_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);
        return ILLEGAL_MAP;
    }

    //if tile is occupied get nearest unoccupied tile
    clients.client[connection]->map_tile=get_nearest_unoccupied_tile(new_map_id, map_tile);

    //update map
    clients.client[connection]->map_id=new_map_id;

    //send new char map to client
    send_change_map(connection, maps.map[new_map_id]->elm_filename);

    //add client to local map list
    add_client_to_map_list(connection, clients.client[connection]->map_id);

    // add in-game chars to this clients
    send_actors_to_client(connection);

    // add this char to each connected client
    broadcast_add_new_enhanced_actor_packet(connection);

    //#TODO log client map move (time / char_id / originating map id)

    sprintf(text_out, "char %s added to map %s", clients.client[connection]->char_name, maps.map[new_map_id]->map_name);
    log_event(EVENT_SESSION, text_out);

    return LEGAL_MAP;
}

void move_char_between_maps(int connection, int new_map_id, int new_map_tile){

    /** public function - see header */

    int old_map_id=clients.client[connection]->map_id;
    char text_out[1024]="";

    //check to see if old map is legal and, if not, transport char to Isla Prima
    if(remove_char_from_map(connection)==ILLEGAL_MAP) {

        printf("attempt to leave illegal map_id [%i] map name [%s]\n", old_map_id, maps.map[old_map_id]->map_name);

        sprintf(text_out, "attempt to leave illegal map (id[%i] map name [%s]) in function remove_char_from_map", old_map_id, maps.map[old_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        new_map_id=START_MAP_ID;
        new_map_tile=START_MAP_TILE;
    }

    //check to see if new map is legal and, if not, return char to old map
    if(add_char_to_map(connection, new_map_id, new_map_tile)==ILLEGAL_MAP){

        printf("attempt to join illegal map_id [%i] map name [%s]\n", new_map_id, maps.map[new_map_id]->map_name);

        sprintf(text_out, "attempt to join illegal map (id[%i] map name [%s]) in function remove_char_from_map", new_map_id, maps.map[new_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        if(add_char_to_map(connection, old_map_id, clients.client[connection]->map_tile)==ILLEGAL_MAP){

            //if old map and new map are illegal, close down the server
            log_event(EVENT_ERROR, "severe map error in function move_char_between_maps - shutting down server");
            exit(EXIT_FAILURE);
            //#TODO simply remove client rather than crash server
        }

    }

    //save char map id and position
    update_db_char_position(connection);
}
