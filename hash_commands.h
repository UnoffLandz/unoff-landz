#ifndef HASH_COMMANDS_H_INCLUDED
#define HASH_COMMANDS_H_INCLUDED

int process_hash_commands(int connection, char *text, int text_len);

void save_data(int connection);

#endif // HASH_COMMANDS_H_INCLUDED
