#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/Package.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Render/Texture.h"
#include "Render/RenderUtils.h"
#include "Commandlets/ImportTextureCommandlet.h"

IMPLEMENT_CLASS( LImportTextureCommandlet )

bool LImportTextureCommandlet::Main( const std::wstring& InCommand )
{
	std::wstring			srcFilename;
	std::wstring			dstFilename;
	std::wstring			nameTexture;
	bool					isAppend = false;

	// Parse arguments
	{
		uint32							offsetInTokens = 0;		// For skip in cmdline path to exe "C:/HeliumGame.exe"
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCommand.c_str(), tokens, switches );

		if ( tokens.size() > switches.size() )
		{
			offsetInTokens = tokens.size() - switches.size();
		}

		for ( uint32 index = 0, count = ( uint32 )switches.size(); index < count; ++index )
		{
			const std::wstring&		param = switches[ index ];
			const std::wstring&		token = tokens[ index + offsetInTokens ];

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
		LE_LOG( LT_Warning, LC_Commandlet, TEXT( "Texture name is not specified, by default it is assigned %s" ), srcFilename.c_str() );
	}

	// Convert image to texture 2D
	FTexture2DRef		texture2D = ConvertTexture2D( srcFilename, nameTexture );
	if ( !texture2D )
	{
		return false;
	}

	FPackage		package;
	package.Load( dstFilename );
	package.Add( texture2D );
	package.Save( dstFilename );
	return true;
}

FTexture2DRef LImportTextureCommandlet::ConvertTexture2D( const std::wstring& InPath, const std::wstring& InAssetName )
{
	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;

	void*			data = stbi_load( TCHAR_TO_ANSI( InPath.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	if ( !data )
	{
		return nullptr;
	}

	// Create texture 2D and init him
	FTexture2DRef		texture2D = new FTexture2D();
	texture2D->SetAssetName( InAssetName );
	{
		std::vector< byte >		tempData;
		tempData.resize( sizeX * sizeY * GPixelFormats[ PF_A8R8G8B8 ].blockBytes );
		memcpy( tempData.data(), data, tempData.size() );
		texture2D->SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );
	}

	// Clean up all data
	stbi_image_free( data );
	return texture2D;
}