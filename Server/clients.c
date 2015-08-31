#include <stdio.h> //support snprintf
#include <unistd.h>     //supports close function

#include "clients.h"
#include "idle_buffer2.h"
#include "db/database_functions.h"
#include "broadcast_actor_functions.h"

struct client_list_type clients;

void close_connection_slot(int connection){

    if(clients.client[connection].client_status==LOGGED_IN){

        //broadcast to local
        broadcast_remove_actor_packet(connection);

        //update last in game time for char
        clients.client[connection].time_of_last_minute=time(NULL);

        char sql[MAX_SQL_LEN]="";
        snprintf(sql, MAX_SQL_LEN, "UPDATE CHARACTER_TABLE SET LAST_IN_GAME=%i WHERE CHAR_ID=%i;",(int)clients.client[connection].time_of_last_minute, clients.client[connection].character_id);
        push_sql_command(sql);
    }

    close(connection);
}
