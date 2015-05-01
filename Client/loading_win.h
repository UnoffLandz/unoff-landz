#ifndef LOADING_WIN_H__
#define LOADING_WIN_H__

#include "platform.h"
#include <stdint.h>

extern uint32_t loading_win;
extern uint32_t loading_win_progress_bar;
extern float progress;
extern GLuint loading_texture;

int create_loading_win(int width, int height, int snapshot);
void update_loading_win(const char *text, float progress_increase);
int destroy_loading_win(void);

#endif //LOADING_WIN_H__
