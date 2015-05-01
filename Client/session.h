#ifndef __EXT_SESSION_H__
#define __EXT_SESSION_H__

#include <stdint.h>

extern int session_win;
extern int exp_log_threshold;

void fill_session_win(void);
void init_session(void);
int session_reset_handler(struct widget_list *, int, int, uint32_t);
int get_session_exp_ranging(void);
void set_last_skill_exp(size_t skill, int exp);
void update_session_distance(void);

#endif
