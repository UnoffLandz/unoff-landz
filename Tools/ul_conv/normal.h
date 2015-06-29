#ifndef NORMAL_H_INCLUDED
#define NORMAL_H_INCLUDED

#include <SDL/SDL_types.h>

Uint16 compress_normal(const float *normal);
void uncompress_normal(const Uint16 value, float *normal);

#endif // NORMAL_H_INCLUDED
