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
#include "RHI/TypesRHI.h"
#include "Render/RenderResource.h"

#if ENABLE_HITPROXY
#include "Render/HitProxies.h"
#endif // ENABLE_HITPROXY

/**
 * @ingroup Engine
 * Information about pixel format
 */
struct SPixelFormatInfo
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
 * Maps members of EPixelFormat to a SPixelFormatInfo describing the format
 */
extern SPixelFormatInfo			g_PixelFormats[ PF_Max ];

/**
 * @ingroup Engine
 * @brief Offset to center of the pixel
 */
extern float					g_PixelCenterOffset;

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
 * A white texture
 */
class CWhiteTexture : public CRenderResource
{
public:
	/**
	 * @brief Get texture 2D RHI
	 * @return Return texture 2D RHI
	 */
	FORCEINLINE Texture2DRHIRef_t GetTexture2DRHI() const
	{
		return texture2DRHI;
	}

protected:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	Texture2DRHIRef_t		texture2DRHI;		/**< Texture 2D RHI */
};

/**
 * @ingroup Engine
 * A black texture
 */
class CBlackTexture : public CRenderResource
{
public:
	/**
	 * @brief Get texture 2D RHI
	 * @return Return texture 2D RHI
	 */
	FORCEINLINE Texture2DRHIRef_t GetTexture2DRHI() const
	{
		return texture2DRHI;
	}

protected:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	Texture2DRHIRef_t		texture2DRHI;		/**< Texture 2D RHI */
};

/**
 * @ingroup Engine
 * A empty normal texture
 */
class CEmptyNormalTexture : public CRenderResource
{
public:
	/**
	 * @brief Get texture 2D RHI
	 * @return Return texture 2D RHI
	 */
	FORCEINLINE Texture2DRHIRef_t GetTexture2DRHI() const
	{
		return texture2DRHI;
	}

protected:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	Texture2DRHIRef_t		texture2DRHI;		/**< Texture 2D RHI */
};

extern TGlobalResource<CWhiteTexture>		g_WhiteTexture;				/**< White texture */
extern TGlobalResource<CBlackTexture>		g_BlackTexture;				/**< Black texture */
extern TGlobalResource<CEmptyNormalTexture>	g_EmptyNormalTexture;		/**< Empty normal texture */

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
void DrawDenormalizedQuad( class CBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ );

/**
 * @ingroup Engine
 * Draws a wireframe box
 *
 * @param InSDG			Scene depth group
 * @param InBox			The CBox to use for drawing
 * @param InColor		Color of the box
 */
void DrawWireframeBox( struct SSceneDepthGroup& InSDG, const class CBox& InBox, const class CColor& InColor );

/**
 * @ingroup Engine
 * Draw circle using lines
 * 
 * @param	InSDG			Scene depth group
 * @param	InLocation		Center of the circle
 * @param	InX				X alignment axis to draw along
 * @param	InY				Y alignment axis to draw along
 * @param	InColor			Color of the circle
 * @param	InRadius		Radius of the circle
 * @param	InNumSides		Numbers of sides that the circle has
 * @param	Thickness		Optional. Thickness for lines
 */
void DrawCircle( struct SSceneDepthGroup& InSDG, const Vector& InLocation, const Vector& InX, const Vector& InY, const class CColor& InColor, float InRadius, uint32 InNumSides, float InThickness = 0.f );

#if ENABLE_HITPROXY
/**
 * @ingroup Engine
 * Draw circle using lines to hit proxy
 * 
 * @param	InSDG			Scene depth group
 * @param	InHitProxyLayer	Hit proxy layer
 * @param	InLocation		Center of the circle
 * @param	InX				X alignment axis to draw along
 * @param	InY				Y alignment axis to draw along
 * @param	InHitProxyId	Hit proxy id
 * @param	InRadius		Radius of the circle
 * @param	InNumSides		Numbers of sides that the circle has
 * @param	Thickness		Optional. Thickness for lines
 */
void DrawHitProxyCircle( struct SSceneDepthGroup& InSDG, EHitProxyLayer InHitProxyLayer, const Vector& InLocation, const Vector& InX, const Vector& InY, const CHitProxyId& InHitProxyId, float InRadius, uint32 InNumSides, float InThickness = 0.f );
#endif // ENABLE_HITPROXY

#endif // !RENDERUTILS_H
