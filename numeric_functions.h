#ifndef NUMERIC_FUNCTIONS_H_INCLUDED
#define NUMERIC_FUNCTIONS_H_INCLUDED

/** RESULT  : converts a 32bit to an integer value

    RETURNS : returns the value of the bits

    PURPOSE : to convert 4byte integer values contained in files and packets

    NOTES   :
*/
int Uint32_to_dec( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);


/** RESULT  : converts a 16bit to an integer value

    RETURNS : returns the value of the bits

    PURPOSE : to convert 2byte integer values contained in files and packets

    NOTES   :
*/
int Uint16_to_dec( unsigned char byte1, unsigned char byte2);

#endif // NUMERIC_FUNCTIONS_H_INCLUDED
