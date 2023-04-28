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
 * @brief 2D texture mipmap
 */
struct STexture2DMipMap
{
	uint32				sizeX;	/**< Width of mipmap */
	uint32				sizeY;	/**< Height of mipmap */
	CBulkData<byte>		data;	/**< Data used when loading texture mipmap */
};

/**
 * @ingroup Engine
 * @brief Implementation for 2D textures
 */
class CTexture2D : public CAsset, public CRenderResource
{
public:
	/**
	 * Constructor
	 */
	CTexture2D();

	/**
	 * Destructor
	 */
	~CTexture2D();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

#if WITH_EDITOR
	/**
	 * Generate mipmaps
	 * @note Work only with editor
	 */
	void GenerateMipmaps();

	/**
	 * Get used memory size by the texture
	 * 
	 * @note Work only with editor
	 * @return Return used memory size in bytes
	 */
	uint64 GetMemorySize() const;
#endif // WITH_EDITOR

	/**
	 * Set texture data
	 * 
	 * @param InPixelFormat			Pixel format
	 * @param InSizeX				Width
	 * @param InSizeY				Height
	 * @param InData				Data
	 * @param InIsGenerateMipmaps	Is need generate mipmaps (only with editor)
	 */
	void SetData( EPixelFormat InPixelFormat, uint32 InSizeX, uint32 InSizeY, const std::vector<byte>& InData, bool InIsGenerateMipmaps = false );

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
	FORCEINLINE Texture2DRHIRef_t GetTexture2DRHI()
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
	 * @brief Get number of mipmaps in the texture
	 * @return Return number of mipmaps in the texture
	 */
	FORCEINLINE uint32 GetNumMips() const
	{
		return mipmaps.size();
	}

	/**
	 * Get texture size by X
	 * 
	 * @param InMipLevel	Mip level
	 * @return Return texture size by X
	 */
	FORCEINLINE uint32 GetSizeX( uint32 InMipLevel = 0 ) const
	{
		if ( InMipLevel >= mipmaps.size() )
		{
			return 0;
		}
		return mipmaps[InMipLevel].sizeX;
	}

	/**
	 * Get texture size by Y
	 * 
	 * @param InMipLevel	Mip level
	 * @return Return texture size by Y
	 */
	FORCEINLINE uint32 GetSizeY( uint32 InMipLevel = 0 ) const
	{
		if ( InMipLevel >= mipmaps.size() )
		{
			return 0;
		}
		return mipmaps[InMipLevel].sizeY;
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
	FORCEINLINE SSamplerStateInitializerRHI GetSamplerStateInitialiser() const
	{
		SSamplerStateInitializerRHI		samplerStateInitializer;
		appMemzero( &samplerStateInitializer, sizeof( SSamplerStateInitializerRHI ) );

		samplerStateInitializer.filter = samplerFilter;
		samplerStateInitializer.addressU = addressU;
		samplerStateInitializer.addressV = addressV;
		return samplerStateInitializer;
	}

	/**
	 * Get mip data on level
	 * 
	 * @param InMipLevel		Mip level
	 * @return Return refrence to mip data
	 */
	FORCEINLINE const STexture2DMipMap& GetMip( uint32 InMipLevel ) const
	{
		check( InMipLevel < mipmaps.size() );
		return mipmaps[InMipLevel];
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
	EPixelFormat						pixelFormat;		/**< Pixel format of texture */
	Texture2DRHIRef_t					texture;			/**< Reference to RHI texture */
	ESamplerAddressMode					addressU;			/**< Address mode for U coord */
	ESamplerAddressMode					addressV;			/**< Address mode for V coord */
	ESamplerFilter						samplerFilter;		/**< Sampler filter */
	std::vector<STexture2DMipMap>		mipmaps;			/**< Array of mipmaps */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CTexture2D>& InValue )
{
	TAssetHandle<CAsset>	asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CTexture2D>& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, STexture2DMipMap& InValue )
{
	InArchive << InValue.sizeX;
	InArchive << InValue.sizeY;
	InArchive << InValue.data;
	return InArchive;
}

//FORCEINLINE CArchive& operator<<( CArchive& InArchive, const STexture2DMipMap& InValue )
//{
//	check( InArchive.IsSaving() );
//	InArchive << InValue.sizeX;
//	InArchive << InValue.sizeY;
//	InArchive << InValue.data;
//	return InArchive;
//}

#endif // !TEXTURE_H