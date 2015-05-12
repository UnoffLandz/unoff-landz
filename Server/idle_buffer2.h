#ifndef IDLE_BUFFER2_H_INCLUDED
#define IDLE_BUFFER2_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

void push_idle_buffer2(const char *sql, int connection, int process_type, unsigned char *packet, int packet_len);
void process_idle_buffer2();

#ifdef __cplusplus
}
#endif

enum{//database buffer processing types
    IDLE_BUFFER2_PROCESS_SQL,
    IDLE_BUFFER2_PROCESS_HASH_DETAILS,
    IDLE_BUFFER2_PROCESS_CHECK_NEWCHAR,
    IDLE_BUFFER2_PROCESS_ADD_NEWCHAR,
    IDLE_BUFFER2_PROCESS_LOGIN
};

#endif // IDLE_BUFFER2_H_INCLUDED
