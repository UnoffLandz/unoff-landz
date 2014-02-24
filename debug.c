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

