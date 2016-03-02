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

#ifndef COLOUR_H_INCLUDED
#define COLOUR_H_INCLUDED


#define MIN_COLOUR_CODE 0
#define MAX_COLOUR_CODE 27

enum {// colours
    c_red1      =0,
    c_orange1   =1,
    c_yellow1   =2,
    c_green1    =3,
    c_blue1     =4,
    c_purple1   =5,
    c_grey1     =6,
    c_red2      =7,
    c_orange2   =8,
    c_yellow2   =9,
    c_green2    =10,
    c_blue2     =11,
    c_purple2   =12,
    c_grey2     =13,
    c_red3      =14,
    c_orange3   =15,
    c_yellow3   =16,
    c_green3    =17,
    c_blue3     =18,
    c_purple3   =19,
    c_grey3     =20,
    c_red4      =21,
    c_orange4   =22,
    c_yellow4   =23,
    c_green4    =24,
    c_blue4     =25,
    c_purple4   =26,
    c_grey4     =27
};


/** RESULT  : gives the colour code for a colour name

    RETURNS : colour code or -1 if colour does not exist

    PURPOSE :

    NOTES   :
*/
int get_colour_code( char *colour_name);



#endif // COLOUR_H_INCLUDED
