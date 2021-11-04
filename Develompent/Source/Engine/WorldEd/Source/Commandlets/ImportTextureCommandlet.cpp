#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Render/TextureFileCache.h"
#include "Render/RenderUtils.h"
#include "Commandlets/ImportTextureCommandlet.h"

IMPLEMENT_CLASS( LImportTextureCommandlet )

void LImportTextureCommandlet::Main( const std::wstring& InCommand )
{
	std::wstring			srcFilename;
	std::wstring			dstFilename;
	std::wstring			nameTexture;
	bool					isAppend = false;

	// Parse arguments
	{
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCommand.c_str(), tokens, switches );

		for ( uint32 index = 0, count = ( uint32 )switches.size(); index < count; ++index )
		{
			const std::wstring&		param = switches[ index ];
			const std::wstring&		token = tokens[ index + 2 ];

			// Source filename
			if ( param == TEXT( "src" ) || param == TEXT( "source" ) )
			{
				srcFilename = token;
			}

			// Destination filename
			else if ( param == TEXT( "dst" ) || param == TEXT( "destination" ) )
			{
				dstFilename = token;
			}

			// Name texture
			else if ( param == TEXT( "n" ) || param == TEXT( "name" ) )
			{
				nameTexture = token;
			}

			// Is need append to already created cache
			else if ( param == TEXT( "a" ) || param == TEXT( "append" ) )
			{
				isAppend = true;
			}
		}
	}

	// If source and destination files is empty - this error
	checkMsg( !srcFilename.empty() || !dstFilename.empty(), TEXT( "Not entered source file name and destination file name" ) );

	// If name texture not seted - use name from srcFilename
	if ( nameTexture.empty() )
	{
		nameTexture = srcFilename;
	}

	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;
	void*			data = stbi_load( TCHAR_TO_ANSI( srcFilename.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	check( data );

	FTextureCacheItem				textureCacheItem;
	textureCacheItem.hash			= appCalcHash( nameTexture );
	textureCacheItem.pixelFormat	= PF_A8R8G8B8;
	textureCacheItem.sizeX			= sizeX;
	textureCacheItem.sizeY			= sizeY;
	textureCacheItem.data.resize( sizeX * sizeY * GPixelFormats[ PF_A8R8G8B8 ].blockBytes );
	memcpy( textureCacheItem.data.data(), data, textureCacheItem.data.size() );

	// Serialize texture cache if his already created
	FTextureFileCache		textureCache;
	{
		FArchive*		archive = GFileSystem->CreateFileReader( dstFilename.c_str() );
		if ( archive )
		{
			archive->SerializeHeader();
			if ( archive->Type() == AT_TextureCache )
			{
				textureCache.Serialize( *archive );
			}

			delete archive;
		}
	}

	// Add and save update texture cache
	textureCache.Add( textureCacheItem );

	FArchive*			archive = GFileSystem->CreateFileWriter( dstFilename.c_str(), AW_NoFail | ( isAppend ? AW_Append : AW_None ) );
	archive->SetType( AT_TextureCache );

	archive->SerializeHeader();
	textureCache.Serialize( *archive );

	// Clean up all data
	stbi_image_free( data );
	delete archive;
}