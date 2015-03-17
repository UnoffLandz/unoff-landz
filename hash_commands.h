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
