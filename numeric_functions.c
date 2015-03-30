/******************************************************************************************************************
	Copyright 2014 UnoffLandz

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

int Uint32_to_dec( unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4){

    /** public function - see header */

    return byte1+(256*byte2)+(256*256*byte3)+(256*256*256*byte4);
}


int Uint16_to_dec( unsigned char byte1, unsigned char byte2){

    /** public function - see header */

    return byte1+(256*byte2);
}


