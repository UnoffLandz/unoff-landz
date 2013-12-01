#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "global.h"
#include "character_movement.h"

int get_proximity(int tile1, int tile2, int map_axis){

    int tile1_x=tile1 % map_axis;
    int tile1_y=tile1 / map_axis;
    int tile2_x=tile2 % map_axis;
    int tile2_y=tile2 / map_axis;

    int x_diff=abs(tile1_x - tile2_x);
    int y_diff=abs(tile1_y - tile2_y);

    if(x_diff>y_diff){
        return x_diff;
    }
    else{
        return y_diff;
    }

    return 0;
}

void raw_text_packet(int channel, char *text, unsigned char *packet, int *packet_length){

    int i=0,j=0;
    int data_length=0;

    packet[i++]=0;             // protocol
    packet[i++]=0;             // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;             // dummy the msb (we'll put the proper value in later
    packet[i++]=channel;       // channel

    for(j=0; j<(int)strlen(text); j++){
        packet[i++]=text[j];
    }

    *packet_length=i;

    // now we know the packet length we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
}

void send_raw_text_packet(int sock, int chan_type, char *text){

    // used to send personal messages

    unsigned char packet[1024];
    int packet_length=0;

    raw_text_packet(chan_type, text, packet, &packet_length);

    send(sock, packet, packet_length, 0);
}

void broadcast_local_chat(int connection, int map_id, char *text){

    int i=0, j=0, k=0;
    unsigned char packet[1024];
    int packet_length=0;

    int char_id=clients.client[connection]->character_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;
    int local_text_proximity=0;
    int receiving_char_tile=0;

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];
        k=clients.client[j]->character_id;

        local_text_proximity=characters.character[k]->local_text_proximity;
        receiving_char_tile=characters.character[k]->map_tile;

        //broadcast only to chars in local text proximity
        if(get_proximity(char_tile, receiving_char_tile, map_axis)<local_text_proximity){
            raw_text_packet(CHAT_LOCAL, text, packet, &packet_length);
            memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
            clients.client[j]->cmd_buffer_end++;
        }
    }

}

void broadcast_channel_chat(int chan, char *text){

    int i=0, j=0;
    unsigned char packet[1024];
    int packet_length=0;

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        j=channels.channel[chan]->client_list[i];

        raw_text_packet(CHAT_SERVER, text, packet, &packet_length);
        memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
        clients.client[j]->cmd_buffer_end++;
    }

}

void broadcast_guild_channel_chat(int guild_id, char *text){

    int i=0, j=0;
    unsigned char packet[1024];
    int packet_length=0;
    int chan=guilds.guild[guild_id]->guild_chan_number;

    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        j=channels.channel[chan]->client_list[i];

        raw_text_packet(CHAT_GM, text, packet, &packet_length);
        memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
        clients.client[j]->cmd_buffer_end++;
    }

}


/*
void broadcast_raw_text_packet(int connection, int channel, int chan_type, char *text){

    int i=0,j=0;
    unsigned char packet[1024];
    int packet_length=0;

    int char_id=clients.client[connection]->character_id;
    int char_tile=characters.character[char_id]->map_tile;

    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;

    int local_text_proximity=0;
    int receiving_char_tile=0;

    for(i=0; i<clients.max; i++){

        // restrict to clients that are logged in
        if(clients.client[i]->status==LOGGED_IN) {

            switch(chan_type){

                case CHAT_SERVER:

                    for(j=0; j<3; j++){

                        if(characters.character[clients.client[i]->character_id]->chan[j]==channel) {

                            raw_text_packet(CHAT_SERVER, text, packet, &packet_length);
                            memcpy(clients.client[i]->cmd_buffer[clients.client[i]->cmd_buffer_end], packet, packet_length);
                            clients.client[i]->cmd_buffer_end++;
                        }
                    }

                break;

                case CHAT_GM:
                    // do we need this now that guild and chat chans are the same ?? - yes, gm chat is more efficient
                    //   than SERVER_CHAT because the latter needs to test multiple chans for each client whereas the
                    //   former only needs to test 1 chan

                    if(characters.character[clients.client[i]->character_id]->chan[0]==channel) {

                        raw_text_packet(CHAT_GM, text, packet, &packet_length);
                        memcpy(clients.client[i]->cmd_buffer[clients.client[i]->cmd_buffer_end], packet, packet_length);
                        clients.client[i]->cmd_buffer_end++;
                    }
                break;

                case CHAT_LOCAL:

                    //broadcast only to chars on local map
                    if(characters.character[clients.client[i]->character_id]->map_id==map_id){

                        local_text_proximity=characters.character[char_id]->local_text_proximity;
                        receiving_char_tile=characters.character[i]->map_tile;

                        //broadcast only to chars in local text proximity
                        if(get_proximity(char_tile, receiving_char_tile, map_axis)<local_text_proximity){

                            raw_text_packet(channel, text, packet, &packet_length);
                            memcpy(clients.client[i]->cmd_buffer[clients.client[i]->cmd_buffer_end], packet, packet_length);
                            clients.client[i]->cmd_buffer_end++;
                        }
                    }

                break;

                default:
                break;
            }

        }

    }

}
*/

