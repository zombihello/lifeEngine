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
	 * Constructor
	 * 
	 * @param[in] InFilename Path to file with texture
	 */
	FTexture2D( const std::wstring& InFilename );

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
	 * Set path to file with texture
	 * 
	 * @param[in] InFilename Path to file with texture
	 */
	void SetFilename( const std::wstring& InFilename );

	/**
	 * Get path to file with texture
	 * @return Return path to file with texture
	 */
	FORCEINLINE const std::wstring& GetFilename() const
	{
		return filename;
	}

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

private:
	std::wstring				filename;		/**< Path to file with texture */
	uint32						sizeX;			/**< Width of texture */
	uint32						sizeY;			/**< Height of texture */
	byte*						data;			/**< Pointer to data used when loading texture */
	FTexture2DRHIRef			texture;		/**< Reference to RHI texture */
	FSamplerStateRHIRef			samplerState;	/**< Reference to sampler state */
};

#endif // !TEXTURE_H