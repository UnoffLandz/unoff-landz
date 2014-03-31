#ifndef HASH_COMMANDS_H_INCLUDED
#define HASH_COMMANDS_H_INCLUDED

enum{ //return values from process_hash_command
    HASH_CMD_UNSUPPORTED,
    HASH_CMD_UNKNOWN,
    HASH_CMD_EXECUTED,
    HASH_CMD_FAILED,
    HASH_CMD_ABORTED
};

enum { //return values for rename_char
    CHAR_RENAME_FAILED_DUPLICATE=-1,
    CHAR_RENAME_SUCCESS=0,
    CANNOT_CREATE_TEMP_FILE=-2
};

int process_hash_commands(int connection, char *text);

void save_data(int connection);

#endif // HASH_COMMANDS_H_INCLUDED
