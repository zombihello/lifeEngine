/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHADERCACHE_H
#define SHADERCACHE_H

#include <string>
#include <vector>

#include "System/Archive.h"
#include "RHI/BaseShaderRHI.h"

/**
 * @ingroup Engine 
 * @brief Class of serialize shader cache
 */
class FShaderCache
{
public:
	/**
	 * @brief Struct of shader cache item
	 */
	struct FShaderCacheItem
	{
		/**
		 * @brief Serialize
		 * @param[in] InArchive Archive
		 */
		void						Serialize( FArchive& InArchive );

		std::wstring				name;				/**< Name of class shader */
		EShaderFrequency			frequency;			/**< Frequency of shader */
		std::vector< byte >			code;				/**< Byte code of shader */
		uint32						numInstructions;	/**< Number instructions in shader */
	};

	/**
	 * @brief Serialize
	 * @param[in] InArchive Archive
	 */
	void													Serialize( FArchive& InArchive );

	/**
	 * @brief Add to cache compiled shader
	 * @param[in] InShaderCacheItem Shader cache item
	 */
	FORCEINLINE void										Add( const FShaderCacheItem& InShaderCacheItem )
	{
		items.push_back( InShaderCacheItem );
	}

	/**
	 * @brief Get array of items shader cache
	 * @return Return const reference to array of items shader cache
	 */
	FORCEINLINE const std::vector< FShaderCacheItem >&		GetItems() const
	{
		return items;
	}

private:
	std::vector< FShaderCacheItem >			items;			/**< Array of items shader cache */
};

#endif // !SHADERCACHE_H
