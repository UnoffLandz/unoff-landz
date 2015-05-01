/*!
 * \file
 * \ingroup other
 * \brief color handling
 */
#ifndef __COLORS_H__
#define __COLORS_H__

#include <SDL_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * A color structure for RGB color values.
 */
typedef struct
{
	uint8_t r1;
	uint8_t g1;
	uint8_t b1;
	uint8_t r2;
	uint8_t g2;
	uint8_t b2;
	uint8_t r3;
	uint8_t g3;
	uint8_t b3;
	uint8_t r4;
	uint8_t g4;
	uint8_t b4;

} color_rgb;

extern const color_rgb colors_list[]; /*!< the global list of colors we use */

#ifdef __cplusplus
} // extern "C"
#endif

#endif
