#include "Render/Shaders/ShaderCache.h"

#define SHADER_CACHE_VERSION			1

/**
 * Serialize of FShaderCacheItem
 */
void FShaderCache::FShaderCacheItem::Serialize( FBaseArchive& InArchive )
{
	uint32		nameSize = 0;
	uint32		codeSize = 0;

	InArchive << frequency;
	InArchive << numInstructions;

	if ( InArchive.IsLoading() )
	{
		// Load shader class name
		InArchive << nameSize;
		name.resize( nameSize / sizeof( std::wstring::value_type ) );
		InArchive.Serialize( ( void* ) name.data(), nameSize );

		// Load byte code of shader
		InArchive << codeSize;
		code.resize( codeSize );
		InArchive.Serialize( ( void* ) code.data(), codeSize );
	}
	else if ( InArchive.IsSaving() )
	{
		nameSize = ( uint32 )name.size() * sizeof( std::wstring::value_type );		// TODO BG yehor.pohuliaka - Need change to one format without dependency from platform
		codeSize = ( uint32 )code.size();

		// Save shader class name
		InArchive << nameSize;
		InArchive.Serialize( ( void* )name.data(), nameSize );

		// Save byte code of shader
		InArchive << codeSize;
		InArchive.Serialize( ( void* )code.data(), codeSize );
	}
}

/**
 * Serialize
 */
void FShaderCache::Serialize( FBaseArchive& InArchive )
{
	if ( InArchive.IsLoading() )
	{
		EArchiveResourceType		resourceType;
		InArchive << resourceType;

		// If resource type not shader cache - this is error
		if ( resourceType != ART_ShaderCache )
		{
			appErrorf( TEXT( "The current section in the archive is not a shader cache" ) );
			return;
		}

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

		InArchive << ART_ShaderCache;
		InArchive << SHADER_CACHE_VERSION;
		InArchive << countItems;

		// Save all items to archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			items[ indexItem ].Serialize( InArchive );
		}
	}
}