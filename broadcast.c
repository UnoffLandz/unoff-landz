#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "global.h"
#include "character_movement.h"
#include "protocol.h"

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

void raw_text_packet(int chan_type, char *text, unsigned char *packet, int *packet_length){

    int i=0,j=0;
    int data_length=0;

    packet[i++]=0;             // protocol
    packet[i++]=0;             // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;             // dummy the msb (we'll put the proper value in later
    packet[i++]=chan_type;     // channel

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

    //sends text packet to a single connection

    unsigned char packet[1024];
    int packet_length=0;

    raw_text_packet(chan_type, text, packet, &packet_length);
    send(sock, packet, packet_length, 0);
}

void broadcast_local_chat(int sender_connection, char *text_in){

    int i=0;

    int char_id=clients.client[sender_connection]->character_id;
    int sender_char_tile=characters.character[char_id]->map_tile;
    int map_id=characters.character[char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;

    int receiver_local_text_proximity=0;
    int receiver_char_tile=0;
    int receiver_connection=0;
    int receiver_char_id=0;

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        if(receiver_connection!=sender_connection){

            receiver_char_id=clients.client[receiver_connection]->character_id;

            receiver_local_text_proximity=characters.character[receiver_char_id]->local_text_proximity;
            receiver_char_tile=characters.character[receiver_char_id]->map_tile;

            //broadcast only to chars in local text proximity
            if(get_proximity(sender_char_tile, receiver_char_tile, map_axis)<receiver_local_text_proximity){

                //raw_text_packet(CHAT_LOCAL, text_in, packet, &packet_length);
                //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet, packet_length);
                //clients.client[receiver_connection]->cmd_buffer_end++;
                send_raw_text_packet(clients.client[receiver_connection]->sock, CHAT_LOCAL, text_in);
            }
        }
    }
}

void broadcast_channel_chat(int chan, int sender_connection, char *text_in){

    int i=0;
    int receiver_connection=0;
    int receiver_char_id=0;
    int receiver_active_chan=0;
    char text_out[1024]="";

    //send to channel
    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        receiver_connection=channels.channel[chan]->client_list[i];

        if(receiver_connection!=sender_connection){

            receiver_char_id=clients.client[receiver_connection]->character_id;
            receiver_active_chan=characters.character[receiver_char_id]->active_chan;

            //show non-active chan in darker grey
            if(characters.character[receiver_char_id]->chan[receiver_active_chan]==chan){
                sprintf(text_out, "%c%s", c_grey1+127, text_in);
            }
            else {
                sprintf(text_out, "%c%s", c_grey2+127, text_in);
            }

            //raw_text_packet(CHAT_SERVER, text_in, packet, &packet_length);
            //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet, packet_length);
            //clients.client[receiver_connection]->cmd_buffer_end++;
            send_raw_text_packet(clients.client[receiver_connection]->sock, CHAT_SERVER, text_out);
        }
    }
}

void broadcast_channel_event(int chan, int sender_connection, char *text_in){

    int i=0;
    int receiver_connection=0;

    //send to channel
    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        receiver_connection=channels.channel[chan]->client_list[i];

        if(receiver_connection!=sender_connection){

            //raw_text_packet(CHAT_SERVER, text_in, packet, &packet_length);
            //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet, packet_length);
            //clients.client[receiver_connection]->cmd_buffer_end++;
            send_raw_text_packet(clients.client[receiver_connection]->sock, CHAT_SERVER, text_in);
        }
    }
}

void broadcast_guild_channel_chat(int sender_connection, char *text_in){

    int i=0;
    int char_id=clients.client[sender_connection]->character_id;
    int guild_id=characters.character[char_id]->guild_id;
    int chan=guilds.guild[guild_id]->guild_chan_number;
    int receiver_connection=0;

    //send to channel
    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        receiver_connection=channels.channel[chan]->client_list[i];

        if(receiver_connection!=sender_connection){

            //raw_text_packet(CHAT_GM, text_in, packet, &packet_length);
            //memcpy(clients.client[j]->cmd_buffer[clients.client[j]->cmd_buffer_end], packet, packet_length);
            //clients.client[j]->cmd_buffer_end++;
            send_raw_text_packet(clients.client[receiver_connection]->sock, CHAT_GM, text_in);
        }
    }
}

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

