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

#ifndef CHARACTER_RACE_TBL_H_INCLUDED
#define CHARACTER_RACE_TBL_H_INCLUDED

#define RACE_TABLE_SQL "CREATE TABLE RACE_TABLE( \
        RACE_ID             INTEGER PRIMARY KEY     NOT NULL, \
        RACE_NAME           TEXT, \
        RACE_DESCRIPTION    TEXT, \
        INITIAL_EMU         INT,  \
        EMU_MULTIPLIER      REAL, \
        INITIAL_VISPROX     INT,  \
        VISPROX_MULTIPLIER  REAL, \
        INITIAL_CHATPROX    INT,  \
        CHATPROX_MULTIPLIER REAL, \
        INITIAL_NIGHTVIS    REAL, \
        NIGHTVIS_MULTIPLIER REAL, \
        CHAR_COUNT          INT)"

#endif // CHARACTER_RACE_TBL_H_INCLUDED
