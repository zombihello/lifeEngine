/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREFILECACHE_H
#define TEXTUREFILECACHE_H

#include <string>
#include <vector>

#include "System/BaseArchive.h"
#include "RHI/BaseSurfaceRHI.h"

/**
 * @ingroup Engine
 * Struct of texture cache item
 */
struct FTextureCacheItem
{
	friend class FTextureFileCache;

	/**
	 * Invalid hash of texture cache item
	 */
	enum { INVALID_HASH = ( uint32 )-1 };

	/**
	 * Constructor
	 */
	FTextureCacheItem();

	/**
	 * @brief Serialize
	 * @param[in] InArchive Archive
	 */
	void Serialize( FBaseArchive& InArchive );

	/**
	 * Calculate hash from name
	 *
	 * @param[in] InName Name
	 */
	FORCEINLINE static uint32 CalcHash( const std::wstring& InName )
	{
		return appMemFastHash( InName.data(), ( uint32 )InName.size() * sizeof( std::wstring::value_type ), 0 );		// TODO BG yehor.pohuliaka - Need change to one format without dependency from platform
	}

	uint32						hash;			/**< Hash of texture */
	EPixelFormat				pixelFormat;	/**< Pixel format */
	uint32						sizeX;			/**< Width */
	uint32						sizeY;			/**< Height */
	std::vector< byte >			data;			/**< Data */
};

/**
 * @ingroup Engine
 * Class for work with file of texture cache
 */
class FTextureFileCache
{
public:
	/**
	 * @brief Serialize
	 * @param[in] InArchive Archive
	 */
	void Serialize( FBaseArchive& InArchive );

	/**
	 * @brief Add to cache texture data
	 * @param[in] InTextureCacheItem Texture cache item
	 */
	FORCEINLINE void Add( const FTextureCacheItem& InTextureCacheItem )
	{
		items.push_back( InTextureCacheItem );
	}

	/**
	 * Find texture data from cache
	 * 
	 * @param[in] InTextureHash Texture hash
	 * @param[out] OutTextureCacheItem Texture cache for return
	 * @return Return true if texture cache is finded, else return false
	 */
	bool Find( uint32 InTextureHash, FTextureCacheItem* OutTextureCacheItem ) const;

	/**
	 * Remove texture data from cache
	 * 
	 * @param[in] InTextureHash Texture hash
	 */
	void Remove( uint32 InTextureHash );

	/**
	 * @brief Get array of items texture cache
	 * @return Return const reference to array of items texture cache
	 */
	FORCEINLINE const std::vector< FTextureCacheItem >& GetItems() const
	{
		return items;
	}

private:
	std::vector< FTextureCacheItem >			items;			/**< Array of items texture cache */
};

#endif // !TEXTUREFILECACHE_H