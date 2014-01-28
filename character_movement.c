#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> //needed for usec time

#include "global.h"
#include "broadcast.h"
#include "protocol.h"
#include "debug.h"
#include "files.h"
#include "maps.h"
#include "protocol.h"
#include "pathfinding.h"

enum {// return values from function dequeue_step
    DEQUEUE_STEP_ABORT,
    DEQUEUE_STEP_COMPLETE
};

int get_move_command_vector(int cmd, int tile_pos, int map_axis){

    //returns the new tile position after a move_cmd from tile_pos

    int i=0;

    for(i=0; i<8; i++){
        if(cmd==movement_cmd[i]) return tile_pos+vector_x[i] + (vector_y[i]*map_axis);
    }

    return 0;
}

int get_move_command(int tile_pos, int tile_dest, int map_axis){

    //returns the char movement command to move from tile_pos to tile_dest

    int i=0;
    int move=0;
    int move_x=0, move_y=0;

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

        if(vector_x[i]==move_x && vector_y[i]==move_y){
            return movement_cmd[i];
        }
    }

    perror("illegal move in function get_move_command");
    exit(EXIT_FAILURE);

    return 0; //we should never get here
}
/*
void enqueue_move_to(int connection, int tile_dest){

    printf("\nENQUEUE_MOVE_TO %i\n", tile_dest);

    char text_out[1024]="";

    if(clients.client[connection]->path[0]==tile_dest){
        printf("new move_to %i ignored as it duplicates current destination\n", tile_dest);
        return;
    }

    if(clients.client[connection]->move_buffer[clients.client[connection]->move_buffer_size-1]==tile_dest){
        printf("new move_to %i ignored as it duplicates next destination\n", tile_dest);
        return;
    }

    //ignore moves if buffer size will be exceeded
    if(clients.client[connection]->move_buffer_size+1>MOVE_BUFFER_MAX){
        sprintf( text_out, "new move_to %i ignored as move_buffer (%i) is full\n", tile_dest, clients.client[connection]->move_buffer_size);
        perror(text_out);
        log_event(EVENT_ERROR, text_out);
        return;
    }

    // add new move to the end of the move buffer
    clients.client[connection]->move_buffer_size++;
    clients.client[connection]->move_buffer[clients.client[connection]->move_buffer_size-1]=tile_dest;
}
*/

void process_char_move(int connection, time_t current_utime){

    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int current_tile=characters.character[clients.client[connection]->character_id]->map_tile;
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

                //calculate the move_cmd and broadcase to clients
                move_cmd=get_move_command(current_tile, next_tile, map_axis);
                broadcast_actor_packet(connection, move_cmd, next_tile);

                //update char current position and save
                characters.character[clients.client[connection]->character_id]->map_tile=next_tile;
                save_character(characters.character[char_id]->char_name, char_id);
            }
        }
    }
}

/*
void process_char_move(int connection, time_t current_utime){

    int i=0;
    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int current_tile=characters.character[clients.client[connection]->character_id]->map_tile;
    int destination_tile=0;
    int next_tile=0;
    int move_cmd=0;
    char text_out[1024];
    int result=0;

    //if we've reached our destination check to see if there are any more
    if(clients.client[connection]->move_buffer_size>0 && clients.client[connection]->path_count==0){

        //dequeue the next destination
        destination_tile=clients.client[connection]->move_buffer[0];

        clients.client[connection]->move_buffer_size--;

        for(i=0; i<clients.client[connection]->move_buffer_size-1; i++){
            clients.client[connection]->move_buffer[i]=clients.client[connection]->move_buffer[i+1];
        }
        clients.client[connection]->move_buffer[i+1]=0;

        //calculate path to new destination
       result=get_astar_path(connection, current_tile, destination_tile);

        if(result==ASTAR_UNREACHABLE) {
            sprintf(text_out, "%cThe tile you clicked on is unreachable. Try another", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return;
        }

        if(result==ASTAR_ABORT) {
            sprintf(text_out, "%cThe tile you clicked on is too far away. Try another", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return;
        }

        if(result==ASTAR_UNKNOWN) {
            sprintf(text_out, "%cThe tile you clicked on caused an error. Please notify the game administrator", c_red3+127);
            send_server_text(connection, CHAT_SERVER, text_out);
            return;
        }

        printf("got new path\n");
        for(i=0; i<clients.client[connection]->path_count; i++){
            printf("%i %i\n", i, clients.client[connection]->path[i]);
        }

        //reset time of last move to zero so the movement is processed without delay
        clients.client[connection]->time_of_last_move=0;
    }

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

                //calculate the move_cmd and broadcase to clients
                move_cmd=get_move_command(current_tile, next_tile, map_axis);
                broadcast_actor_packet(connection, move_cmd, next_tile);

                //update char current position and save
                characters.character[clients.client[connection]->character_id]->map_tile=next_tile;
                save_character(characters.character[char_id]->char_name, char_id);
            }
        }
    }
}
*/

