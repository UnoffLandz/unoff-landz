#ifndef HALF_H_INCLUDED
#define HALF_H_INCLUDED

/****************************************************************************
 *            half.h
 *
 * Author: 2009  Daniel Jungmann <dsj@gmx.net>
 * Copyright: See COPYING file that comes with this distribution
 ****************************************************************************/

#include <SDL/SDL_types.h>

float half_to_float(const Uint16 value);

Uint16 float_to_half(const float value);

#endif // HALF_H_INCLUDED
