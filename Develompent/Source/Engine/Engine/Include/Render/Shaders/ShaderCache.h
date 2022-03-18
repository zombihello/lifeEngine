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

#include "Containers/BulkData.h"
#include "System/Archive.h"
#include "RHI/BaseShaderRHI.h"

/**
 * @ingroup Engine
 * @brief A map of shader parameter names to registers allocated to that parameter
 */
class FShaderParameterMap
{
public:
	struct FParameterAllocation
	{
		/**
		 * @brief Construct a new FParameterAllocation object
		 */
		FParameterAllocation() :
			isBound( false )
		{}

		/**
		 * Overload operator << for serialize
		 */
		FORCEINLINE friend FArchive& operator<<( FArchive& InAr, FParameterAllocation& InParameterAllocation )
		{
			return InAr << InParameterAllocation.bufferIndex << InParameterAllocation.baseIndex << InParameterAllocation.size << InParameterAllocation.samplerIndex << InParameterAllocation.isBound;
		}

		/**
		 * Overload operator << for serialize
		 */
		FORCEINLINE friend FArchive& operator<<( FArchive& InAr, const FParameterAllocation& InParameterAllocation )
		{
			return InAr << InParameterAllocation.bufferIndex << InParameterAllocation.baseIndex << InParameterAllocation.size << InParameterAllocation.samplerIndex << InParameterAllocation.isBound;
		}

		uint32			bufferIndex;		/**< Buffer index */
		uint32			baseIndex;			/**< Base index */
		uint32			size;				/**< Size parameter */
		uint32			samplerIndex;		/**< Sampler index */
		mutable bool 	isBound;			/**< Is bound */
	};

	/**
	 * @brief Find parameter allocation
	 * 
	 * @param InParameterName Parameter name
	 * @param OutBufferIndex Output buffer index
	 * @param OutBaseIndex Output base index
	 * @param OutSize Output size
	 * @param OutSamplerIndex Output sampler index
	 * @return Return true if parameter is finded, else return false
	 */
	bool FindParameterAllocation( const tchar* InParameterName, uint32& OutBufferIndex, uint32& OutBaseIndex, uint32& OutSize, uint32& OutSamplerIndex ) const;

	/**
	 * @brief Add parameter allocation
	 * 
	 * @param InParameterName Parameter name
	 * @param InBufferIndex Buffer index
	 * @param InBaseIndex Base index
	 * @param InSize Size
	 * @param InSamplerIndex Sampler index
	 */
	void AddParameterAllocation( const tchar* InParameterName, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InSize, uint32 InSamplerIndex );

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InAr, FShaderParameterMap& InShaderParameterMap )
	{
		return InAr << InShaderParameterMap.parameterMap;
	}

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InAr, const FShaderParameterMap& InShaderParameterMap )
	{
		return InAr << InShaderParameterMap.parameterMap;
	}

private:
	std::unordered_map< std::wstring, FParameterAllocation >		parameterMap;		/**< Parameter map */
};

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
		uint32						vertexFactoryHash;	/**< Vertex factory hash */
		FBulkData< byte >			code;				/**< Byte code of shader */
		uint32						numInstructions;	/**< Number instructions in shader */
		FShaderParameterMap			parameterMap;		/**< Parameter map */
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
