#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"
#include "System/Package.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Scripts/Script.h"

FORCEINLINE FAsset* AssetFactory( EAssetType InType )
{
	switch ( InType )
	{
	case AT_Texture2D:		return new FTexture2D();
	case AT_Material:		return new FMaterial();
	case AT_Script:			return new FScript();

	default:	
		appErrorf( TEXT( "Unknown asset type %i" ), InType );
		return nullptr;
	}
}

FAsset::FAsset( EAssetType InType ) :
	hash( ( uint32 )INVALID_HASH ),
	type( InType ),
	package( nullptr )
{}

FAsset::~FAsset()
{
	// If asset in package, mark unload state
	if ( package )
	{
		package->MarkAssetUnlnoad( hash );
	}
}

void FAsset::Serialize( class FArchive& InArchive )
{}

void FAsset::SetHash( uint32 InHash )
{
	if ( package )
	{
		package->MarkHashAssetUpdate( hash, InHash );
	}
	hash = InHash;
}

FAssetReference FAsset::GetAssetReference() const
{
	return FAssetReference( hash, package ? package->GetPath() : TEXT( "" ) );
}

FPackage::FPackage() :
	archive( nullptr )
{}

FPackage::~FPackage()
{
	Close();
}

bool FPackage::Open( const std::wstring& InPath, bool InIsWrite /* = false */ )
{
	if ( InIsWrite )
	{
		archive = GFileSystem->CreateFileWriter( InPath );
	}
	else
	{
		archive = GFileSystem->CreateFileReader( InPath );
	}

	if ( !archive )
	{
		return false;
	}

	// Serialize header of archive and package
	archive->SetType( AT_Package );
	archive->SerializeHeader();

	uint32		packageFileTag = PACKAGE_FILE_TAG;
	*archive << packageFileTag;
	checkMsg( packageFileTag == PACKAGE_FILE_TAG, TEXT( "Unknown package file tag. Current package file tag is 0x%X, need 0x%X" ), packageFileTag, PACKAGE_FILE_TAG );
	
	path = InPath;
	return true;
}

void FPackage::Close()
{
	RemoveAll();

	path = TEXT( "" );
	if ( archive )
	{
		delete archive;
		archive = nullptr;
	}
}

void FPackage::Serialize()
{
	check( archive && archive->Ver() >= VER_Assets );

	if ( archive->IsSaving() )
	{
		for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
		{
			FAssetInfo&			assetInfo = itAsset->second;
			check( assetInfo.data );

			// Serialize asset header	
			assetInfo.offset = archive->Tell();
			*archive << assetInfo.data->type;
			*archive << assetInfo.data->hash;
			*archive << assetInfo.size;

			// Serialize asset
			uint32		startOffset = archive->Tell();
			assetInfo.data->Serialize( *archive );
			uint32		currentOffset = archive->Tell();

			// Update asset size in header
			assetInfo.size = currentOffset - startOffset;
			archive->Seek( startOffset - sizeof( assetInfo.size ) );
			*archive << assetInfo.size;
			archive->Seek( currentOffset );
		}
	}
	else
	{
		// Build asset table
		while ( !archive->IsEndOfFile() )
		{
			// Serialize asset header and add to table
			FAssetInfo			assetInfo;
			uint32				assetHash;
			appMemzero( &assetInfo, sizeof( FAssetInfo ) );
			assetInfo.offset = archive->Tell();

			// Skip asset type and serialize hash with size data
			archive->Seek( assetInfo.offset + sizeof( EAssetType ) );
			*archive << assetHash;
			*archive << assetInfo.size;

			// Skip asset data
			if ( assetInfo.size > 0 )
			{
				archive->Seek( archive->Tell() + assetInfo.size );
			}

			// Add asset info in table
			assetsTable[ assetHash ] = assetInfo;
		}
	}
}

void FPackage::MarkAssetUnlnoad( uint32 InHash )
{
	auto		itAsset = assetsTable.find( InHash );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	assetInfo.data = nullptr;

	if ( assetInfo.offset == INVALID_HASH && assetInfo.size == INVALID_HASH )
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "An asset was uploaded that was not recorded on the HDD. This asset has been removed from the package and will not be written" ) );
		assetsTable.erase( itAsset );
	}
}

void FPackage::MarkHashAssetUpdate( uint32 InOldHash, uint32 InNewHash )
{
	auto		itAsset = assetsTable.find( InOldHash );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	assetsTable[ InNewHash ] = assetInfo;
	assetsTable.erase( InOldHash );
}

void FPackage::Remove( uint32 InHash )
{
	auto		itAsset = assetsTable.find( InHash );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	if ( assetInfo.data )
	{
		assetInfo.data->package = nullptr;
	}

	assetsTable.erase( itAsset );
}

void FPackage::RemoveAll()
{
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;
		if ( assetInfo.data )
		{
			assetInfo.data->package = nullptr;
		}
	}

	assetsTable.clear();
}

FAssetRef FPackage::Find( uint32 InHash )
{
	auto		itAsset = assetsTable.find( InHash );
	uint32		oldOffset = archive->Tell();

	// If asset with this hash not found - return nullptr
	if ( itAsset == assetsTable.end() )
	{
		return nullptr;
	}

	FAssetInfo&			assetInfo = itAsset->second;

	// If asset allrady created - return it
	if ( assetInfo.data )
	{
		return assetInfo.data;
	}

	// Else we create asset from package
	EAssetType			assetType;
	archive->Seek( assetInfo.offset );

	*archive << assetType;
	assetInfo.data = AssetFactory( assetType );

	// Init asset
	assetInfo.data->hash = itAsset->first;
	assetInfo.data->package = this;

	// Skip hash with size asset and serialize data
	archive->Seek( archive->Tell() + sizeof( uint32 ) * 2 );

	uint32		startOffset = archive->Tell();
	assetInfo.data->Serialize( *archive );
	uint32		currentOffset = archive->Tell();

	check( currentOffset - startOffset == assetInfo.size );

	// Seek to old offset and exit
	archive->Seek( oldOffset );
	return assetInfo.data;
}