void add_new_enhanced_actor_packet(int char_id, unsigned char *packet, int *packet_length){

    int i=0,j=0;
    int data_length=0;
    int guild_id=characters.character[char_id]->guild_id;
    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int x=characters.character[char_id]->map_tile % map_axis;
    int y=characters.character[char_id]->map_tile / map_axis;

    packet[i++]=51;                                                   // protocol
    packet[i++]=0;                                                    // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;                                                    // dummy the msb (we'll put the proper value in later
    packet[i++]=char_id % 256;                                        // char_id lsb
    packet[i++]=char_id / 256;                                        // char_id msb
    packet[i++]=x % 256;                                              // x axis lsb
    packet[i++]=x / 256;                                              // x axis msb
    packet[i++]=y % 256;                                              // y axis lsb
    packet[i++]=y / 256;                                              // y axis msb
    packet[i++]=0;                                                    // z axis lsb
    packet[i++]=0;                                                    // z axis msb
    packet[i++]=45;                                                   // rotation lsb
	packet[i++]=0;                                                    // rotation msb
	packet[i++]=characters.character[char_id]->char_type;            // char type
    packet[i++]=0;                                                    // unknown
	packet[i++]=characters.character[char_id]->skin_type;            // 1 skin type
	packet[i++]=characters.character[char_id]->hair_type;            // 1 hair type
	packet[i++]=characters.character[char_id]->shirt_type;           // 1 shirt type
	packet[i++]=characters.character[char_id]->pants_type;           // 1 pants type
	packet[i++]=characters.character[char_id]->boots_type;           // 1 boots type
	packet[i++]=characters.character[char_id]->head_type;            // 1 head type
	packet[i++]=characters.character[char_id]->shield_type;          // 11 shield type
	packet[i++]=characters.character[char_id]->weapon_type;          // 0 weapon type
	packet[i++]=characters.character[char_id]->cape_type;            // 30 cape type
	packet[i++]=characters.character[char_id]->helmet_type;          // 0 helmet type
	packet[i++]=characters.character[char_id]->neck_type;            // 7 neck type (THIS COULD ALSO BE FRAME TYPE
	packet[i++]=characters.character[char_id]->max_health % 256;     // max health lsb
	packet[i++]=characters.character[char_id]->max_health / 256;     // max health msb
	packet[i++]=characters.character[char_id]->current_health % 256; // current health lsb
	packet[i++]=characters.character[char_id]->current_health / 256; // current health msb
	packet[i++]=1;                                                    // special char type HUMAN / NPC

    // add char name to packet
	for(j=0;j< (int)strlen(characters.character[char_id]->char_name); j++){
            packet[i++]=characters.character[char_id]->char_name[j];
	}

    // add guild name
	if(guild_id>0) {
        packet[i++]=32;
        packet[i++]=guilds.guild[guild_id]->tag_colour;

        for(j=0; j< (int)strlen(guilds.guild[guild_id]->guild_tag); j++){
	        packet[i++]=guilds.guild[guild_id]->guild_tag[j];
        }
	}

    packet[i++]='\0';

	packet[i++]=0;
	packet[i++]=64;
	packet[i++]=255;
	packet[i++]=64;

    *packet_length=i;

    // now we know the packet length we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
}

