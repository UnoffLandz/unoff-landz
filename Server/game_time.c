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
#include "game_data.h"
#include "game_time.h"
#include "idle_buffer2.h"


void update_game_time(){

    game_data.game_minutes++;

    if(game_data.game_minutes>GAME_DAY_MINUTES){

        game_data.game_minutes=0;
        game_data.game_days++;

        push_sql_command("UPDATE GAME_DATA_TABLE SET GAME_DAYS=%i WHERE GAME_DATA_ID=1", game_data.game_days);
    }

    push_sql_command("UPDATE GAME_DATA_TABLE SET GAME_MINUTES=%i WHERE GAME_DATA_ID=1", game_data.game_minutes);
}

