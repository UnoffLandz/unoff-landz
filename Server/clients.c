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

    //notify guild that char has logged off
    int guild_id=clients.client[actor_node].guild_id;

    if(guild_id>0){

        // TODO (themuntdregger#1#): create broadcast_guild_event function to hold this code and to allow ...
        //guild master to modify leaving/joining messages and colours
        char text_out[80]="";
        sprintf(text_out, "%c%s LEFT THE GAME", c_blue3+127, clients.client[actor_node].char_name);
        broadcast_guild_chat(guild_id, actor_node, text_out);
    }

    //close client socket connection
    int socket=clients.client[actor_node].socket;

    if(client_socket[socket].socket_node_status==CLIENT_LOGGED_IN){

        //broadcast actor removal
        broadcast_remove_actor_packet(actor_node);

        //update last in game time for char
        clients.client[actor_node].time_of_last_minute=time(NULL);
        push_sql_command("UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[actor_node].time_of_last_minute, clients.client[actor_node].character_id);

        log_event(EVENT_SESSION, "socket [%i] closed on char [%s] whilst LOGGED_IN", socket, clients.client[actor_node].char_name);
    }
    else if(client_socket[socket].socket_node_status==CLIENT_CONNECTED){

        log_event(EVENT_SESSION, "socket [%i] closed whilst CONNECTED", socket);

    }
    else {

        log_event(EVENT_ERROR, "attempt to close unused socket [%i] in function %s: module %s: line %i", socket, GET_CALL_INFO);
        stop_server();
    }

    //hasta la vista baby
    close(socket);
}
