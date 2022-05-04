#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <qfileinfo.h>

#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"
#include "Render/RenderUtils.h"
#include "System/AssetsImport.h"
#include "WorldEd.h"

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

FHelperAssetImporter::EImportResult FHelperAssetImporter::Import( const QString& InPath, FPackage* InPackage, FAssetRef& OutAsset, std::wstring& OutError, bool InIsForceImport /* = false */ )
{
	checkMsg( InPackage, TEXT( "For import package must be valid" ) );

	QFileInfo		fileInfo( InPath );
	QString			assetName = fileInfo.baseName();
	std::wstring	fileExtension = fileInfo.suffix().toStdWString();

	// Check on already exist asset with name in package
	if ( !InIsForceImport && InPackage->IsExist( assetName.toStdWString() ) )
	{
		return IR_AlreadyExist;
	}

	// We import texture
	if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FTexture2DImporter::GetSupportedExtensions() ) )
	{
		OutAsset = FTexture2DImporter::Import( appQtAbsolutePathToEngine( InPath ), OutError );
	}

	// We import audio bank
	else if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FAudioBankImporter::GetSupportedExtensions() ) )
	{
		OutAsset = FAudioBankImporter::Import( appQtAbsolutePathToEngine( InPath ), OutError );
	}

	// Else this is unknown asset
	else
	{
		OutError = FString::Format( TEXT( "Unknown asset type '%s'" ), InPath.toStdWString().c_str() );
		LE_LOG( LT_Warning, LC_Editor, OutError.c_str() );
		return IR_Error;
	}

	// Add asset to package
	if ( OutAsset )
	{
		LE_LOG( LT_Log, LC_Editor, TEXT( "Imported asset '%s' from '%s' to package '%s'" ), OutAsset->GetAssetName().c_str(), InPath.toStdWString().c_str(), InPackage->GetName().c_str() );
		InPackage->Add( OutAsset );
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed import asset '%s' from '%s' to package '%s'. Error: %s" ), OutAsset->GetAssetName().c_str(), InPath.toStdWString().c_str(), InPackage->GetName().c_str(), OutError.c_str() );
	}

	return OutAsset ? IR_Seccussed : IR_Error;
}

bool FHelperAssetImporter::Reimport( FAsset* InAsset, std::wstring& OutError )
{
	check( InAsset );
	bool		bResult = false;

	switch ( InAsset->GetType() )
	{
	case AT_Texture2D:		bResult = FTexture2DImporter::Reimport( ( FTexture2D* )InAsset, OutError ); break;
	case AT_AudioBank:		bResult = FAudioBankImporter::Reimport( ( FAudioBank* )InAsset, OutError ); break;
	default:				return false;
	}

	if ( bResult )
	{
		LE_LOG( LT_Log, LC_Editor, TEXT( "Reimported asset '%s' from '%s'" ), InAsset->GetAssetName().c_str(), InAsset->GetAssetSourceFile().c_str() );
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed reimport asset '%s' from '%s'. Error %s" ), InAsset->GetAssetName().c_str(), InAsset->GetAssetSourceFile().c_str(), OutError.c_str() );
	}

	return bResult;
}

//
// TEXTURE 2D
//

FTexture2DRef FTexture2DImporter::Import( const std::wstring& InPath, std::wstring& OutError )
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
	return Reimport( texture2D, OutError ) ? texture2D : nullptr;
}

bool FTexture2DImporter::Reimport( FTexture2D* InTexture2D, std::wstring& OutError )
{
	check( InTexture2D );

	std::wstring		sourceFile = InTexture2D->GetAssetSourceFile();
	if ( sourceFile.empty() )
	{
		OutError = TEXT( "Source file is empty" );
		return false;
	}

	// Loading data from image
	int				numComponents = 0;
	uint32			sizeX = 0;
	uint32			sizeY = 0;
	void*			data = stbi_load( TCHAR_TO_ANSI( sourceFile.c_str() ), ( int* ) &sizeX, ( int* ) &sizeY, &numComponents, 4 );
	if ( !data )
	{
		OutError = FString::Format( TEXT( "Failed open file '%s'" ), sourceFile.c_str() );
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

FAudioBankRef FAudioBankImporter::Import( const std::wstring& InPath, std::wstring& OutError )
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
	return Reimport( audioBank, OutError ) ? audioBank : nullptr;
}

bool FAudioBankImporter::Reimport( FAudioBank* InAudioBank, std::wstring& OutError )
{
	check( InAudioBank );
	InAudioBank->SetSourceOGGFile( InAudioBank->GetAssetSourceFile() );
	return true;
}