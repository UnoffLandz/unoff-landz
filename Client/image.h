/****************************************************************************
 *            image.h
 *
 * Author: 2011  Daniel Jungmann <dsj@gmx.net>
 * Copyright: See COPYING file that comes with this distribution
 ****************************************************************************/

#ifndef UUID_26e27c11_f811_40c6_81a5_d01cf7aa36ee
#define UUID_26e27c11_f811_40c6_81a5_d01cf7aa36ee

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \ingroup     load
 * \brief 	Image manipulation functions
 */

static inline uint32_t popcount(const uint32_t x)
{
	uint32_t r;

	r = x - ((x >> 1) & 033333333333) - ((x >> 2) & 011111111111);

	return ((r + (r >> 3)) & 030707070707) % 63;
}

/*!
 * \ingroup 	textures
 * \brief 	Unpacks the given image
 *
 *      	Unpacks the given image using the masks of red, green blue and alpha.
 *
 * \param   	source The source image to use.
 * \param   	size The size of the image in pixels.
 * \param   	red The mask for the red channel.
 * \param   	green The mask for the green channel.
 * \param   	blue The mask for the blue channel.
 * \param   	alpha The mask for the alpha channel.
 * \param	dest The buffer for the unpacked image. Must be RGBA8.
 * \callgraph
 */
void fast_unpack(const uint8_t* source, const uint32_t size, const uint32_t red,
	const uint32_t green, const uint32_t blue, const uint32_t alpha, uint8_t* dest);

/*!
 * \ingroup 	textures
 * \brief 	Replaces the alpha values
 *
 *      	Replaces the alpha values of the image with the alpha values from the alpha image.
 *
 * \param   	alpha The alpha only image. Format must be A8.
 * \param   	size The size of the image in pixels.
 * \param   	source The source image to use. Format must be RGBA8.
 * \callgraph
 */
void fast_replace_a8_rgba8(const uint8_t* alpha, const uint32_t size, uint8_t* source);

/*!
 * \ingroup 	textures
 * \brief 	Replaces the alpha values
 *
 *      	Replaces the alpha values of the image with the alpha value.
 *
 * \param   	alpha The alpha value.
 * \param   	size The size of the image in pixels.
 * \param   	source The source image to use. Format must be RGBA8.
 * \callgraph
 */
void fast_replace_alpha_rgba8(const uint8_t alpha, const uint32_t size, uint8_t* source);

/*!
 * \ingroup 	textures
 * \brief 	Blend two images
 *
 *      	Blends two images using the alpha values.
 *
 * \param   	alpha The alpha values.
 * \param   	size The size of the image in pixels.
 * \param   	source0 The first source image to use. Format must be RGBA8.
 * \param   	source1 The second source image to use. Format must be RGBA8.
 * \param	dest The buffer for the blended image. Format must be RGBA8.
 * \callgraph
 */
void fast_blend(const uint8_t* alpha, const uint32_t size, const uint8_t* source0,
	const uint8_t* source1, uint8_t* dest);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* UUID_26e27c11_f811_40c6_81a5_d01cf7aa36ee */
