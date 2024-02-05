/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/System/Archive.h"
#include "Engine/Render/Shaders/ShaderCache.h"

#define SHADER_CACHE_VERSION			4

/*
==================
CShaderParameterMap::FindParameterAllocation
==================
*/
bool CShaderParameterMap::FindParameterAllocation( const tchar* InParameterName, uint32& OutBufferIndex, uint32& OutBaseIndex, uint32& OutSize, uint32& OutSamplerIndex ) const
{
	auto		itParamAllocation = parameterMap.find( InParameterName );
	if ( itParamAllocation != parameterMap.end() )
	{
		const ParameterAllocation&		allocation = itParamAllocation->second;
		OutBufferIndex = allocation.bufferIndex;
		OutBaseIndex = allocation.baseIndex;
		OutSize = allocation.size;
		OutSamplerIndex = allocation.samplerIndex;
		allocation.isBound = true;
		return true;
	}
	else
	{
		return false;
	}
}

/*
==================
CShaderParameterMap::AddParameterAllocation
==================
*/
void CShaderParameterMap::AddParameterAllocation( const tchar* InParameterName, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InSize, uint32 InSamplerIndex )
{
	ParameterAllocation 		allocation;
	allocation.bufferIndex = InBufferIndex;
	allocation.baseIndex = InBaseIndex;
	allocation.size = InSize;
	allocation.samplerIndex = InSamplerIndex;
	parameterMap[ InParameterName ] = allocation;
}

/*
==================
CShaderCache::ShaderCacheItem::Serialize
==================
*/
void CShaderCache::ShaderCacheItem::Serialize( CArchive& InArchive )
{
	InArchive << frequency;

	if ( InArchive.Ver() < VER_HashUInt64 )
	{
		uint32		tmpVertexFactoryHash = vertexFactoryHash;
		InArchive << tmpVertexFactoryHash;
		vertexFactoryHash = tmpVertexFactoryHash;
	}
	else
	{
		InArchive << vertexFactoryHash;
	}

	InArchive << numInstructions;
	InArchive << name;

	if ( InArchive.Ver() >= VER_ShaderParameterMap )
	{
		InArchive << parameterMap;
	}

	if ( InArchive.Ver() < VER_CompressedZlib )
	{
		std::vector<byte>		tmpCode;
		InArchive << tmpCode;
		code = tmpCode;
	}
	else
	{
		InArchive << code;
	}
}

/*
==================
CShaderCache::Serialize
==================
*/
void CShaderCache::Serialize( CArchive& InArchive )
{
	Assert( InArchive.Type() == AT_ShaderCache );

	if ( InArchive.IsLoading() )
	{
		// Check version of shader cache
		uint32			shaderCacheVersion = 0;
		InArchive << shaderCacheVersion;
		if ( shaderCacheVersion != SHADER_CACHE_VERSION )
		{
			Sys_Errorf( TEXT( "Not supported version of shader cache. In archive version %i, need %i" ), shaderCacheVersion, SHADER_CACHE_VERSION );
			return;
		}

		uint32			countItems = 0;
		InArchive << countItems;
		items.resize( countItems );

		// Loading all items from archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			ShaderCacheItem&		item = items[ indexItem ];
			item.Serialize( InArchive );
			itemsMap[ item.vertexFactoryHash ].insert( item.name );
		}
	}
	else if ( InArchive.IsSaving() )
	{
		uint32		countItems = ( uint32 )items.size();
		InArchive << SHADER_CACHE_VERSION;
		InArchive << countItems;

		// Save all items to archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			items[ indexItem ].Serialize( InArchive );
		}
	}
}