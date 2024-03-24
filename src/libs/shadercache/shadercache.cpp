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

#include "interfaces/interfaces.h"
#include "core/debug.h"
#include "filesystem/ifilesystem.h"
#include "libs/shadercache/shadercache.h"

// Studio shader cache version log
enum StudioShaderCacheVersion
{
	SSC_VER_BASE = 1,						/**< Min version for save/load file */

	//
	// New versions can be added here
	//

	SSC_VER_AUTOMATIC_VERSION_PLUSONE,		/**< This needs for automatic update last version of package */
};

// Studio shader cache tag
#define SSC_TAG					( ('C'<<24) + ('S'<<16) + ('S'<<8) + 'L' )

// Latest studio shader cache version
#define SSC_VERSION_LATEST		( SSC_VER_AUTOMATIC_VERSION_PLUSONE-1 )


/*
==================
CShaderParameterMap::FindParameterAllocation
==================
*/
bool CShaderParameterMap::FindParameterAllocation( const achar* pParameterName, uint32& bufferIndex, uint32& baseIndex, uint32& size, uint32& samplerIndex ) const
{
	auto		itParamAllocation = parameterMap.find( pParameterName );
	if ( itParamAllocation != parameterMap.end() )
	{
		const ParameterAllocation&		allocation = itParamAllocation->second;
		bufferIndex			= allocation.bufferIndex;
		baseIndex			= allocation.baseIndex;
		size				= allocation.size;
		samplerIndex		= allocation.samplerIndex;
		allocation.bBound	= true;
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
void CShaderParameterMap::AddParameterAllocation( const achar* pParameterName, uint32 bufferIndex, uint32 baseIndex, uint32 size, uint32 samplerIndex )
{
	ParameterAllocation 			allocation;
	allocation.bufferIndex			= bufferIndex;
	allocation.baseIndex			= baseIndex;
	allocation.size					= size;
	allocation.samplerIndex			= samplerIndex;
	parameterMap[pParameterName]	= allocation;
}

/*
==================
CShaderParameterMap::Write
==================
*/
void CShaderParameterMap::Write( IFileWriter* pFile ) const
{
	// Write parameter map size
	const uint32	numParameters = ( uint32 )parameterMap.size();
	pFile->Write( ( void* )&numParameters, sizeof( uint32 ) );

	// Write each parameter allocation
	for ( auto it = parameterMap.begin(), itEnd = parameterMap.end(); it != itEnd; ++it )
	{
		// Write parameter name
		const uint32	sizeName = ( uint32 )it->first.size();
		pFile->Write( ( void* )&sizeName, sizeof( uint32 ) );
		pFile->Write( ( void* )it->first.data(), sizeName * sizeof( achar ) );

		// Write parameter
		pFile->Write( ( void* )&it->second, sizeof( ParameterAllocation ) );
	}
}

/*
==================
CShaderParameterMap::Read
==================
*/
void CShaderParameterMap::Read( IFileReader* pFile )
{}


/*
==================
CShaderCacheDoc::Write
==================
*/
void ShaderCacheElement::Write( IFileWriter* pFile ) const
{
	// Write number of instructions and parameter map
	pFile->Write( ( void* )&numInstructions, sizeof( uint32 ) );
	parameterMap.Write( pFile );

	// Write bytecode
	const uint64	sizeBytecode = ( uint64 )bytecode.size();
	pFile->Write( ( void* )&sizeBytecode, sizeof( uint64 ) );
	pFile->WriteCompressed( ( void* )bytecode.data(), sizeBytecode, COMPRESSION_ZLIB );
}

/*
==================
CShaderCacheDoc::Read
==================
*/
void ShaderCacheElement::Read( IFileReader* pFile )
{}


/*
==================
CShaderCacheDoc::CShaderCacheDoc
==================
*/
CShaderCacheDoc::CShaderCacheDoc()
	: bUseVertexFactory( false )
{}

/*
==================
CShaderCacheDoc::Save
==================
*/
bool CShaderCacheDoc::Save( const achar* pPath )
{
	// Create a file
	Assert( g_pFileSystem );
	TRefPtr<IFileWriter>		pFile = g_pFileSystem->CreateFileWriter( pPath );
	if ( !pFile )
	{
		Warning( "CShaderCacheDoc::Save: Failed to open file '%s' for save a shader cache", pPath );
		return false;
	}

	// Write format tag and version
	const uint32	formatTag = SSC_TAG;
	const uint16	formatVersion = SSC_VERSION_LATEST;
	pFile->Write( ( void* )&formatTag, sizeof( uint32 ) );
	pFile->Write( ( void* )&formatVersion, sizeof( uint16 ) );

	// Write shader type
	pFile->Write( &shaderType, sizeof( uint16 ) );

	// Save place for number of vertex factories
	uint64	numVertexFactoriesPos	= pFile->Tell();
	uint32	numVertexFactories		= 0;
	pFile->Write( &numVertexFactories, sizeof( uint32 ) );

	// Write shader caches for each vertex factory type
	for ( uint32 vfIdx = 0; vfIdx < VERTEXFACTORY_NUM_TYPES; ++vfIdx )
	{
		const std::vector<ShaderCacheElement>&		shaderCaches = elements[vfIdx];
		if ( !shaderCaches.empty() )
		{
			// Write vertex factory type
			EVertexFactoryType		vfType = IsUseVertexFactory() ? ( EVertexFactoryType )vfIdx : ( EVertexFactoryType )-1;
			pFile->Write( &vfType, sizeof( uint16 ) );

			// Write shader caches
			uint32					numShaderCaches = ( uint32 )shaderCaches.size();
			pFile->Write( &numShaderCaches, sizeof( uint32 ) );
			for ( uint32 cacheIdx = 0; cacheIdx < numShaderCaches; ++cacheIdx )
			{
				const ShaderCacheElement&		shaderCache = shaderCaches[cacheIdx];
				shaderCache.Write( pFile );
			}

			++numVertexFactories;
		}
	}

	// Update number of vertex factories
	if ( !IsUseVertexFactory() )
	{
		numVertexFactories = ( uint32 )-1;
	}

	pFile->Seek( numVertexFactoriesPos );
	pFile->Write( &numVertexFactories, sizeof( uint32 ) );
	return true;
}

/*
==================
CShaderCacheDoc::Load
==================
*/
bool CShaderCacheDoc::Load( const achar* pPath )
{
	return false;
}