/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <string>

#include "Misc/RefCounted.h"
#include "Render/RenderResource.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Render target
 */
class CRenderTarget : public CRenderResource, public CRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	CRenderTarget();

	/**
	 * @brief Update RHI render target
	 *
	 * @param InIsDestroyed		Is need destroy render target
	 * @param InNewSizeX		New width
	 * @param InNewSizeY		New height
	 * @param InPixelFormat		New render target pixel format
	 * @param InFlags			New texture create flags (use ETextureCreateFlags). Note: flag TCF_ResolveTargetable will automatically add by the method
	 * @param InName			Render target name for debug, if empty him is ignoring (in shipping not used)
	 */
	void Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, EPixelFormat InPixelFormat, uint32 InFlags = 0, const std::wstring& InName = TEXT( "" ) );

	/**
	 * @brief Get size X
	 * @return Return size X
	 */
	FORCEINLINE uint32 GetSizeX() const
	{
		return sizeX;
	}

	/**
	 * @brief Get size Y
	 * @return Return size Y
	 */
	FORCEINLINE uint32 GetSizeY() const
	{
		return sizeY;
	}

	/**
	 * @brief Get pixel format
	 * @return Return pixel format
	 */
	FORCEINLINE EPixelFormat GetPixelFormat() const
	{
		return pixelFormat;
	}

	/**
	 * @brief Get pointer to RHI texture 2D
	 * @return Return pointer to RHI texture 2D. If RHI resource is not inited will return NULL
	 */
	FORCEINLINE Texture2DRHIRef_t GetTexture2DRHI() const
	{
		return texture;
	}

	/**
	 * @brief Get pointer to RHI surface
	 * @return Return pointer to RHI surface. If RHI resource is not inited will return NULL
	 */
	FORCEINLINE SurfaceRHIRef_t GetSurfaceRHI() const
	{
		return surface;
	}

	/**
	 * @brief Get texture create flags
	 * @return Return texture create flags (see ETextureCreateFlags)
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Is initialized render target RHI
	 * @return Return TRUE if render target RHI is allocated, else will return FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return texture.IsValid() && surface.IsValid();
	}

	/**
	 * @brief Is render target is dirty
	 * @return Return TRUE if render target is dirty, otherwise will return FALSE
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
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

	/**
	 * @brief If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing.
	 * This is only called by the rendering thread.
	 */
	virtual void UpdateRHI() override;

private:
	bool					bDirty;			/**< Is render target is dirty. In true case him will be updated in render thread */
	uint32					flags;			/**< Texture flags */
	uint32					sizeX;			/**< Size X */
	uint32					sizeY;			/**< Size Y */
	EPixelFormat			pixelFormat;	/**< Pixel format */
	Texture2DRHIRef_t		texture;		/**< Texture for resolving to */
	SurfaceRHIRef_t			surface;		/**< Surface to render to */

#if !SHIPPING_BUILD
	std::wstring			debugName;		/**< Debug resource name */
#endif // !SHIPPING_BUILD
};

#endif // !RENDERTARGET_H