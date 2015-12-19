#include <stdio.h> //support snprintf
#include <unistd.h>     //supports close function

#include "clients.h"
#include "idle_buffer2.h"
#include "db/database_functions.h"
#include "broadcast_actor_functions.h"

struct client_list_type clients;
struct client_socket_type client_socket[MAX_SOCKETS];


int get_next_free_actor_node(){


    for(int i=0; i<MAX_ACTORS; i++){

        if(clients.client[i].node_status==CLIENT_NODE_UNUSED) return i;
    }

    return -1;
}




void close_connection_slot(int actor_node){

    /** public function - see header */

    if(client_socket[actor_node].socket_status==CLIENT_LOGGED_IN){

        //broadcast to local
        broadcast_remove_actor_packet(actor_node);
        // TODO (themuntdregger#1#): add broadcast to nearby chars that client has been eaten by grue

        //update last in game time for char
        clients.client[actor_node].time_of_last_minute=time(NULL);
        push_sql_command("UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[actor_node].time_of_last_minute, clients.client[actor_node].character_id);
    }

    close(clients.client[actor_node].socket);
}
