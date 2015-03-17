int Uint32_to_dec( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){

    /** public function - see header */

    return byte1+(256*byte2)+(256*256*byte3)+(256*256*256*byte4);
}

int Uint16_to_dec( unsigned char byte1, unsigned char byte2){

    /** public function - see header */

    return byte1+(256*byte2);
}


