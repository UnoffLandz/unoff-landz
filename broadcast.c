#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "global.h"
#include "character_movement.h"
#include "protocol.h"
#include "chat.h"
#include "string_functions.h" //needed for ASCII_SPACE
#include "character_inventory.h"

int get_char_visual_range(int connection){

    int race_id=clients.client[connection].race_type;
    int initial_visual_proximity=race[race_id].initial_visual_proximity;
    int visual_proximity_multiplier=race[race_id].visual_proximity_multiplier;

    return initial_visual_proximity + (visual_proximity_multiplier * clients.client[connection].vitality);
}

int get_proximity(int tile_pos_1, int tile_pos_2, int map_axis){

    int x_diff=abs((tile_pos_1 % map_axis) - (tile_pos_2 % map_axis));
    int y_diff=abs((tile_pos_1 / map_axis) - (tile_pos_2 / map_axis));

    if(x_diff>y_diff) return x_diff; else return y_diff;
}

int get_char_chat_range(int connection){

    int race_id=clients.client[connection].race_type;
    int initial_chat_proximity=race[race_id].initial_chat_proximity;
    int chat_proximity_multiplier=race[race_id].chat_proximity_multiplier;

    return initial_chat_proximity + (chat_proximity_multiplier * clients.client[connection].will);
}

void broadcast_bag_drop(int bag_id, int map_id){

    int i=0, j=0;
    int char_tile=0, bag_tile=0;

    printf("dropping bag [%i] on map [%i] name [%s]\n", bag_id, map_id, maps.map[map_id]->map_name);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];

        if(clients.client[j].status==LOGGED_IN){

            char_tile=clients.client[j].map_tile;
            bag_tile=bag_list[bag_id].tile_pos;

            //only send bag drop to chars that are within visual proximity of the bag
            if(get_proximity(char_tile, bag_tile, maps.map[map_id]->map_axis) < get_char_visual_range(j)) {

                  send_get_new_bag(j, bag_id);
            }
        }
    }

    bag_list[bag_id].status=USED;
}

void broadcast_bag_poof(int bag_id, int map_id){

    int i=0, j=0, k=0;
    int char_tile=0, bag_tile=0;

    printf("poofing bag [%i] on map [%i] name [%s]\n", bag_id, map_id, maps.map[map_id]->map_name);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        j=maps.map[map_id]->client_list[i];

        if(clients.client[j].status==LOGGED_IN){

            char_tile=clients.client[j].map_tile;
            bag_tile=bag_list[bag_id].tile_pos;

            //zero the bag inventory
            for(k=0; k<MAX_BAG_SLOTS; k++){
                bag_list[bag_id].inventory[k].amount=0;
                bag_list[bag_id].inventory[k].image_id=0;
            }

            //only send char poof to chars that are within visual proximity of the bag
            if(get_proximity(char_tile, bag_tile, maps.map[map_id]->map_axis) < get_char_visual_range(j)) {

                send_destroy_bag(j, bag_id);

                //close the bag inventory grid if client is standing on bag
                if(bag_tile==char_tile && clients.client[j].bag_open==TRUE){

                    clients.client[j].bag_open=FALSE;
                    send_s_close_bag(j);
                }
            }
        }
    }

    bag_list[bag_id].status=UNUSED;
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

    int sender_char_tile=clients.client[sender_connection].map_tile;
    int map_id=clients.client[sender_connection].map_id;
    int map_axis=maps.map[map_id]->map_axis;

    int receiver_char_tile=0;
    int receiver_connection=0;

    //parse the local map list and get connections for all chars that are there
    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        if(receiver_connection!=sender_connection){

            receiver_char_tile=clients.client[receiver_connection].map_tile;

            //broadcast only to chars in local text proximity
            if(get_proximity(sender_char_tile, receiver_char_tile, map_axis) < get_char_chat_range(receiver_connection)) {

                send_raw_text_packet(receiver_connection, CHAT_LOCAL, text_in);
            }
        }
    }
}

void broadcast_channel_chat(int chan, int sender_connection, char *text_in){

    int i=0;
    int receiver_connection=0;
    int receiver_active_chan=0;
    char text_out[1024]="";

    //send to channel
    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        receiver_connection=channels.channel[chan]->client_list[i];

        if(receiver_connection!=sender_connection){

            receiver_active_chan=clients.client[receiver_connection].active_chan;

            //show non-active chan in darker grey
            if(clients.client[receiver_connection].chan[receiver_active_chan-1]==chan){
                sprintf(text_out, "%c[%s @ %i(%s)] %s", c_grey1+127, clients.client[sender_connection].char_name, chan, channels.channel[chan]->channel_name, text_in);
            }
            else {
                sprintf(text_out, "%c[%s @ %i(%s)] %s", c_grey1+127, clients.client[sender_connection].char_name, chan, channels.channel[chan]->channel_name, text_in);
            }

            send_raw_text_packet(receiver_connection, CHAT_SERVER, text_out);
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

            send_raw_text_packet(receiver_connection, CHAT_SERVER, text_in);
        }
    }
}

