#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Containers/StringConv.h"
#include "Render/RenderUtils.h"
#include "System/AssetsImport.h"

//
// GENERIC
//

QString FHelperAssetImporter::MakeFilterOfSupportedExtensions( uint32 InFlags /* = ET_All */ )
{
	QString				result;
	std::wstring		allSupportedFormats;

	// Texture formats
	if ( InFlags & ET_Texture2D )
	{
		const std::vector< std::wstring >&		textureExtension = FTexture2DImporter::GetSupportedExtensions();
		if ( !textureExtension.empty() )
		{
			result += "Texture formats (";
			for ( uint32 index = 0, count = textureExtension.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), textureExtension[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// Audio formats
	if ( InFlags & ET_AudioBank )
	{
		const std::vector< std::wstring >&		audioExtnesion = FAudioBankImporter::GetSupportedExtensions();
		if ( !audioExtnesion.empty() )
		{
			allSupportedFormats += !allSupportedFormats.empty() ? TEXT( ";" ) : TEXT( "" );
			result += "Audio formats (";
			for ( uint32 index = 0, count = audioExtnesion.size(); index < count; ++index )
			{
				std::wstring		format = FString::Format( TEXT( "*.%s%s" ), audioExtnesion[ index ].c_str(), index + 1 < count ? TEXT( ";" ) : TEXT( "" ) );
				result += format;
				allSupportedFormats += format;
			}
			result += ");;";
		}
	}

	// All supported formats
	{
		result += FString::Format( TEXT( "All supported formats (%s)" ), allSupportedFormats.c_str() );
	}

	return result;
}

//
// TEXTURE 2D
//

FTexture2DRef FTexture2DImporter::Import( const std::wstring& InPath )
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
	FTexture2DRef		texture2D = new FTexture2D();
	texture2D->SetAssetName( filename );
	texture2D->SetAssetSourceFile( InPath );
	return Reimport( texture2D ) ? texture2D : nullptr;
}

bool FTexture2DImporter::Reimport( FTexture2D* InTexture2D )
{
	check( InTexture2D );

	std::wstring		sourceFile = InTexture2D->GetAssetSourceFile();
	if ( sourceFile.empty() )
	{
		return false;
	}

	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;
	void* data = stbi_load( TCHAR_TO_ANSI( sourceFile.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	if ( !data )
	{
		return false;
	}

	// Set new data in texture
	std::vector< byte >		tempData;
	tempData.resize( sizeX * sizeY * GPixelFormats[ PF_A8R8G8B8 ].blockBytes );
	memcpy( tempData.data(), data, tempData.size() );
	InTexture2D->SetData( PF_A8R8G8B8, sizeX, sizeY, tempData );

	// Clean up all data
	stbi_image_free( data );
	return true;
}

//
// AUDIO BANK
//

FAudioBankRef FAudioBankImporter::Import( const std::wstring& InPath )
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
	FAudioBankRef		audioBank = new FAudioBank();
	audioBank->SetAssetName( filename );
	audioBank->SetAssetSourceFile( InPath );
	return Reimport( audioBank ) ? audioBank : nullptr;
}

bool FAudioBankImporter::Reimport( FAudioBank* InAudioBank )
{
	check( InAudioBank );
	InAudioBank->SetSourceOGGFile( InAudioBank->GetAssetSourceFile() );
	return true;
}