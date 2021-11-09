#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Misc/CoreGlobals.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Render/Texture.h"
#include "Render/RenderUtils.h"
#include "Render/TextureFileCache.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Render/TextureFileCache.h"

FTexture2D::FTexture2D() :
	FAsset( AT_Texture2D ),
	textureCacheHash( ( uint32 )INVALID_HASH ),
	sizeX( 0 ),
	sizeY( 0 ),
	pixelFormat( PF_Unknown ),
	addressU( SAM_Wrap ),
	addressV( SAM_Wrap )
{}

FTexture2D::~FTexture2D()
{}

void FTexture2D::InitRHI()
{
	check( !data.empty() );
	texture = GRHI->CreateTexture2D( FString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeX, sizeY, pixelFormat, 1, 0, data.data() );
	data.clear();
}

void FTexture2D::ReleaseRHI()
{
	texture = nullptr;
}

void FTexture2D::SetTextureCache( const struct FTextureCacheItem& InTextureCache, const std::wstring& InTextureCachePath )
{
	check( InTextureCache.hash != ( uint32 )INVALID_HASH );
	
	// Copy new parameters of texture
	textureCachePath		= InTextureCachePath;
	textureCacheHash		= InTextureCache.hash;
	sizeX					= InTextureCache.sizeX;
	sizeY					= InTextureCache.sizeY;
	pixelFormat				= InTextureCache.pixelFormat;
	data					= InTextureCache.data;

	BeginUpdateResource( this );
}

void FTexture2D::Serialize( class FArchive& InArchive )
{
	FAsset::Serialize( InArchive );
	InArchive << textureCachePath;
	InArchive << textureCacheHash;
	InArchive << sizeX;
	InArchive << sizeY;
	InArchive << pixelFormat;
	InArchive << addressU;
	InArchive << addressV;

	// If we loading Texture2D - load texture cache
	if ( InArchive.IsLoading() )
	{
		FArchive*		archive = GFileSystem->CreateFileReader( appBaseDir() + textureCachePath );
		if ( !archive )
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "Texture cache '%s' not found" ), textureCachePath.c_str() );
			return;
		}

		FTextureFileCache		textureFileCache;
		archive->SerializeHeader();
		textureFileCache.Serialize( *archive );

		FTextureCacheItem		textureCacheItem;
		if ( textureFileCache.Find( textureCacheHash, &textureCacheItem ) )
		{
			data = textureCacheItem.data;
			BeginUpdateResource( this );
		}
		else
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "In Texture cache '%s' not found item with hash 0x%X" ), textureCachePath.c_str(), textureCacheHash );
		}

		delete archive;
	}
}