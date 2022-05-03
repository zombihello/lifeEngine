#include <QFileInfo>

#include "Logger/LoggerMacros.h"
#include "System/AssetManager.h"
#include "System/AssetsImport.h"
#include "WorldEd.h"

FAssetManager::EImportResult FAssetManager::Import( const QString& InPath, FPackage* InPackage, FAssetRef& OutAsset, bool InIsForceImport /* = false */ )
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
		OutAsset = FTexture2DImporter::Import( appQtAbsolutePathToEngine( InPath ) );
	}

	// We import audio bank
	else if ( FHelperAssetImporter::IsSupportedExtension( fileExtension, FAudioBankImporter::GetSupportedExtensions() ) )
	{
		OutAsset = FAudioBankImporter::Import( appQtAbsolutePathToEngine( InPath ) );
	}

	// Else this is unknown asset
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Unknown asset type '%s'" ), InPath.toStdWString().c_str() );
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
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed import asset '%s' from '%s' to package '%s'" ), OutAsset->GetAssetName().c_str(), InPath.toStdWString().c_str(), InPackage->GetName().c_str() );
	}

	return OutAsset ? IR_Seccussed : IR_Error;
}

bool FAssetManager::Reimport( FAsset* InAsset )
{
	check( InAsset );
	bool		bResult = false;

	switch ( InAsset->GetType() )
	{
	case AT_Texture2D:		bResult = FTexture2DImporter::Reimport( ( FTexture2D* )InAsset ); break;
	case AT_AudioBank:		bResult = FAudioBankImporter::Reimport( ( FAudioBank* )InAsset ); break;
	default:				return false;
	}

	if ( bResult )
	{
		LE_LOG( LT_Log, LC_Editor, TEXT( "Reimported asset '%s' from '%s'" ), InAsset->GetAssetName().c_str(), InAsset->GetAssetSourceFile().c_str() );
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed reimport asset '%s' from '%s'" ), InAsset->GetAssetName().c_str(), InAsset->GetAssetSourceFile().c_str() );
	}

	return bResult;
}