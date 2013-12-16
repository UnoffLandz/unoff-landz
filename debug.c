#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include "global.h"

void show_map(int tile, unsigned char *height_map) {


    int x=0, y=0, z=0;

    printf("\n\n");

    for(y=40; y>=0; y--){

        printf("|");

        for(x=0; x<70; x++){

            z=x+(y*192);

            if(height_map[z]>0){

                if(z!=tile){

                    if(height_map[z]>1 && height_map[z]<9) {
                        printf("%i", height_map[z]);
                    }
                    else {
                        printf("-");
                    }
                }
                else {
                    printf("X");
                }
            }
            else {
                if(z!=tile){
                    printf(" ");
                }
                else {
                    printf("X");
                }

            }
        }

        printf("\n");
    }
    printf("----------------------------------------\n");

    //printf("tile [%i] height [%i]\n", tile, height_map[tile]);
}

void debug_raw_text(int connection, char *text, int text_len){

    switch(text[0]){

                case '@': // chat

                    printf("RAW_TEXT (CHANNEL TEXT) %s\n", text);

                    //remove @
                    memcpy(text, text+1, text_len-1);
                    text[text_len-1]='\0';

                    if(process_chat(connection, text)==-1) printf("chan text failed - char not in any channel\n");

                break;

                case '#':// hash commands

                    printf("RAW_TEXT (#COMMAND) %s\n", text);

                break;

                default://local chat

                    printf("RAW_TEXT (LOCAL TEXT) %s\n", text);

                break;
            }
}

void debug_new_char_packet(unsigned char packet[1024]){

    int i=0;
    int lsb=packet[1]%256;
    int msb=packet[2]/256;

    printf("\nDEBUG NEW CHAR PACKET\n");
    printf("-----------------------\n");

    printf("Protocol [%i]\n", packet[0]);

    printf("Data length [%i]\n", lsb+(msb*255));

    printf("Name & password ");
    for(i=3; packet[i]!=0; i++) {
        printf("%c", packet[i]);
    }
    printf("\n");
    i++;// null byte terminating  the nam and password
    printf("Skin [%i]\n", packet[i++]);
    printf("Hair [%i]\n", packet[i++]);
    printf("Shirt [%i]\n", packet[i++]);
    printf("Pants [%i]\n", packet[i++]);
    printf("Boots [%i]\n", packet[i++]);
    printf("Type [%i]\n", packet[i++]);
    printf("Head [%i]\n", packet[i++]);

    printf("\n");
}

void debug_char_struct(int connection){

    int char_id=clients.client[connection]->character_id;

    printf("\nDEBUG CHAR STRUCT\n");
    printf("-------------------\n");

    printf("char_id %i\n",char_id);
    printf("name %s\n",characters.character[char_id]->char_name);
    printf("password %s\n",characters.character[char_id]->password);
    printf("time played %i\n",characters.character[char_id]->time_played);
    printf("char status %i\n",characters.character[char_id]->char_status);
    printf("active chan %i\n",characters.character[char_id]->active_chan);
    printf("chan 1 %i\n",characters.character[char_id]->chan[0]);
    printf("chan 2 %i\n",characters.character[char_id]->chan[1]);
    printf("chan 3 %i\n",characters.character[char_id]->chan[2]);
    printf("chan 4 %i\n",characters.character[char_id]->chan[3]);
    printf("gm permission %i\n",characters.character[char_id]->gm_permission);
    printf("ig permission %i\n",characters.character[char_id]->ig_permission);
    printf("map id %i\n",characters.character[char_id]->map_id);
    printf("map tile %i\n",characters.character[char_id]->map_tile);
    printf("guild id %i\n",characters.character[char_id]->guild_id);
    printf("skin %i\n",characters.character[char_id]->skin_type);
    printf("hair %i\n",characters.character[char_id]->hair_type);
    printf("shirt %i\n",characters.character[char_id]->shirt_type);
    printf("pants %i\n",characters.character[char_id]->pants_type);
    printf("boots %i\n",characters.character[char_id]->boots_type);
    printf("char type %i\n",characters.character[char_id]->char_type);
    printf("heads %i\n",characters.character[char_id]->head_type);
    printf("shield %i\n",characters.character[char_id]->shield_type);
    printf("weapon %i\n",characters.character[char_id]->weapon_type);
    printf("cape %i\n",characters.character[char_id]->cape_type);
    printf("helmet %i\n",characters.character[char_id]->helmet_type);
    printf("neck %i\n",characters.character[char_id]->neck_type);
    printf("max health %i\n",characters.character[char_id]->max_health);
    printf("current health %i\n",characters.character[char_id]->current_health);
    printf("visual proximity %i\n",characters.character[char_id]->visual_proximity);
    printf("local_text_proximity %i\n",characters.character[char_id]->local_text_proximity);
    //printf("last in game %d\n",characters.character[char_id]->last_in_game);
    //printf("char created %d\n",characters.character[char_id]->char_created);
    //printf("joined guild %d\n",characters.character[char_id]->joined_guild);
}

void debug_map_client_list(int map_id){

    int i=0;

    printf("\nDEBUG MAP CLIENT LIST (map %i)\n", map_id);
    printf("-------------------\n");

    for(i=0; i<maps.map[map_id]->client_list_count; i++){
        printf("i %i, connection %i\n", i, maps.map[map_id]->client_list[i]);
    }
}

void debug_channel_client_list(int chan){

    int i=0;

    printf("\nDEBUG CHANNEL CLIENT LIST (chan %i)\n", chan);
    printf("----------------------\n");

    for(i=0; i<channels.channel[chan]->client_list_count; i++){
        printf("i %i connection %i\n", i, channels.channel[chan]->client_list[i]);
    }
}

void debug_channels(int char_id){

    printf("\nChannels for char [%s]...\n", characters.character[char_id]->char_name);
    printf("slot 1 - [%i] [%s]\n", characters.character[char_id]->chan[0], channels.channel[characters.character[char_id]->chan[0]]->channel_name);
    printf("slot 2 - [%i] [%s]\n", characters.character[char_id]->chan[1], channels.channel[characters.character[char_id]->chan[1]]->channel_name);
    printf("slot 3 - [%i] [%s]\n", characters.character[char_id]->chan[2], channels.channel[characters.character[char_id]->chan[2]]->channel_name);
    printf("active channel [%i]\n", characters.character[char_id]->active_chan);
    printf("\n");
}
