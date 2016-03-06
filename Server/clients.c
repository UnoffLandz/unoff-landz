#include <stdio.h>      //support snprintf
#include <unistd.h>     //supports close function

#include "clients.h"
#include "idle_buffer2.h"
#include "db/database_functions.h"
#include "logging.h"
#include "server_start_stop.h"
#include "colour.h"
#include "movement.h"

struct client_list_type clients;
struct client_socket_type client_socket[MAX_SOCKETS];

int get_next_free_actor_node(){

    /** public function - see header */

    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].client_node_status==CLIENT_NODE_UNUSED) return i;
    }

    return -1;
}


void close_connection_slot(int actor_node){

    /** public function - see header */

    //update last in game time for char
    clients.client[actor_node].time_of_last_minute=time(NULL);
    push_sql_command("UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[actor_node].time_of_last_minute, clients.client[actor_node].character_id);

    //set the actor node to UNUSED otherwise we could end up broadcasting to a closed socket
    clients.client[actor_node].client_node_status=CLIENT_NODE_UNUSED;

    //notify guild that char has logged off
    int guild_id=clients.client[actor_node].guild_id;

    if(guild_id>0){

        char text_out[80]="";

        sprintf(text_out, "%c%s LEFT THE GAME", c_blue3+127, clients.client[actor_node].char_name);
        broadcast_guild_chat(guild_id, actor_node, text_out);
    }

    int socket=clients.client[actor_node].socket;

    //only remove the actor from the game if the char is actually logged in
    if(client_socket[socket].socket_node_status==CLIENT_LOGGED_IN){

        broadcast_remove_actor_packet(actor_node);
    }

    close(socket);

    log_event(EVENT_SESSION, "socket [%i] closed for char [%s]", socket, clients.client[actor_node].char_name);
}
