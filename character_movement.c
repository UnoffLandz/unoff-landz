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

int get_heading(int tile_pos, int tile_dest, int map_axis) {

    int x_pos=tile_pos % map_axis;
    int y_pos=tile_pos / map_axis;

    int x_dest=tile_dest % map_axis;
    int y_dest=tile_dest / map_axis;

    //printf("x pos[%i] y pos[%i] - x dest[%i] y dest[%i]\n", x_pos, y_pos, x_dest, y_dest);

    if(x_dest==x_pos && y_dest>y_pos) return NORTH;
    else if(x_dest>x_pos && y_dest>y_pos) return NORTH_EAST;
    else if(x_dest>x_pos && y_dest==y_pos) return EAST;
    else if(x_dest>x_pos && y_dest<y_pos) return SOUTH_EAST;
    else if(x_dest==x_pos && y_dest<y_pos) return SOUTH;
    else if(x_dest<x_pos && y_dest<y_pos) return SOUTH_WEST;
    else if(x_dest<x_pos && y_dest==y_pos) return WEST;
    else if(x_dest<x_pos && y_dest>y_pos) return NORTH_WEST;

    printf("position [%i] position [%i] destination x [%i] destination y [%i]\n", x_pos, y_pos, x_dest, y_dest);
    perror ("illegal heading in function get_heading");
    exit (EXIT_FAILURE);

    return -1; //we never get here
}

void enqueue_move_to(int connection, int tile_dest){

    int move_buffer_size=clients.client[connection]->move_buffer_size;
    int char_id=clients.client[connection]->character_id;

    //ignore moves which duplicate the last buffered destination
    if(tile_dest!=clients.client[connection]->move_buffer[move_buffer_size-1]){

        //ignore moves where destination is the current char postion
        if(tile_dest!=characters.character[char_id]->map_tile){

            /*if there's space in the move buffer, increment the buffer count so as the new destination is added
            to the end. This means that, if buffer is full, the new destination replaces the last destination on
            the buffer */
            if(clients.client[connection]->move_buffer_size+1<MOVE_BUFFER_MAX) {
                clients.client[connection]->move_buffer_size++;
            }
            else {
                printf("move_to buffer full\n");
            }

            clients.client[connection]->move_buffer[clients.client[connection]->move_buffer_size-1]=tile_dest;

        }
        else {
            printf("move_to ignored - duplicate of current position\n");
        }
    }
    else {
        printf("move_to ignored - duplicate of current destination\n");
    }

}

void dequeue_move_to(int connection, int *tile_dest){

    int i=0;

    //remove destination from the start of buffer
    *tile_dest=clients.client[connection]->move_buffer[0];

    //move the remaining buffer content
    for(i=0; i<clients.client[connection]->move_buffer_size; i++){
        clients.client[connection]->move_buffer[i]=clients.client[connection]->move_buffer[i+1];
    }

    //decrement the buffer count
    clients.client[connection]->move_buffer_size--;

    if(clients.client[connection]->move_buffer_size<0){
        perror ("move buffer less than 0 in function dequeue_move");
        exit (EXIT_FAILURE);
    }
}

void dequeue_path_step(int connection, int *destination_tile){

    int path_count=clients.client[connection]->path_count;

    *destination_tile=clients.client[connection]->path[path_count];

    //point path count to next destination tile node
    clients.client[connection]->path_count++;

    if(clients.client[connection]->path_count==clients.client[connection]->path_max-1) {
        clients.client[connection]->path_count=0;
        clients.client[connection]->path_max=0;
    }

    if(clients.client[connection]->path_count>clients.client[connection]->path_max){
        perror ("path count exceeds path max in function dequeue_path_step");
        exit (EXIT_FAILURE);
    }
}

int find_existing_tile(int tile, int queue_count, int queue[]){

    const int TILE_NOT_FOUND=-1;

    int i=0;

    for(i=0; i<queue_count; i++){
        if(tile==queue[i]) return i;
    }

    return TILE_NOT_FOUND;
}

