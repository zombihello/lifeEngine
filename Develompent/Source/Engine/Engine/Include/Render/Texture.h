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
#include "Containers/BulkData.h"
#include "System/Package.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to FTexture2D
 */
typedef TRefCountPtr< class FTexture2D >				FTexture2DRef;

/**
 * @ingroup Engine
 * @brief Implementation for 2D textures
 */
class FTexture2D : public FAsset, public FRenderResource
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
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Set texture data
	 * 
	 * @param[in] InPixelFormat Pixel format
	 * @param[in] InSizeX Width
	 * @param[in] InSizeY Height
	 * @param[in] InData Data
	 */
	void SetData( EPixelFormat InPixelFormat, uint32 InSizeX, uint32 InSizeY, const std::vector< byte >& InData );

	/**
	 * Set address mod for U coord
	 * 
	 * @param[in] InAdressU Address mode for U coord
	 */
	FORCEINLINE void SetAddressU( ESamplerAddressMode InAdressU )
	{
		addressU = InAdressU;
	}

	/**
	 * Set address mod for V coord
	 *
	 * @param[in] InAdressV Address mode for V coord
	 */
	FORCEINLINE void SetAddressV( ESamplerAddressMode InAdressV )
	{
		addressV = InAdressV;
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
	 * Get pixel format
	 * @return Return pixel format in texture
	 */
	FORCEINLINE EPixelFormat GetPixelFormat() const
	{
		return pixelFormat;
	}

	/**
	 * Get sampler state initializer for RHI
	 * @return Return sampler state initializer
	 */
	FORCEINLINE FSamplerStateInitializerRHI GetSamplerStateInitialiser() const
	{
		FSamplerStateInitializerRHI		samplerStateInitializer;
		appMemzero( &samplerStateInitializer, sizeof( FSamplerStateInitializerRHI ) );

		samplerStateInitializer.filter = SF_Bilinear;
		samplerStateInitializer.addressU = addressU;
		samplerStateInitializer.addressV = addressV;
		return samplerStateInitializer;
	}

private:
	uint32						sizeX;				/**< Width of texture */
	uint32						sizeY;				/**< Height of texture */
	FBulkData<byte>				data;				/**< Data used when loading texture */
	EPixelFormat				pixelFormat;		/**< Pixel format of texture */
	FTexture2DRHIRef			texture;			/**< Reference to RHI texture */
	ESamplerAddressMode			addressU;			/**< Address mode for U coord */
	ESamplerAddressMode			addressV;			/**< Address mode for V coord */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FTexture2DRef& InValue )
{
	FAssetRef			asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FTexture2DRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( FAssetRef )InValue;
	return InArchive;
}

#endif // !TEXTURE_H