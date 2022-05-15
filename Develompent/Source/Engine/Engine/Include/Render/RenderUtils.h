/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include "Math/Math.h"
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
 * @brief Offset to center of the pixel
 */
extern float					GPixelCenterOffset;

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

/**
 * @ingroup Engine
 * Draws a quad with the given vertex positions and UVs in denormalized pixel/texel coordinates.
 * Note that the positions are affected by the current viewport
 *
 * @param InDeviceContextRHI		RHI Device context
 * @param InX						Position by X in screen pixels of the top left corner of the quad
 * @param InY						Position by Y in screen pixels of the top left corner of the quad
 * @param InSizeX					Size by X in pixels of the quad
 * @param InSizeY					Size by Y in pixels of the quad
 * @param InU						Position by U in texels of the top left corner of the quad's UV's
 * @param InV						Position by V in texels of the top left corner of the quad's UV's
 * @param InTargetSizeX				Size by X in screen pixels of the target surface
 * @param InTargetSizeY				Size by Y in screen pixels of the target surface
 * @param InTextureSizeX			Size by X in texels of the source texture
 * @param InTextureSyzeY			Size by Y in texels of the source texture
 * @param InClipSpaceQuadZ			Clip space quad by Z
 */
void DrawDenormalizedQuad( class FBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ );

/**
 * @ingroup Engine
 * Draws a wireframe box
 *
 * @param InSDG			Scene depth group
 * @param InBox			The FBox to use for drawing
 * @param InColor		Color of the box
 */
void DrawWireframeBox( struct FSceneDepthGroup& InSDG, const class FBox& InBox, const class FColor& InColor );

/**
 * @ingroup Engine
 * Draws a sphere
 * 
 * @param InSDG			Scene depth group
 * @param InCenter		Center
 * @param InRadius		Radius
 * @param InNumSides	Number sides in sphere
 * @param InNumRings	Number rings in sphere
 * @param InMaterial	Material
 */
LE_DEPRECATED( 0.4.0, "Use LSphereComponent for draw sphere" )
void DrawSphere( struct FSceneDepthGroup& InSDG, const FVector& InCenter, const FVector& InRadius, uint32 InNumSides, uint32 InNumRings, class FMaterial* InMaterial );

#endif // !RENDERUTILS_H
