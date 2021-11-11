#include "Render/Shaders/ShaderCache.h"

#define SHADER_CACHE_VERSION			3

/**
 * Serialize of FShaderCacheItem
 */
void FShaderCache::FShaderCacheItem::Serialize( FArchive& InArchive )
{
	InArchive << frequency;
	InArchive << vertexFactoryHash;
	InArchive << numInstructions;
	InArchive << name;

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

/**
 * Serialize
 */
void FShaderCache::Serialize( FArchive& InArchive )
{
	check( InArchive.Type() == AT_ShaderCache );

	if ( InArchive.IsLoading() )
	{
		// Check version of shader cache
		uint32			shaderCacheVersion = 0;
		InArchive << shaderCacheVersion;
		if ( shaderCacheVersion != SHADER_CACHE_VERSION )
		{
			appErrorf( TEXT( "Not supported version of shader cache. In archive version %i, need %i" ), shaderCacheVersion, SHADER_CACHE_VERSION );
			return;
		}

		uint32			countItems = 0;
		InArchive << countItems;
		items.resize( countItems );

		// Loading all items from archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			FShaderCacheItem&		item = items[ indexItem ];
			item.Serialize( InArchive );
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