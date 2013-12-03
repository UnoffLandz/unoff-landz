#include <stdio.h>
#include <stdlib.h>

int Uint32_to_dec( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){

    return byte1+(256*byte2)+(256*256*byte3)+(256*256*256*byte4);

}

int Uint16_to_dec( unsigned char byte1, unsigned char byte2){

    return byte1+(256*byte2);

}

float Uint32_to_float(unsigned char *byte){

    union test
	{
        unsigned char buf[4];
        float number;
	}test;

	test.buf[0] = byte[0];
	test.buf[1] = byte[1];
	test.buf[2] = byte[2];
	test.buf[3] = byte[3];

	return test.number;
}
