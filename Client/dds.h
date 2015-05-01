/****************************************************************************
 *            dds.h
 *
 * Author: 2011  Daniel Jungmann <el.3d.source@googlemail.com>
 * Copyright: See COPYING file that comes with this distribution
 ****************************************************************************/

#ifndef	_DDS_H_
#define	_DDS_H_

#include "platform.h"

#ifndef	MAKEFOURCC
	#define MAKEFOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
#endif	/* MAKEFOURCC */

typedef struct DdsPixelFormat
{
	uint32_t m_size;
	uint32_t m_flags;
	uint32_t m_fourcc;
	uint32_t m_bit_count;
	uint32_t m_red_mask;
	uint32_t m_green_mask;
	uint32_t m_blue_mask;
	uint32_t m_alpha_mask;
} DdsPixelFormat;

typedef struct DdsCaps
{
	uint32_t m_caps1;
	uint32_t m_caps2;
	uint32_t m_caps3;
	uint32_t m_caps4;
} DdsCaps;

typedef struct DdsHeader
{
	uint32_t m_size;
	uint32_t m_flags;
	uint32_t m_height;
	uint32_t m_width;
	uint32_t m_size_or_pitch;
	uint32_t m_depth;
	uint32_t m_mipmap_count;
	uint32_t m_reserved1[11];
	DdsPixelFormat m_pixel_format;
	DdsCaps m_caps;
	uint32_t m_reserved2;
} DdsHeader;

#define DDS_PIXEL_FORMAT_SIZE	(8 * sizeof(uint32_t))
#define DDS_CAPS_SIZE		(4 * sizeof(uint32_t))
#define DDS_HEADER_SIZE		(19 * sizeof(uint32_t) + DDS_PIXEL_FORMAT_SIZE + DDS_CAPS_SIZE)

/* Header caps. */
#define DDSD_CAPS			0x00000001
#define DDSD_HEIGHT			0x00000002
#define DDSD_WIDTH			0x00000004
#define DDSD_PITCH			0x00000008
#define DDSD_PIXELFORMAT		0x00001000
#define DDSD_MIPMAPCOUNT		0x00020000
#define DDSD_LINEARSIZE			0x00080000
#define DDSD_DEPTH			0x00800000
#define DDSD_MIN_FLAGS			(DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT)

/* Pixel format constants. */
#define DDPF_ALPHAPIXELS		0x00000001
#define DDPF_ALPHA			0x00000002
#define DDPF_FOURCC			0x00000004
#define DDPF_RGB			0x00000040
#define DDPF_RGBA			0x00000041
#define DDPF_PALETTEINDEXED1		0x00000800
#define DDPF_PALETTEINDEXED2		0x00001000
#define DDPF_PALETTEINDEXED4		0x00000008
#define DDPF_PALETTEINDEXED8		0x00000020
#define DDPF_LUMINANCE			0x00020000
#define DDPF_ALPHAPREMULT		0x00008000
#define DDPF_NORMAL			0x80000000

#define DDSCAPS_COMPLEX			0x00000008
#define DDSCAPS_TEXTURE			0x00001000
#define DDSCAPS_MIPMAP			0x00400000

/* Cube map constants. */
#define DDSCAPS2_CUBEMAP		0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00008000
#define DDSCAPS2_CUBEMAP_ALL_FACES	(DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX \
					| DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY	\
					| DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define DDSCAPS2_VOLUME			0x00200000

/* Special FourCC codes. */
#define DDSFMT_DXT1			MAKEFOURCC('D', 'X', 'T', '1')
#define DDSFMT_DXT2			MAKEFOURCC('D', 'X', 'T', '2')
#define DDSFMT_DXT3			MAKEFOURCC('D', 'X', 'T', '3')
#define DDSFMT_DXT4			MAKEFOURCC('D', 'X', 'T', '4')
#define DDSFMT_DXT5			MAKEFOURCC('D', 'X', 'T', '5')
#define DDSFMT_ATI1			MAKEFOURCC('A', 'T', 'I', '1')
#define DDSFMT_ATI2			MAKEFOURCC('A', 'T', 'I', '2')
#define DDSFMT_RXGB			MAKEFOURCC('R', 'X', 'G', 'B')

/* RGB formats. */
#define DDSFMT_R8G8B8			20
#define DDSFMT_A8R8G8B8			21
#define DDSFMT_X8R8G8B8			22
#define DDSFMT_R5G6B5			23
#define DDSFMT_X1R5G5B5			24
#define DDSFMT_A1R5G5B5			25
#define DDSFMT_A4R4G4B4			26
#define DDSFMT_R3G3B2			27
#define DDSFMT_A8			28
#define DDSFMT_A8R3G3B2			29
#define DDSFMT_X4R4G4B4			30
#define DDSFMT_A2B10G10R10		31
#define DDSFMT_A8B8G8R8			32
#define DDSFMT_X8B8G8R8			33
#define DDSFMT_G16R16			34
#define DDSFMT_A2R10G10B10		35
#define DDSFMT_A16B16G16R16		36

/* Palette formats. */
#define DDSFMT_A8P8			40
#define DDSFMT_P8			41

/* Luminance formats. */
#define DDSFMT_L8			50
#define DDSFMT_A8L8			51
#define DDSFMT_A4L4			52

/* Floating point formats. */
#define DDSFMT_R16F			111
#define DDSFMT_G16R16F			112
#define DDSFMT_A16B16G16R16F		113
#define DDSFMT_R32F			114
#define DDSFMT_G32R32F			115
#define DDSFMT_A32B32G32R32F		116

/* Magic number of dds files. */
#define DDSMAGIC			MAKEFOURCC('D', 'D', 'S', ' ')

typedef struct DXTColorBlock
{
	uint16_t m_colors[2];
	uint8_t m_indices[4];
} DXTColorBlock;

typedef struct DXTExplicitAlphaBlock
{
	uint16_t m_alphas[4];
} DXTExplicitAlphaBlock;

typedef struct DXTInterpolatedAlphaBlock
{
	uint8_t m_alphas[2];
	uint8_t m_indices[6];
} DXTInterpolatedAlphaBlock;

void unpack_dxt1(DXTColorBlock *block, uint8_t *values);
void unpack_dxt3(DXTExplicitAlphaBlock *alpha_block, DXTColorBlock *color_block, uint8_t *values);
void unpack_dxt5(DXTInterpolatedAlphaBlock *alpha_block, DXTColorBlock *color_block, uint8_t *values);
void unpack_ati1(DXTInterpolatedAlphaBlock *block, uint8_t *values);
void unpack_ati2(DXTInterpolatedAlphaBlock *first_block, DXTInterpolatedAlphaBlock *second_block,
	uint8_t *values);

#endif	/* _DDS_H_ */
