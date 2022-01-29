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

/**
 * @ingroup Engine
 * Handles initialization/release for a global resource
 */
template< class ResourceType >
class TGlobalResource : public ResourceType
{
public:
	/**
	 * Constructor
	 */
	TGlobalResource()
	{
		// If the resource is constructed in the rendering thread, directly initialize it
		if ( !IsInGameThread() )
		{
			( ( ResourceType* )this )->InitResource();
		}

		// If the resource is constructed outside of the rendering thread, enqueue a command to initialize it
		else
		{
			BeginInitResource( ( ResourceType* )this );
		}
	}

	/**
	 * @brief Is global resource
	 * @return Return true if this resource is global, else return false
	 */
	virtual bool IsGlobal() const override 
	{ 
		return true; 
	}

	/**
	 * Destructor
	 */
	virtual ~TGlobalResource()
	{
		// Cleanup the resource
		( ( ResourceType* )this )->ReleaseResource();
	}
};

#endif // !RENDERUTILS_H
