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

#ifndef HASH_COMMANDS_H_INCLUDED
#define HASH_COMMANDS_H_INCLUDED

enum{ //return values from process_hash_command
    HASH_CMD_UNSUPPORTED,
    HASH_CMD_UNKNOWN,
    HASH_CMD_EXECUTED,
    HASH_CMD_FAILED,
    HASH_CMD_ABORTED
};

void process_hash_commands(int connection, char *text);

#endif // HASH_COMMANDS_H_INCLUDED