void broadcast_guild_channel_chat(int sender_connection, char *text_in){

    int i=0;
    int guild_id=clients.client[sender_connection].guild_id;
    int chan=guilds.guild[guild_id]->guild_chan_number;
    int receiver_connection=0;

    //send to channel
    for(i=0; i<channels.channel[chan]->client_list_count; i++){

        receiver_connection=channels.channel[chan]->client_list[i];

        if(receiver_connection!=sender_connection){

            send_raw_text_packet(receiver_connection, CHAT_GM, text_in);
        }
    }
}

void add_new_enhanced_actor_packet(int connection, unsigned char *packet, int *packet_length){

    int i=0,j=0;
    int data_length=0;
    int guild_id=clients.client[connection].guild_id;

    int map_id=clients.client[connection].map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int x=clients.client[connection].map_tile % map_axis;
    int y=clients.client[connection].map_tile / map_axis;

    packet[i++]=ADD_NEW_ENHANCED_ACTOR;                               // protocol
    packet[i++]=0;                                                    // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;                                                    // dummy the msb (we'll put the proper value in later
    packet[i++]=connection % 256;                                     // char_id lsb
    packet[i++]=connection / 256;                                     // char_id msb
    packet[i++]=x % 256;                                              // x axis lsb  2
    packet[i++]=x / 256;                                              // x axis msb  3
    packet[i++]=y % 256;                                              // y axis lsb  4
    packet[i++]=y / 256;                                              // y axis msb  5
    packet[i++]=0;                                                    // z axis lsb  6
    packet[i++]=0;                                                    // z axis msb  7
    packet[i++]=45;                                                   // rotation lsb 8
	packet[i++]=0;                                                    // rotation msb 9

    packet[i++]=clients.client[connection].char_type;                //              10

    packet[i++]=0;                                                    // unknown
	packet[i++]=clients.client[connection].skin_type;
	packet[i++]=clients.client[connection].hair_type;
	packet[i++]=clients.client[connection].shirt_type;
	packet[i++]=clients.client[connection].pants_type;
	packet[i++]=clients.client[connection].boots_type;  //16
	packet[i++]=clients.client[connection].head_type;

	packet[i++]=clients.client[connection].shield_type;
	packet[i++]=clients.client[connection].weapon_type;
	packet[i++]=clients.client[connection].cape_type;
	packet[i++]=clients.client[connection].helmet_type; //21

    packet[i++]=clients.client[connection].frame;

	packet[i++]=clients.client[connection].max_health % 256;         // 23 max health lsb
	packet[i++]=clients.client[connection].max_health / 256;         // 24 max health msb
	packet[i++]=clients.client[connection].current_health % 256;     // 25 current health lsb
	packet[i++]=clients.client[connection].current_health / 256;     // 26 current health msb
	packet[i++]=1;                                                   // 27 special char type HUMAN / NPC

    // add char name to packet
	for(j=0;j< (int)strlen(clients.client[connection].char_name); j++){
            packet[i++]=clients.client[connection].char_name[j];
	}

    // add guild name
	if(guild_id>0) {

        packet[i++]=ASCII_SPACE;
        packet[i++]=guilds.guild[guild_id]->tag_colour;

        for(j=0; j< (int)strlen(guilds.guild[guild_id]->guild_tag); j++){
	        packet[i++]=guilds.guild[guild_id]->guild_tag[j];
        }
	}

    packet[i++]='\0';

	packet[i++]=0; // unknown
	packet[i++]=64; //char height (min=2 max=127)
	packet[i++]=255; //char riding (none=255  brown horse=200)
	packet[i++]=64;// neck attachment (none=64 10=

    *packet_length=i;

    // now we know the packet length we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
}

void broadcast_add_new_enhanced_actor_packet(int connection){

    //broadcasts a single char to all connected clients

    int i=0;
    unsigned char packet[1024];
    int packet_length=0;

    int receiving_char_tile=0;
    int receiver_connection=0;

    int map_id=clients.client[connection].map_id;
    int char_tile=clients.client[connection].map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    // create the packet to be broadcast
    add_new_enhanced_actor_packet(connection, packet, &packet_length);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];
        receiving_char_tile=clients.client[receiver_connection].map_tile;

        //restrict to characters within visual proximity
        if(get_proximity(char_tile, receiving_char_tile, map_axis)<get_char_visual_range(receiver_connection)){

            send(receiver_connection, packet, packet_length, 0);
        }
    }
}

