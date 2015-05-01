/****************************************************************************
 *            half.h
 *
 * Author: 2009  Daniel Jungmann <dsj@gmx.net>
 * Copyright: See COPYING file that comes with this distribution
 ****************************************************************************/

#ifndef	_HALF_H_
#define	_HALF_H_

#include <SDL_types.h>

float half_to_float(const uint16_t value);

uint16_t float_to_half(const float value);

#endif	/* _HALF_H_ */

