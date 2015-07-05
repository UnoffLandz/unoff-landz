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


/** RESULT  : converts a 32bit to an float value

    RETURNS : returns the value of the bits

    PURPOSE : to convert 4byte float values contained in elm (map) files

    NOTES   :
*/
float Uint32_to_float(unsigned char *bytes);


#endif // NUMERIC_FUNCTIONS_H_INCLUDED
