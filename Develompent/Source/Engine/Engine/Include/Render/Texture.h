/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

#include "RenderResource.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Implementation for 2D textures
 */
class FTexture2D : public FRenderResource
{
public:
	/**
	 * Constructor
	 */
	FTexture2D();

	/**
	 * Destructor
	 */
	~FTexture2D();

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
	 * Set data texture
	 * 
	 * @param[in] InTextureCache Texture cache
	 */
	void SetData( const struct FTextureCacheItem& InTextureCache );

	/**
	 * Get RHI texture 2D
	 * @return Return pointer to RHI texture 2D
	 */
	FORCEINLINE FTexture2DRHIRef GetTexture2DRHI()
	{
		return texture;
	}

	/**
	 * Get RHI sampler state
	 * @return Return pointer to THI sampler state
	 */
	FORCEINLINE FSamplerStateRHIRef GetSamplerStateRHI()
	{
		return samplerState;
	}

	/**
	 * Get pixel format
	 * @return Return pixel format in texture
	 */
	FORCEINLINE EPixelFormat GetPixelFormat() const
	{
		return pixelFormat;
	}

private:
	uint32						sizeX;			/**< Width of texture */
	uint32						sizeY;			/**< Height of texture */
	std::vector< byte >			data;			/**< Data used when loading texture */
	EPixelFormat				pixelFormat;	/**< Pixel format of texture */
	FTexture2DRHIRef			texture;		/**< Reference to RHI texture */
	FSamplerStateRHIRef			samplerState;	/**< Reference to sampler state */
};

#endif // !TEXTURE_H