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
		if ( addressU != InAdressU )
		{
			MarkDirty();
		}
		addressU = InAdressU;
	}

	/**
	 * Set address mod for V coord
	 *
	 * @param[in] InAdressV Address mode for V coord
	 */
	FORCEINLINE void SetAddressV( ESamplerAddressMode InAdressV )
	{
		if ( addressV != InAdressV )
		{
			MarkDirty();
		}
		addressV = InAdressV;
	}

	/**
	 * Set sampler filter
	 *
	 * @param InSamplerFilter		Sampler filter
	 */
	FORCEINLINE void SetSamplerFilter( ESamplerFilter InSamplerFilter )
	{
		if ( samplerFilter != InSamplerFilter )
		{
			MarkDirty();
		}
		samplerFilter = InSamplerFilter;
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
	 * Get texture size by X
	 * @return Return texture size by X
	 */
	FORCEINLINE uint32 GetSizeX() const
	{
		return sizeX;
	}

	/**
	 * Get texture size by Y
	 * @return Return texture size by Y
	 */
	FORCEINLINE uint32 GetSizeY() const
	{
		return sizeY;
	}

	/**
	 * Get address mod for U coord
	 * @return Return address mode for U coord
	 */
	FORCEINLINE ESamplerAddressMode GetAddressU() const
	{
		return addressU;
	}

	/**
	 * Get address mod for V coord
	 * @return Return address mode for V coord
	 */
	FORCEINLINE ESamplerAddressMode GetAddressV() const
	{
		return addressV;
	}

	/**
	 * Get sampler filter
	 * @return Return sampler filter
	 */
	FORCEINLINE ESamplerFilter GetSamplerFilter() const
	{
		return samplerFilter;
	}

	/**
	 * Get sampler state initializer for RHI
	 * @return Return sampler state initializer
	 */
	FORCEINLINE FSamplerStateInitializerRHI GetSamplerStateInitialiser() const
	{
		FSamplerStateInitializerRHI		samplerStateInitializer;
		appMemzero( &samplerStateInitializer, sizeof( FSamplerStateInitializerRHI ) );

		samplerStateInitializer.filter = samplerFilter;
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
	ESamplerFilter				samplerFilter;		/**< Sampler filter */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, TAssetHandle<FTexture2D>& InValue )
{
	TAssetHandle<FAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const TAssetHandle<FTexture2D>& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<FAsset> )InValue;
	return InArchive;
}

#endif // !TEXTURE_H