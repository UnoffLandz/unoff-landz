/******************************************************************************************************************
    Copyright 2014, 2015, 2016 UnoffLandz

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

#include <string.h>     //support for memcpy function
#include <stdlib.h>//support for ssize_t datatype
#include <sys/socket.h>// support for send function
#include <errno.h>      //supports errno function
#include <stdio.h> // support sprintf function

#include "packet.h"
#include "logging.h"
#include "server_start_stop.h"
#include "string_functions.h"
#include "clients.h"

void send_packet(int socket, void *packet, size_t packet_length){

    /** public function - see header */

    log_packet(socket, packet, SEND);

    size_t total_bytes_sent=0;

    //make multiple attempts to send data if not everything is sent first attempt
    do{

        int bytes_sent=(int)send(socket, packet, packet_length, 0);

        if(bytes_sent==-1){

            int errnum=errno;

            log_event(EVENT_ERROR, "send failed in function %s: module %s: line %i", __func__, __FILE__, __LINE__);
            log_text(EVENT_ERROR, "error [%i] [%s]", errnum, strerror(errnum));

            int32_t actor_node=client_socket[socket].actor_node;
            log_text(EVENT_ERROR, "actor node [%i] actor name [%s] protocol [%i]", actor_node, clients.client[actor_node].char_name, packet);

            return;
        }

        total_bytes_sent+=(size_t)bytes_sent;

        if(total_bytes_sent==packet_length) break;

        log_event(EVENT_SESSION, "partial packet send %i of %i", bytes_sent, packet_length);

    }while(1);
}


size_t get_packet_length(const unsigned char *packet){

    /** public function - see header */

    int packet_length=packet[1] + (packet[2] * 256) +2;

    size_t _packet_length=(size_t)packet_length;

    return _packet_length;
}