void remove_actor_packet(int connection, unsigned char *packet, int *packet_length){

    int i;
    int data_length=0;

    i=0;                       /* zero the packet length counter */
    packet[i++]=6;             /* protocol       */
    packet[i++]=0;             /* dummy the lsb (we'll put the proper value in later  */
    packet[i++]=0;             /* dummy the msb (we'll put the proper value in later  */
    packet[i++]=connection % 256;      /* char_id lsb    */
    packet[i++]=connection / 256;      /* char_id msb    */

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

    int map_id=clients.client[sender_connection].map_id;
    int char_tile=clients.client[sender_connection].map_tile;
    int map_axis=maps.map[map_id]->map_axis;

    int receiver_connection=0;
    int receiver_char_tile=0;
    int receiver_char_visual_range=get_char_visual_range(receiver_connection);

    remove_actor_packet(sender_connection, packet, &packet_length);

    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];
        receiver_char_tile=clients.client[receiver_connection].map_tile;

        //filter for receiving char visual proximity
        if(get_proximity(char_tile, receiver_char_tile, map_axis) < receiver_char_visual_range){

             send(receiver_connection, packet, packet_length, 0);
         }
    }
}

void add_actor_packet(int connection, unsigned char move, unsigned char *packet, int *packet_length){

    int i=0;
    int data_length=0;

    packet[i++]=2;               // protocol
    packet[i++]=0;               // dummy the lsb (we'll put the proper value in later
    packet[i++]=0;               // dummy the msb (we'll put the proper value in later
    packet[i++]= connection % 256;  // char_id lsb
    packet[i++]= connection / 256;  // char_id msb
    packet[i++]=move;            // actor command

    *packet_length=i;

    // now we know the packet length, we can calculate the data length by subtracting 2
    data_length=i-2;

    // now we know our data length, we can will in the proper values for our lsb/msb
    packet[1]=data_length % 256;
    packet[2]=data_length / 256;
}

void broadcast_actor_packet(int sender_connection, unsigned char move, int sender_destination_tile){

    //broadcast the specified char movement to all connected clients

    int i=0;

    unsigned char packet[1024];// sending char packets
    int packet_length=0;

    unsigned char packet1[1024];// receiving char add_actor packet
    int packet1_length=0;

    unsigned char packet2[1024];// receiving char add_enhanced_actor packet
    int packet2_length=0;

    unsigned char packet3[1024];// receiving char remove_actor packet
    int packet3_length=0;

    int sender_current_tile=clients.client[sender_connection].map_tile;
    int map_id=clients.client[sender_connection].map_id;
    int map_axis=maps.map[map_id]->map_axis;
    int sender_visual_proximity=get_char_visual_range(sender_connection);

    int proximity_before_move=0;
    int proximity_after_move=0;

    int receiver_connection=0;
    int receiver_char_tile=0;
    int receiver_char_visual_proximity=0;

    // pre-create packets that will be sent more than once in order to save time
    add_actor_packet(sender_connection, move, packet1, &packet1_length);
    add_new_enhanced_actor_packet(sender_connection, packet2, &packet2_length);
    remove_actor_packet(sender_connection, packet3, &packet3_length);

    // broadcast sender char move to all receiver clients
    for(i=0; i<maps.map[map_id]->client_list_count; i++){

        receiver_connection=maps.map[map_id]->client_list[i];

        receiver_char_tile=clients.client[receiver_connection].map_tile;

        proximity_before_move=get_proximity(sender_current_tile, receiver_char_tile, map_axis);
        proximity_after_move=get_proximity(sender_destination_tile, receiver_char_tile, map_axis);

        //This block deals with receiving char vision
        if(receiver_connection!=sender_connection){

            if(proximity_before_move>receiver_char_visual_proximity && proximity_after_move<=receiver_char_visual_proximity){

                //sending char moves into visual proximity of receiving char
                send(receiver_connection, packet2, packet2_length, 0);
            }
            else if(proximity_before_move<=receiver_char_visual_proximity && proximity_after_move>receiver_char_visual_proximity){

                //sending char moves out of visual proximity of receiving char
                send(receiver_connection, packet3, packet3_length, 0);
            }
            else if(proximity_before_move<=receiver_char_visual_proximity && proximity_after_move<=receiver_char_visual_proximity){

                //sending char moving within visual proximity of receiving char
                send(receiver_connection, packet1, packet1_length, 0);
            }
        }

        //this block deals with sending char vision
        if(proximity_before_move>sender_visual_proximity && proximity_after_move<=sender_visual_proximity){

            //sending char moves into visual proximity of receiving char
            add_new_enhanced_actor_packet(receiver_connection, packet, &packet_length);
            send(sender_connection, packet, packet_length, 0);
        }
        else if(proximity_before_move<=sender_visual_proximity && proximity_after_move>sender_visual_proximity){

            //sending char moves out of visual proximity of receiving char
            remove_actor_packet(receiver_connection, packet, &packet_length);
            send(sender_connection, packet, packet_length, 0);
        }
        else if(proximity_before_move<=sender_visual_proximity && proximity_after_move<=sender_visual_proximity){

            //sending char moves within visual proximity of receiving char
            if(receiver_connection==sender_connection) {

                // only move our actor
                add_actor_packet(receiver_connection, move, packet, &packet_length);
            }
            else{
                // other actors remain stationary
                add_actor_packet(receiver_connection, 0, packet, &packet_length);
            }

            send(sender_connection, packet, packet_length, 0);
        }
    }
}