void broadcast_add_new_enhanced_actor_packet(int connection){

    //broadcasts a single char to all connected clients

    int i=0, j=0;
    unsigned char packet[1024];
    int packet_length=0;

    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    int receiving_char_id=0;
    int receiving_char_tile=0;
    int receiving_char_visual_proximity=0;

    // create the packet to be broadcast
    add_new_enhanced_actor_packet(char_id, packet, &packet_length);

    printf("Broadcasting add_new_enhanced_actor for %s to...\n", characters.character[char_id]->char_name);

    for(i=0; i<maps.map[i]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];

        receiving_char_id=clients.client[j]->character_id;
        receiving_char_tile=characters.character[receiving_char_id]->map_tile;
        receiving_char_visual_proximity=characters.character[receiving_char_id]->visual_proximity;

        //printf("char tile %i receiving char tile %i map axis %i\n", char_tile, receiving_char_tile, map_axis);
        //printf("name=%s proximity=%i char view within=%i\n", characters.character[receiving_char_id]->char_name, get_proximity(char_tile, receiving_char_tile, map_axis), receiving_char_visual_proximity);

        //restrict to characters within visual proximity
        if(get_proximity(char_tile, receiving_char_tile, map_axis)<receiving_char_visual_proximity){
            memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
            clients.client[j]->cmd_buffer_end++;
        }
    }
}

void remove_actor_packet(int char_id, unsigned char *packet, int *packet_length){

    int i;
    int data_length=0;

    i=0;                       /* zero the packet length counter */
    packet[i++]=6;             /* protocol       */
    packet[i++]=0;             /* dummy the lsb (we'll put the proper value in later  */
    packet[i++]=0;             /* dummy the msb (we'll put the proper value in later  */
    packet[i++]=char_id % 256;      /* char_id lsb    */
    packet[i++]=char_id / 256;      /* char_id msb    */

    *packet_length=i;

    /* now we know the packet length we can calculate the data length by subtracting 2 */
    data_length=i-2;

    /* now we know our data length we can will in the proper values for our lsb/msb */
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;

}

void broadcast_remove_actor_packet(int connection) {

    int i=0, j=0;
    unsigned char packet[1024];
    int packet_length=0;

    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    int receiving_char_id=0;
    int receiving_char_tile=0;
    int receiving_char_visual_proximity=0;

    remove_actor_packet(char_id, packet, &packet_length);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];

        receiving_char_id=clients.client[j]->character_id;
        receiving_char_tile=characters.character[receiving_char_id]->map_tile;
        receiving_char_visual_proximity=characters.character[receiving_char_id]->visual_proximity;

        //filter for receiving char visual proximity
        if(get_proximity(char_tile, receiving_char_tile, map_axis)>receiving_char_visual_proximity){
            memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
            clients.client[j]->cmd_buffer_end++;
        }
    }
}

void add_actor_packet(int char_id, unsigned char move, unsigned char *packet, int *packet_length){

    int i=0;
    int data_length=0;

    packet[i++]=2;               // protocol
    packet[i++]=0;               // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;               // dummy the msb (we'll put the proper value in later
    packet[i++]= char_id % 256;  // char_id lsb
    packet[i++]= char_id / 256;  // char_id msb
    packet[i++]=move;            // actor command

    *packet_length=i;

    /* now we know the packet length we can calculate the data length by subtracting 2 */
    data_length=i-2;

    /* now we know our data length we can will in the proper values for our lsb/msb */
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
}

