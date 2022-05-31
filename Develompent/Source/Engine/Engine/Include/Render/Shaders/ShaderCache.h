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
#include <unordered_map>
#include <unordered_set>

#include "Containers/BulkData.h"
#include "System/Archive.h"
#include "RHI/BaseShaderRHI.h"

/**
 * @ingroup Engine
 * @brief A map of shader parameter names to registers allocated to that parameter
 */
class CShaderParameterMap
{
public:
	struct SParameterAllocation
	{
		/**
		 * @brief Construct a new SParameterAllocation object
		 */
		SParameterAllocation() :
			isBound( false )
		{}

		/**
		 * Overload operator << for serialize
		 */
		FORCEINLINE friend CArchive& operator<<( CArchive& InAr, SParameterAllocation& InParameterAllocation )
		{
			return InAr << InParameterAllocation.bufferIndex << InParameterAllocation.baseIndex << InParameterAllocation.size << InParameterAllocation.samplerIndex << InParameterAllocation.isBound;
		}

		/**
		 * Overload operator << for serialize
		 */
		FORCEINLINE friend CArchive& operator<<( CArchive& InAr, const SParameterAllocation& InParameterAllocation )
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
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, CShaderParameterMap& InShaderParameterMap )
	{
		return InAr << InShaderParameterMap.parameterMap;
	}

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, const CShaderParameterMap& InShaderParameterMap )
	{
		return InAr << InShaderParameterMap.parameterMap;
	}

private:
	std::unordered_map< std::wstring, SParameterAllocation >		parameterMap;		/**< Parameter map */
};

/**
 * @ingroup Engine 
 * @brief Class of serialize shader cache
 */
class CShaderCache
{
public:
	/**
	 * @brief Struct of shader cache item
	 */
	struct SShaderCacheItem
	{
		/**
		 * @brief Serialize
		 * @param[in] InArchive Archive
		 */
		void						Serialize( CArchive& InArchive );

		std::wstring				name;				/**< Name of class shader */
		EShaderFrequency			frequency;			/**< Frequency of shader */
		uint64						vertexFactoryHash;	/**< Vertex factory hash */
		ÑBulkData< byte >			code;				/**< Byte code of shader */
		uint32						numInstructions;	/**< Number instructions in shader */
		CShaderParameterMap			parameterMap;		/**< Parameter map */
	};

	/**
	 * @brief Serialize
	 * @param[in] InArchive Archive
	 */
	void													Serialize( CArchive& InArchive );

	/**
	 * @brief Add to cache compiled shader
	 * @param[in] InShaderCacheItem Shader cache item
	 */
	FORCEINLINE void										Add( const SShaderCacheItem& InShaderCacheItem )
	{
		items.push_back( InShaderCacheItem );
		itemsMap[ InShaderCacheItem.vertexFactoryHash ].insert( InShaderCacheItem.name );
	}

	/**
	 * @brief Get array of items shader cache
	 * @return Return const reference to array of items shader cache
	 */
	FORCEINLINE const std::vector< SShaderCacheItem >&		GetItems() const
	{
		return items;
	}

	/**
	 * @brief Is shader exist in cache
	 * 
	 * @param InShaderName Shader name
	 * @param InVertexFactoryHash Vertex factory hash
	 * @return Return true if shader exist in cache, else return false
	 */
	FORCEINLINE bool IsExist( const std::wstring& InShaderName, uint64 InVertexFactoryHash ) const
	{
		auto		itVFtype = itemsMap.find( InVertexFactoryHash );
		if ( itVFtype == itemsMap.end() )
		{
			return false;
		}

		auto		itShader = itVFtype->second.find( InShaderName );
		if ( itShader == itVFtype->second.end() )
		{
			return false;
		}

		return true;
	}

private:
	std::vector< SShaderCacheItem >											items;		/**< Array of items shader cache */
	std::unordered_map< uint64, std::unordered_set< std::wstring > >		itemsMap;	/**< Map of items separated by vertex factory. Need for check on exist in cache */
};

#endif // !SHADERCACHE_H
