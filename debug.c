#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat.h"
#include "global.h"

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
    i++;
    printf("Skin [%i]\n", packet[i++]);
    printf("Hair [%i]\n", packet[i++]);
    printf("Shirt [%i]\n", packet[i++]);
    printf("Pants [%i]\n", packet[i++]);
    printf("Boots [%i]\n", packet[i++]);
    printf("Type [%i]\n", packet[i++]);
    printf("Head [%i]\n", packet[i++]);

    printf("\n");
}

void debug_channels(int char_id){

    printf("\nChannels for char [%s]...\n", characters.character[char_id]->char_name);
    printf("slot 1 - [%i] [%s]\n", characters.character[char_id]->chan[0], channels.channel[characters.character[char_id]->chan[0]]->channel_name);
    printf("slot 2 - [%i] [%s]\n", characters.character[char_id]->chan[1], channels.channel[characters.character[char_id]->chan[1]]->channel_name);
    printf("slot 3 - [%i] [%s]\n", characters.character[char_id]->chan[2], channels.channel[characters.character[char_id]->chan[2]]->channel_name);
    printf("active channel [%i]\n", characters.character[char_id]->active_chan);
    printf("\n");
}