void broadcast_add_actor_packet(int connection, unsigned char move, int destination_tile){

    //broadcasts a single char to all connected clients

    int i=0, j=0;

    unsigned char packet1[1024];// receiving char add_actor packet
    int packet1_length=0;

    unsigned char packet2[1024];// receiving char add_enhanced_actor packet
    int packet2_length=0;

    unsigned char packet3[1024];// receiving char remove_actor packet
    int packet3_length=0;

    unsigned char packet4[1024];
    int packet4_length=0;

    unsigned char packet5[1024];
    int packet5_length=0;

    unsigned char packet6[1024];
    int packet6_length=0;

    int char_id=clients.client[connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;
    int char_visual_proximity=characters.character[char_id]->visual_proximity;

    int proximity_before_move=0;
    int proximity_after_move=0;

    int receiving_char_id=0;
    int receiving_char_tile=0;
    int receiving_char_visual_proximity=0;

    // pre-create packets that will be sent more than once in order to save time
    add_actor_packet(char_id, move, packet1, &packet1_length);
    add_new_enhanced_actor_packet(char_id, packet2, &packet2_length);
    remove_actor_packet(char_id, packet3, &packet3_length);

    //printf("Broadcasting for %s to...\n", characters.character[char_id]->char_name);

    for(i=0; i<maps.map[i]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];

        receiving_char_id=clients.client[j]->character_id;
        receiving_char_tile=characters.character[receiving_char_id]->map_tile;
        receiving_char_visual_proximity=characters.character[receiving_char_id]->visual_proximity;

        //printf("name=%s proximity=%i char view within=%i\n", characters.character[receiving_char_id]->char_name, get_proximity(char_tile, receiving_char_tile, map_axis), receiving_char_visual_proximity);

        proximity_before_move=get_proximity(char_tile, receiving_char_tile, map_axis);
        proximity_after_move=get_proximity(destination_tile, receiving_char_tile, map_axis);
        //printf("proximity before move %i proximity_after_move %i\n", proximity_before_move, proximity_after_move);

        //This block deals with receiving char vision
        if(j!=connection){

            if(proximity_before_move>receiving_char_visual_proximity && proximity_after_move<=receiving_char_visual_proximity){

                //sending char moves into visual proximity of receiving char
                //printf("sending char moves into range of receiving char\n");

                memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet2, packet2_length);
                clients.client[j]->cmd_buffer_end++;
            }
            else if(proximity_before_move<=receiving_char_visual_proximity && proximity_after_move>receiving_char_visual_proximity){

                //sending char moves out of visual proximity of receiving char
                //printf("sending char moves out of range of receiving char\n");

                memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet3, packet3_length);
                clients.client[j]->cmd_buffer_end++;
            }
            else if(proximity_before_move<=receiving_char_visual_proximity && proximity_after_move<=receiving_char_visual_proximity){

                //sending char moving within visual proximity of receiving char
                //printf("sending char moves within range of receiving char\n");

                memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet1, packet1_length);
                clients.client[j]->cmd_buffer_end++;
            }

        }

        if(proximity_before_move>char_visual_proximity && proximity_after_move<=char_visual_proximity){

            //sending char moves into visual proximity of receiving char
            //printf("receiving char moves into range of sending char\n");

            add_new_enhanced_actor_packet(clients.client[j]->character_id, packet4, &packet4_length);
            send(clients.client[connection]->sock, packet4, packet4_length, 0);
        }
        else if(proximity_before_move<=char_visual_proximity && proximity_after_move>char_visual_proximity){

            //sending char moves out of visual proximity of receiving char
            //printf("receiving char moves out of range of sending char\n");

            remove_actor_packet(clients.client[j]->character_id, packet5, &packet5_length);
            send(clients.client[connection]->sock, packet5, packet5_length, 0);
        }
        else if(proximity_before_move<=char_visual_proximity && proximity_after_move<=char_visual_proximity){

            //sending char moves within visual proximity of receiving char
            //printf("receiving char moves within range of sending char\n");

            if(j==connection) {
                // only move our actor
                add_actor_packet(clients.client[j]->character_id, move, packet6, &packet6_length);
            }
            else{
                // other actors remain stationary
                add_actor_packet(clients.client[j]->character_id, 0, packet6, &packet6_length);
            }

            send(clients.client[connection]->sock, packet6, packet6_length, 0);
        }

    }

}
