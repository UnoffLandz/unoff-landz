/******************************************************************************************************************
    Copyright 2014, 2015 UnoffLandz

    This file is part of unoff_server_4.

    unoff_server_4 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    unoff_server_4 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with unoff_server_4.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************************************************/
/*                             PACKET FORMAT
                               -------------

Packets received by and sent to the server conform to a common format:

        Byte  1         - Describes the protocol command
        Bytes 2 and 3   - An unsigned 16bit integer indicating the length of the packet data
                          (calculated as packet length -2).
        Byte  3 onwards - The packet data
*/

#include <string.h> //support for memcpy

#include "packet.h"
#include "logging.h"
#include "server_start_stop.h"
#include "string_functions.h"

int build_packet(struct packet_element_type *element, int element_count, unsigned char *packet){

    /** public function - see header */

    int count=0;

    int i=0;
    for(i=0; i<element_count; i++){

        if(element[i].data_type==PROTOCOL){

            packet[count]=element[i].data.numeric;
            count=count+1;
        }

        else if(element[i].data_type==DATA_LENGTH){

            count=count+2;
        }

        else if(element[i].data_type==BYTE){

            packet[count]=element[i].data.numeric;
            count=count+1;
        }

        else if(element[i].data_type==UINT16){

            packet[count]=element[i].data.numeric % 256;
            packet[count+1]=element[i].data.numeric / 256;
            count=count+2;
        }

        else if(element[i].data_type==UINT32){

            packet[count]=element[i].data.numeric % 256;
            packet[count+1]=element[i].data.numeric / 256 % 256;
            packet[count+2]=element[i].data.numeric / 256 / 256 % 256;
            packet[count+4]=element[i].data.numeric / 256 / 256 / 256 % 256;
            count=count+4;
        }

        else if(element[i].data_type==STRING_NULL){

            memcpy(packet+count, element[i].data.string, strlen(element[i].data.string)+1);
            count=count+strlen(element[i].data.string)+1;
        }

        else {

            log_event(EVENT_ERROR, "Unknown packet element in Protocol [%i]", packet[0]);
            log_text(EVENT_ERROR, "packet element data type [%i] element number [%i]", element[i].data_type, i);
            stop_server();
        }
    }

    int data_length=count-2;
    packet[1]=(data_length % 256);
    packet[2]=(data_length / 256);

    return count;
}


void read_packet(struct packet_element_type *element, int element_count, const unsigned char *packet){

    /** public function - see header */

    int packet_length = packet[1] + (packet[2] * 256) + 2;

    int count=0;

    int i=0;
    for(i=0; i<element_count; i++){

        if(element[i].data_type==PROTOCOL){

            element[i].data.numeric = packet[0];
            count++;
        }

        else if(element[i].data_type==DATA_LENGTH){

            element[i].data.numeric = packet[count] + (packet[count+1] * 256);
            count=count+2;
        }

        else if(element[i].data_type==BYTE){


            element[i].data.numeric = packet[count];
            count++;
        }

        else if(element[i].data_type==UINT16){


            element[i].data.numeric = packet[count] + (packet[count+1] * 256);
            count=count+2;
        }

        else if(element[i].data_type==UINT32){

            element[i].data.numeric= packet[count] + (packet[count+1] * 256^1) + (packet[count+2] * 256^2) + (packet[count+3] * 256^3);
            count=count+4;
        }

        else if(element[i].data_type==STRING_NULL){

            int j=0;
            for(j=count; j<packet_length; j++){

                if(packet[j]==ASCII_NULL) break;
            }

            j++;

            memcpy(element[i].data.string, packet+count, j-count);
            count=count+(j-count);
        }

        else if(element[i].data_type==STRING_SPACE){

            int j=0;
            for(j=count; j<packet_length; j++){

                if(packet[j]==ASCII_SPACE) break;
            }

            memcpy(element[i].data.string, packet+count, j-count);

            //add null terminator
            element[i].data.string[j-count+1]=ASCII_NULL;

            count=count+(j-count+1);
        }
    }
}
