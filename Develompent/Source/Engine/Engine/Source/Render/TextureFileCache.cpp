#include "Render/TextureFileCache.h"

#define TEXTURE_CACHE_VERSION			1

FTextureCacheItem::FTextureCacheItem() :
	pixelFormat( PF_Unknown ),
	sizeX( 0 ),
	sizeY( 0 ),
	hash( ( uint32 )INVALID_HASH )
{}

void FTextureCacheItem::Serialize( FBaseArchive& InArchive )
{
	InArchive << hash;
	InArchive << pixelFormat;
	InArchive << sizeX;
	InArchive << sizeY;

	if ( InArchive.IsLoading() )
	{
		uint32			dataSize = 0;
		InArchive << dataSize;
		data.resize( dataSize, 0 );

		InArchive.Serialize( data.data(), dataSize );
	}
	else if ( InArchive.IsSaving() )
	{
		uint32			dataSize = ( uint32 )data.size();
		InArchive << dataSize;
		InArchive.Serialize( data.data(), dataSize );
	}
}

void FTextureFileCache::Serialize( FBaseArchive& InArchive )
{
	if ( InArchive.IsLoading() )
	{
		EArchiveResourceType		resourceType;
		InArchive << resourceType;

		// If resource type not texture cache - this is error
		if ( resourceType != ART_TextureCahce )
		{
			appErrorf( TEXT( "The current section in the archive is not a texture cache" ) );
			return;
		}

		// Check version of texture cache
		uint32			textureCacheVersion = 0;
		InArchive << textureCacheVersion;
		if ( textureCacheVersion != TEXTURE_CACHE_VERSION )
		{
			appErrorf( TEXT( "Not supported version of texture cache. In archive version %i, need %i" ), textureCacheVersion, TEXTURE_CACHE_VERSION );
			return;
		}

		uint32			countItems = 0;
		InArchive << countItems;
		items.resize( countItems );

		// Loading all items from archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			FTextureCacheItem&		item = items[ indexItem ];
			item.Serialize( InArchive );
		}
	}
	else if ( InArchive.IsSaving() )
	{
		uint32		countItems = ( uint32 )items.size();

		InArchive << ART_TextureCahce;
		InArchive << TEXTURE_CACHE_VERSION;
		InArchive << countItems;

		// Save all items to archive
		for ( uint32 indexItem = 0; indexItem < countItems; ++indexItem )
		{
			FTextureCacheItem&			item = items[ indexItem ];
			item.Serialize( InArchive );
		}
	}
}

bool FTextureFileCache::Find( uint32 InTextureHash, FTextureCacheItem* OutTextureCacheItem ) const
{
	check( OutTextureCacheItem );
	for ( uint32 index = 0, count = ( uint32 )items.size(); index < count; ++index )
	{
		const FTextureCacheItem&		item = items[ index ];
		if ( item.hash == InTextureHash )
		{
			*OutTextureCacheItem = item;
			return true;
		}
	}

	return false;
}

void FTextureFileCache::Remove( uint32 InTextureHash )
{
	for ( uint32 index = 0, count = ( uint32 )items.size(); index < count; ++index )
	{
		const FTextureCacheItem&		item = items[ index ];
		if ( item.hash == InTextureHash )
		{
			items.erase( items.begin() + index );
			return;
		}
	}
}