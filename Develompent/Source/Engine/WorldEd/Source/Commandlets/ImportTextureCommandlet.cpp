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
		LE_LOG( LT_Warning, LC_Commandlet, TEXT( "Texture name is not specified, by default it is assigned %s" ), srcFilename.c_str() );
	}

	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;
	void*			data = stbi_load( TCHAR_TO_ANSI( srcFilename.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	check( data );

	// Create texture 2D and serialize to package
	FTexture2D		texture2D;
	texture2D.SetAssetName( nameTexture );
	texture2D.SetAssetHash( appCalcHash( nameTexture ) );
	{
		std::vector< byte >		tempData;
		tempData.resize( sizeX * sizeY * GPixelFormats[ PF_A8R8G8B8 ].blockBytes );
		memcpy( tempData.data(), data, tempData.size() );
		texture2D.SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );
	}

	FPackage		package;
	if ( !package.Load( dstFilename ) )
	{
		std::wstring		packageName = dstFilename;

		// Find and remove first section of the string (before last '/')
		{
			std::size_t			posSlash = packageName.find_last_of( TEXT( "/" ) );
			if ( posSlash == std::wstring::npos )
			{
				posSlash = packageName.find_last_of( TEXT( "\\" ) );
			}

			if ( posSlash != std::wstring::npos )
			{
				packageName.erase( 0, posSlash + 1 );
			}
		}

		// Find and remove second section of the string (after last '.')
		{
			std::size_t			posDot = packageName.find_last_of( TEXT( "." ) );
			if ( posDot != std::wstring::npos )
			{
				packageName.erase( posDot, packageName.size() );
			}
		}

		// Set package name
		package.SetName( packageName );
	}

	package.Add( &texture2D );
	package.Save( dstFilename );

	// Clean up all data
	stbi_image_free( data );
}