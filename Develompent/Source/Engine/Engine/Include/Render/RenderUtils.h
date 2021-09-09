/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include "Misc/Types.h"
#include "RHI/BaseSurfaceRHI.h"

/**
 * @ingroup Engine
 * Information about pixel format
 */
struct FPixelFormatInfo
{
	const tchar*			name;			/**< Pixel format name */
	uint32					blockSizeX;		/**< Size of block by X */
	uint32					blockSizeY;		/**< Size of block by Y */
	uint32					blockSizeZ;		/**< Size of block by Z */
	uint32					blockBytes;		/**< Count bytes in one block */
	uint32					numComponents;	/**< Number components in block */
	uint32					platformFormat;	/**< Platform specific token, e.g. D3DFORMAT with D3D11RHI */
	uint32					flags;			/**< Format specific internal flags, e.g. whether SRGB is supported with this format */
	bool					supported;		/**< Whether the texture format is supported on the current platform/ rendering combination */
	EPixelFormat			engineFormat;	/**< Pixel format in engine */
};

/**
 * @ingroup Engine
 * Maps members of EPixelFormat to a FPixelFormatInfo describing the format
 */
extern FPixelFormatInfo			GPixelFormats[ PF_Max ];

#endif // !RENDERUTILS_H