int check_tile_bounds(int position, int vector, int map_id){

    //checks if position+vector will exceed the horizontal or vertical bounds or reach a non-traversable tile

    //check input bounds
    if(vector<0 || vector>7){
        perror ("illegal vector in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    if(position<0 || position>maps.map[map_id]->height_map_size){
        perror ("tile outside bounds in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    if(map_id<0 || map_id>maps.count-1){
        perror ("map outside bounds in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    int map_axis=maps.map[map_id]->map_axis;

    int destination=position+vector_x[vector]+(vector_y[vector]*map_axis);

    int x=position % map_axis;
    int y=position / map_axis;

    //check vector is within map bounds
    if(y==0 && vector_y[vector]<0) return TILE_OUTSIDE_BOUNDS;
    if(y==map_axis-1 && vector_y[vector]>0) return TILE_OUTSIDE_BOUNDS;

    if(x==0 && vector_x[vector]<0) return TILE_OUTSIDE_BOUNDS;
    if(x==map_axis-1 && vector_x[vector]>0) return TILE_OUTSIDE_BOUNDS;

    //check tile is traversable
    if(maps.map[map_id]->height_map[destination]<MIN_TRAVERSABLE_VALUE) return TILE_NON_TRAVERSABLE;

    return TILE_TRAVERSABLE;
}

int get_indirect_path(int position, int destination, int map_id, int *indirect_path_count, int indirect_path[]){

    //check input bounds
    if(position<0 || position > maps.map[map_id]->height_map_size){
        perror ("position tile outside bounds in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    if(destination<0 || destination > maps.map[map_id]->height_map_size){
        perror ("destination tile outside bounds in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    if(map_id<0 || map_id > maps.count-1){
        perror ("map outside bounds in function check_tile_bounds");
        exit (EXIT_FAILURE);
    }

    //illegal destination should be caught in function process_packet but lets double check just in case
    if(maps.map[map_id]->height_map[destination]<MIN_TRAVERSABLE_VALUE) {
        perror("illegal destination in function get_direct_path");
        exit(EXIT_FAILURE);
    }

    //reset path buffer
    *indirect_path_count=0;

    enum {
        DESTINATION_REACHED=1,
        DESTINATION_NOT_REACHED=0
    };

    int i=0, j=0;
    int queue[PATH_QUEUE_MAX]={0};
    int wave[PATH_QUEUE_MAX]={0};
    int map_axis=maps.map[map_id]->map_axis;
    int check_tile=0;
    int bounds=0;
    int queue_count=1;
    int parse_start=0;
    int parse_finish=0;
    int wave_count=0;
    int queue_node=0;
    int found=0;
    int highest_wave=0;
    int lowest_wave=0;
    int lowest_node=0;
/*
    unsigned char map[50000];
    for(i=0; i<50000; i++){
        map[i]=maps.map[map_id]->height_map[i];
    }
*/
    queue_count=1;
    queue[queue_count-1]=position;
    parse_start=0;
    parse_finish=1;

    //calculate indirect path
    do {

        wave_count++;

        for(i=parse_start; i<parse_finish; i++){

           for(j=0; j<8; j++){

                bounds=check_tile_bounds(queue[i], j, map_id);

                check_tile=queue[i]+vector_x[j]+(vector_y[j]*map_axis);

                if(check_tile!=destination){

                    if(bounds==TILE_TRAVERSABLE){

                        if(find_existing_tile(check_tile, queue_count, queue)==-1){
                            //map[check_tile]=wave_count;
                            wave[queue_count]=wave_count;
                            queue[queue_count]=check_tile;
                            queue_count++;

                            /*test for circumstances where the destination is reachable but is cut-off
                            by non-traversable tiles */
                            if(queue_count==maps.map[map_id]->tile_map_size) return PATH_UNREACHABLE;

                            // test for circumstances where the resulting path exceeds the buffer size
                            if(queue_count>PATH_QUEUE_MAX) return PATH_TOO_LONG;
                        }
                    }
                }
                else {
                    found=1;
                    break;
                }

            }

        }

        if(found==0){
            parse_start=parse_finish-1;
            parse_finish=queue_count;

            if(parse_start==parse_finish-1) exit(1);
        }

    }while(found==0);

    highest_wave=wave_count;
    *indirect_path_count=wave_count;
    indirect_path[wave_count-1]=destination;

    do{

        lowest_wave=highest_wave;

        for(j=0; j<8; j++){

            if(check_tile_bounds(indirect_path[wave_count-1], j, map_id)==TILE_TRAVERSABLE){

                check_tile=indirect_path[wave_count-1]+vector_x[j]+(vector_y[j]*map_axis);

                queue_node=find_existing_tile(check_tile, queue_count, queue);

                if(wave[queue_node]<lowest_wave && queue_node>0){
                    lowest_wave=wave[queue_node];
                    lowest_node=queue_node;
                }
            }
        }

        wave_count--;
        indirect_path[wave_count-1]=queue[lowest_node];
        //map[indirect_path[wave_count-1]]=wave_count;

    }while(wave_count>0);

    //show_map(destination, map);

    return PATH_OPEN;
}

int get_direct_path(int position, int destination, int map_id, int *direct_path_count, int direct_path[]){

    //check our input bounds
    if(position<0 || position>maps.map[map_id]->height_map_size){
        perror ("position tile outside bounds in function get_direct_path");
        exit (EXIT_FAILURE);
    }

    if(destination<0 || destination>maps.map[map_id]->height_map_size){
        perror ("destination tile outside bounds in function get_direct_path");
        exit (EXIT_FAILURE);
    }

    if(map_id<0 || map_id>maps.count-1){
        perror ("map outside bounds in function get_direct_path");
        exit (EXIT_FAILURE);
    }

    //illegal destination should be caught in function process_packet but lets double check just in case
    if(maps.map[map_id]->height_map[destination]<MIN_TRAVERSABLE_VALUE) {
        perror("illegal destination in function get_direct_path");
        exit(EXIT_FAILURE);
    }

    //reset path buffer
    *direct_path_count=0;

    int heading=0;
    int move=0;
    int map_axis=maps.map[map_id]->map_axis;

    do{
        //find the optimum direction in which to move
        heading=get_heading(position, destination, map_axis);

        //calculate the next tile position
        move=vector_x[heading] + (vector_y[heading] * map_axis);

         //update path position
        position+=move;

        //detect collision
        if(maps.map[map_id]->height_map[position]==0) return PATH_BLOCKED;

        //load path to the path_buffer
        direct_path[*direct_path_count]=position;

        //increase the number of path steps
        *direct_path_count=*direct_path_count+1;

        //test for array bounds being exceeded
        if(*direct_path_count>PATH_MAX) return PATH_TOO_LONG;

    } while(position!=destination);

    return PATH_OPEN;
}

void process_char_move(int connection, time_t current_utime){

    int i=0;
    int direct_path[PATH_MAX];
    int direct_path_count=0;
    int direct_path_result=0;

    int indirect_path[PATH_MAX];
    int indirect_path_count=0;
    int indirect_path_result=0;

    int sock=connection;
    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int current_tile=characters.character[clients.client[connection]->character_id]->map_tile;
    int destination_tile=0;
    //time_t current_utime;
    int move_cmd=0;
    char text_out[1024];

    //if we've reached our last destination check the move_to buffer to see if there are any more
    if(clients.client[connection]->move_buffer_size>0 && clients.client[connection]->path_max==0){

        //get the next destination from the MOVE_TO buffer
        dequeue_move_to(connection, &destination_tile);

        if(current_tile!=destination_tile){

            printf("try direct path\n");

            direct_path_result=get_direct_path(current_tile, destination_tile, map_id, &direct_path_count, direct_path);

            //printf("direct path result %i\n", direct_path_result);

            if(direct_path_result==PATH_OPEN){

                //set start and end position for path
                clients.client[connection]->path_count=0;
                clients.client[connection]->path_max=direct_path_count+1;

                //reset time of last move to zero so the movement is processed without delay
                clients.client[connection]->time_of_last_move=0;

                //transfer direct path to path buffer
                for(i=0; i<=direct_path_count; i++){
                    clients.client[connection]->path[i]=direct_path[i];
                }

            }
            else if(direct_path_result==PATH_BLOCKED) {

                printf("try indirect path\n");

                indirect_path_result=get_indirect_path(current_tile, destination_tile, map_id, &indirect_path_count, indirect_path);

                //printf("indirect path result %i\n", indirect_path_result);

                if(indirect_path_result==PATH_OPEN){

                    //set start and end position for path
                    clients.client[connection]->path_count=0;
                    clients.client[connection]->path_max=indirect_path_count+1;

                    //reset time of last move to zero so the movement is processed without delay
                    clients.client[connection]->time_of_last_move=0;

                    //transfer indirect path to path buffer
                    for(i=0; i<=indirect_path_count-1; i++){
                        clients.client[connection]->path[i]=indirect_path[i];
                    }

                }
                else if(indirect_path_result==PATH_UNREACHABLE){

                    printf("destination is unreachable by indirect path\n");

                    sprintf(text_out, "%cThe tile you clicked on is unreachable from this position.", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                }
                else if(indirect_path_result==PATH_TOO_LONG) {

                    printf("indirect path exceeds path buffer\n");

                    sprintf(text_out, "%cThe tile you clicked on caused an error. Please notify the game administrator", c_red3+127);
                    send_server_text(sock, CHAT_SERVER, text_out);
                }
                else {
                    perror("unknown indirect path result in function process_move");
                    exit(EXIT_FAILURE);
                }
            }
            else if(direct_path_result==PATH_TOO_LONG){

                printf("direct path exceeds path buffer\n");

                sprintf(text_out, "%cThe tile you clicked on caused an error. Please notify the game administrator", c_red3+127);
                send_server_text(sock, CHAT_SERVER, text_out);
            }
            else {
                perror("unknown direct path result in function process_move");
                exit(EXIT_FAILURE);
            }

        }
    }

    // move actor one step along the path
    if(clients.client[connection]->path_max>clients.client[connection]->path_count){

        //adjust timer to compensate for wrap-around
        if(clients.client[connection]->time_of_last_move>current_utime) current_utime+=1000000;

        // check for time of next movement
        if(current_utime>clients.client[connection]->time_of_last_move+290000) {

            //get destination tile from the path queue
            dequeue_path_step(connection, &destination_tile);

            printf("step[%i] current tile[%i] destination tile[%i] move[%i] ",  clients.client[connection]->path_count, current_tile, destination_tile, destination_tile-current_tile);

            // filter out moves where position and destination are the same
            if(current_tile!=destination_tile) {

                printf(" - executed\n");

                //update the time of movement
                gettimeofday(&time_check, NULL);
                clients.client[connection]->time_of_last_move=time_check.tv_usec;

                //calculate the move_cmd to send to clients
                move_cmd=get_move_command(current_tile, destination_tile, map_axis);

                //broadcast move to clients
                broadcast_actor_packet(connection, move_cmd, destination_tile);
                //receive_add_actor_packet(connection, move_cmd, destination_tile);

                //update char current position
                characters.character[clients.client[connection]->character_id]->map_tile=destination_tile;

                save_character(characters.character[char_id]->char_name, char_id);
            }
            else {
                printf(" - ignored\n");
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
