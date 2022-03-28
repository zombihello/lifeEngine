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
	std::wstring			pathToDir;
	std::wstring			outputPackage;
	bool					bFlipVertically = false;

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
			const std::wstring& param = switches[ index ];
			const std::wstring& token = tokens[ index + offsetInTokens ];

			// Source filename
			if ( param == TEXT( "src" ) || param == TEXT( "source" ) )
			{
				srcFilename = token;
			}

			// Path to directory with source files
			if ( param == TEXT( "dir" ) || param == TEXT( "directory" ) )
			{
				pathToDir = token;
			}

			// Output package
			else if ( param == TEXT( "pak" ) || param == TEXT( "package" ) )
			{
				outputPackage = token;
			}

			// Is need flip vertically
			else if ( param == TEXT( "fv" ) || param == TEXT( "flip-vertically" ) )
			{
				bFlipVertically = true;
			}
		}
	}

	checkMsg( ( srcFilename.empty() || pathToDir.empty() ) || outputPackage.empty(), TEXT( "Not entered source file or directory and destination package for saving" ) );
	checkMsg( !srcFilename.empty() || !pathToDir.empty(), TEXT( "Setted parameters '-src' and '-dir'. One must be displayed" ) );

	// Set flip vertically on load if need
	if ( bFlipVertically )
	{
		stbi_set_flip_vertically_on_load( true );
	}

	// Getting all source files for import
	std::vector< std::wstring >			srcFiles;
	if ( !srcFilename.empty() )
	{
		appNormalizePathSeparators( srcFilename );

		// Set path to directory
		pathToDir = srcFilename;
		std::size_t			pathSeparatorPos = pathToDir.find_last_of( PATH_SEPARATOR );
		if ( pathSeparatorPos != std::string::npos )
		{
			pathToDir.erase( pathSeparatorPos, pathToDir.size() );
		}

		// Get filename with extension
		if ( pathSeparatorPos != std::string::npos )
		{
			srcFilename.erase( 0, pathSeparatorPos + 1 );
		}
		srcFiles.push_back( srcFilename );
	}
	else
	{
		srcFiles = GFileSystem->FindFiles( pathToDir, true, false );
	}

	// Import all textures
	FPackageRef						package = GPackageManager->LoadPackage( outputPackage, true );
	std::vector< FTexture2DRef >	importedTextures;
	for ( uint32 index = 0, count = srcFiles.size(); index < count; ++index )
	{
		const std::wstring&		file = srcFiles[ index ];
		std::wstring			filename = file;
		{
			std::size_t			dotPos = file.find_last_of( TEXT( "." ) );
			std::wstring		extension = file;
			extension.erase( 0, dotPos + 1 );

			// We import texture only supported extensions
			if ( !IsSupportedExtension( extension ) )
			{
				continue;
			}

			// Getting filename
			appNormalizePathSeparators( filename );
			std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
			if ( pathSeparatorPos != std::string::npos )
			{
				filename.erase( 0, pathSeparatorPos + 1 );
			}

			dotPos = filename.find_last_of( TEXT( "." ) );
			if ( dotPos != std::string::npos )
			{
				filename.erase( dotPos, filename.size() + 1 );
			}
		}

		FTexture2DRef		texture2D = ConvertTexture2D( pathToDir + PATH_SEPARATOR + file, filename );
		check( texture2D );
		importedTextures.push_back( texture2D );
		package->Add( texture2D );

		bool	result = package->Save( outputPackage );
		check( result );
	}

	// Saving package
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