void broadcast_add_new_enhanced_actor_packet(int sender_connection){

    //broadcasts a single char to all connected clients

    int i=0;
    //int j=0;
    unsigned char packet[1024];
    int packet_length=0;

    int receiving_char_id=0;
    int receiving_char_tile=0;
    int receiving_char_visual_proximity=0;
    int receiver_connection=0;

    int char_id=clients.client[sender_connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    // create the packet to be broadcast
    add_new_enhanced_actor_packet(char_id, packet, &packet_length);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        receiving_char_id=clients.client[receiver_connection]->character_id;
        receiving_char_tile=characters.character[receiving_char_id]->map_tile;
        receiving_char_visual_proximity=characters.character[receiving_char_id]->visual_proximity;

        //restrict to characters within visual proximity
        if(get_proximity(char_tile, receiving_char_tile, map_axis)<receiving_char_visual_proximity){

            //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet, packet_length);
            //clients.client[receiver_connection]->cmd_buffer_end++;
            send(clients.client[receiver_connection]->sock, packet, packet_length, 0);
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

void broadcast_remove_actor_packet(int sender_connection) {

    int i=0;
    unsigned char packet[1024];
    int packet_length=0;

    int char_id=clients.client[sender_connection]->character_id;
    int map_id=characters.character[char_id]->map_id;
    int char_tile=characters.character[char_id]->map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    int receiver_connection=0;
    int receiver_char_id=0;
    int receiver_char_tile=0;
    int receiver_char_visual_proximity=0;

    remove_actor_packet(char_id, packet, &packet_length);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        receiver_char_id=clients.client[receiver_connection]->character_id;
        receiver_char_tile=characters.character[receiver_char_id]->map_tile;
        receiver_char_visual_proximity=characters.character[receiver_char_id]->visual_proximity;

        //filter for receiving char visual proximity
        if(get_proximity(char_tile, receiver_char_tile, map_axis)<=receiver_char_visual_proximity){

            //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet, packet_length);
            //clients.client[receiver_connection]->cmd_buffer_end++;
            send(clients.client[receiver_connection]->sock, packet, packet_length, 0);
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

void broadcast_actor_packet(int sender_connection, unsigned char move, int sender_destination_tile){

    //broadcasts a specific char's movement to all connected clients

    int i=0;

    unsigned char packet[1024];// sending char packets
    int packet_length=0;

    unsigned char packet1[1024];// receiving char add_actor packet
    int packet1_length=0;

    unsigned char packet2[1024];// receiving char add_enhanced_actor packet
    int packet2_length=0;

    unsigned char packet3[1024];// receiving char remove_actor packet
    int packet3_length=0;

    int sender_char_id=clients.client[sender_connection]->character_id;
    int sender_current_tile=characters.character[sender_char_id]->map_tile;
    int map_id=characters.character[sender_char_id]->map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int sender_visual_proximity=characters.character[sender_char_id]->visual_proximity;

    int proximity_before_move=0;
    int proximity_after_move=0;

    int receiver_connection=0;
    int receiver_char_id=0;
    int receiver_char_tile=0;
    int receiver_char_visual_proximity=0;

    // pre-create packets that will be sent more than once in order to save time
    add_actor_packet(sender_char_id, move, packet1, &packet1_length);
    add_new_enhanced_actor_packet(sender_char_id, packet2, &packet2_length);
    remove_actor_packet(sender_char_id, packet3, &packet3_length);

    // broadcast sender char move to all receiver clients
    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        receiver_char_id=clients.client[receiver_connection]->character_id;
        receiver_char_tile=characters.character[receiver_char_id]->map_tile;
        receiver_char_visual_proximity=characters.character[receiver_char_id]->visual_proximity;

        proximity_before_move=get_proximity(sender_current_tile, receiver_char_tile, map_axis);
        proximity_after_move=get_proximity(sender_destination_tile, receiver_char_tile, map_axis);

        //This block deals with receiving char vision
        if(receiver_connection!=sender_connection){

            if(proximity_before_move>receiver_char_visual_proximity && proximity_after_move<=receiver_char_visual_proximity){

                //sending char moves into visual proximity of receiving char

                //memcpy(clients.client[receiver_connection]->cmd_buffer[clients.client[receiver_connection]->cmd_buffer_end], packet2, packet2_length);
                //clients.client[receiver_connection]->cmd_buffer_end++;
                send(clients.client[receiver_connection]->sock, packet2, packet2_length, 0);
            }
            else if(proximity_before_move<=receiver_char_visual_proximity && proximity_after_move>receiver_char_visual_proximity){

                //sending char moves out of visual proximity of receiving char

                //memcpy(clients.client[sender_connection]->cmd_buffer[clients.client[sender_connection]->cmd_buffer_end], packet3, packet3_length);
                //clients.client[sender_connection]->cmd_buffer_end++;
                send(clients.client[receiver_connection]->sock, packet3, packet3_length, 0);
            }
            else if(proximity_before_move<=receiver_char_visual_proximity && proximity_after_move<=receiver_char_visual_proximity){

                //sending char moving within visual proximity of receiving char

                //memcpy(clients.client[sender_connection]->cmd_buffer[clients.client[sender_connection]->cmd_buffer_end], packet1, packet1_length);
                //clients.client[sender_connection]->cmd_buffer_end++;
                send(clients.client[receiver_connection]->sock, packet1, packet1_length, 0);
            }

        }

        //this block deals with sending char vision

        if(proximity_before_move>sender_visual_proximity && proximity_after_move<=sender_visual_proximity){

            //sending char moves into visual proximity of receiving char

            add_new_enhanced_actor_packet(clients.client[receiver_connection]->character_id, packet, &packet_length);
            send(clients.client[sender_connection]->sock, packet, packet_length, 0);
        }
        else if(proximity_before_move<=sender_visual_proximity && proximity_after_move>sender_visual_proximity){

            //sending char moves out of visual proximity of receiving char

            remove_actor_packet(clients.client[receiver_connection]->character_id, packet, &packet_length);
            send(clients.client[sender_connection]->sock, packet, packet_length, 0);
        }
        else if(proximity_before_move<=sender_visual_proximity && proximity_after_move<=sender_visual_proximity){

            //sending char moves within visual proximity of receiving char

            if(receiver_connection==sender_connection) {
                // only move our actor
                add_actor_packet(clients.client[receiver_connection]->character_id, move, packet, &packet_length);
            }
            else{
                // other actors remain stationary
                add_actor_packet(clients.client[receiver_connection]->character_id, 0, packet, &packet_length);
            }

            send(clients.client[sender_connection]->sock, packet, packet_length, 0);
        }

    }

}
