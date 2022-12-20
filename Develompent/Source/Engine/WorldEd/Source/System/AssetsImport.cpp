#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "System/AssetsImport.h"
#include "Render/RenderUtils.h"
#include "WorldEd.h"

TSharedPtr<CAsset> CTexture2DImporter::Import( const std::wstring& InPath, std::wstring& OutError )
{
	// Getting file name from path if InName is empty
	std::wstring		filename = InPath;

	appNormalizePathSeparators( filename );
	std::size_t			pathSeparatorPos = filename.find_last_of( PATH_SEPARATOR );
	if ( pathSeparatorPos != std::string::npos )
	{
		filename.erase( 0, pathSeparatorPos + 1 );
	}

	std::size_t dotPos = filename.find_last_of( TEXT( "." ) );
	if ( dotPos != std::string::npos )
	{
		filename.erase( dotPos, filename.size() + 1 );
	}

	// Import new texture 2D
	TSharedPtr<CTexture2D>		texture2DRef = MakeSharedPtr<CTexture2D>();
	texture2DRef->SetAssetName( filename );
	texture2DRef->SetAssetSourceFile( InPath );
	return Reimport( texture2DRef, OutError ) ? texture2DRef : nullptr;
}

bool CTexture2DImporter::Reimport( const TSharedPtr<CAsset>& InTexture2D, std::wstring& OutError )
{
	TSharedPtr<CTexture2D>		texture2D = InTexture2D;
	check( texture2D );

	std::wstring		sourceFile = texture2D->GetAssetSourceFile();
	if ( sourceFile.empty() )
	{
		OutError = TEXT( "Source file is empty" );
		return false;
	}

	// Loading data from image
	int		numComponents = 0;
	uint32	sizeX	= 0;
	uint32	sizeY	= 0;
	void*	data	= stbi_load( TCHAR_TO_ANSI( sourceFile.c_str() ), ( int* )&sizeX, ( int* )&sizeY, &numComponents, 4 );
	if ( !data )
	{
		OutError = CString::Format( TEXT( "Failed open file '%s'" ), sourceFile.c_str() );
		return false;
	}

	// Set new data in texture
	std::vector<byte>		tempData;
	tempData.resize( sizeX * sizeY * GPixelFormats[PF_A8R8G8B8].blockBytes );
	memcpy( tempData.data(), data, tempData.size() );
	texture2D->SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );

	// Clean up all data
	stbi_image_free( data );

	// Broadcast event of reimport/reloaded asset
	std::vector<TSharedPtr<CAsset>>		reimportedAssets{ texture2D };
	SEditorDelegates::onAssetsReloaded.Broadcast( reimportedAssets );

	return true;
}