int remove_char_from_map(int connection){

    /** RESULT  : Removes actor from map

        RETURNS : 0=sucess / -1=fail

        PURPOSE : Consolidate all required operations into a resuable function that can be called
                  at login and on map change

        USAGE   : protocol.c process_packet
    */

    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    char text_out[1024]="";

    //check for illegal map
    if(map_id>maps.max || map_id<START_MAP_ID) return ILLEGAL_MAP;

    //broadcast actor removal to other chars on map
    broadcast_remove_actor_packet(connection);

    //remove from local map list
    remove_client_from_map_list(connection, characters.character[char_id]->map_id);

    sprintf(text_out, "char %s removed from map %s", characters.character[char_id]->char_name, maps.map[map_id]->map_name);
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
    int char_id=0;

    // return if tile is not traversable
    if(maps.map[map_id]->height_map[map_tile]<MIN_TRAVERSABLE_VALUE) {
        return TILE_NON_TRAVERSABLE;
    }

    //cycle through actors on map list and see if any are on tile
    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        client_id=maps.map[map_id]->client_list[i];
        char_id=clients.client[client_id]->character_id;

        if(characters.character[char_id]->map_tile==map_tile) return TILE_OCCUPIED;
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

            next_tile=map_tile+vector_x[i]+(vector_y[i]*map_axis*j);

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

    int char_id=clients.client[connection]->character_id;
    char text_out[1024]="";

    //check for illegal maps
    if(new_map_id>maps.max || new_map_id<START_MAP_ID) {
        sprintf(text_out, "attempt to access illegal map (id[%i] map name [%s]) in function add_char_to_map", new_map_id, maps.map[new_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);
        return ILLEGAL_MAP;
    }

    //TODO# bounds check connection and char_id

    //if tile is occupied get nearest unoccupied tile
    characters.character[char_id]->map_tile=get_nearest_unoccupied_tile(new_map_id, map_tile);

    //update map
    characters.character[char_id]->map_id=new_map_id;

    //send new char map to client
    send_change_map(connection, maps.map[new_map_id]->elm_filename);

    //add client to local map list
    add_client_to_map_list(connection, characters.character[char_id]->map_id);

    // add in-game chars to this clients
    send_actors_to_client(connection);

    // add this char to each connected client
    broadcast_add_new_enhanced_actor_packet(connection);

    //#TODO log client map move (time / char_id / originating map id)

    save_character(characters.character[char_id]->char_name, char_id);

    sprintf(text_out, "char %s added to map %s", characters.character[char_id]->char_name, maps.map[new_map_id]->map_name);
    log_event(EVENT_SESSION, text_out);

    printf("move to map [%i] tile [%i]\n", new_map_id, characters.character[char_id]->map_tile);

    return LEGAL_MAP;
}

void move_char_between_maps(int connection, int new_map_id, int new_map_tile){

    /** public function - see header */

    int char_id=clients.client[connection]->character_id;
    int old_map_id=characters.character[char_id]->map_id;
    char text_out[1024]="";

    //check to see if old map is legal and, if not, transport char to Isla Prima
    if(remove_char_from_map(connection)==ILLEGAL_MAP) {

        printf("attempt to leave illegal map_id [%i] map name [%s]\n", old_map_id, maps.map[old_map_id]->map_name);

        sprintf(text_out, "attempt to leave illegal map (id[%i] map name [%s]) in function remove_char_from_map", old_map_id, maps.map[old_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        new_map_id=START_MAP_ID;
        new_map_tile=START_MAP_START_TILE;
    }

    //check to see if new map is legal and, if not, return char to old map
    if(add_char_to_map(connection, new_map_id, new_map_tile)==ILLEGAL_MAP){

        printf("attempt to join illegal map_id [%i] map name [%s]\n", new_map_id, maps.map[new_map_id]->map_name);

        sprintf(text_out, "attempt to join illegal map (id[%i] map name [%s]) in function remove_char_from_map", new_map_id, maps.map[new_map_id]->map_name);
        log_event(EVENT_ERROR, text_out);

        if(add_char_to_map(connection, old_map_id, characters.character[char_id]->map_tile)==ILLEGAL_MAP){

            //if old map and new map are illegal, close down the server
            log_event(EVENT_ERROR, "severe map error in function move_char_between_maps - shutting down server");
            exit(EXIT_FAILURE);
            //#TODO simply remove client rather than crash server
        }

    }

}

