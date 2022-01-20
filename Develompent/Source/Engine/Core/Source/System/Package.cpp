#include "System/Config.h"
#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"
#include "System/Package.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/StaticMesh.h"
#include "Scripts/Script.h"

FORCEINLINE FAsset* AssetFactory( EAssetType InType )
{
	switch ( InType )
	{
	case AT_Texture2D:		return new FTexture2D();
	case AT_Material:		return new FMaterial();
	case AT_Script:			return new FScript();
	case AT_StaticMesh:		return new FStaticMesh();

	//
	// Instert new asset type her
	//

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
{
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_AssetName )
	{
		return;
	}

	InArchive << name;
}

void FAsset::SetAssetHash( uint32 InHash )
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
	archive( nullptr ),
	numUsageAssets( 0 )
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
		archive = GFileSystem->CreateFileReader( TEXT( "../../") + InPath);
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
			*archive << assetInfo.type;
			*archive << assetInfo.data->hash;
			*archive << assetInfo.size;

			// Serialize asset
			assetInfo.offset = archive->Tell();
			assetInfo.data->Serialize( *archive );
			uint32		currentOffset = archive->Tell();

			// Update asset size in header
			assetInfo.size = currentOffset - assetInfo.offset;
			archive->Seek( assetInfo.offset - sizeof( assetInfo.size ) );
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

			// Serialize hash with size data
			*archive << assetInfo.type;
			*archive << assetHash;
			*archive << assetInfo.size;
			assetInfo.offset = archive->Tell();

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
	if ( --numUsageAssets <= 0 )
	{
		GPackageManager->CheckUsagePackage( path );
	}

	if ( assetInfo.offset == INVALID_HASH && assetInfo.size == INVALID_HASH )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "An asset was uploaded that was not recorded on the HDD. This asset has been removed from the package and will not be written" ) );
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
	assetInfo.data = AssetFactory( assetInfo.type );

	// Init asset
	assetInfo.data->hash = itAsset->first;
	assetInfo.data->package = this;

	// Seek to asset data
	archive->Seek( assetInfo.offset );

	uint32		startOffset = archive->Tell();
	assetInfo.data->Serialize( *archive );
	uint32		currentOffset = archive->Tell();

	check( currentOffset - startOffset == assetInfo.size );

	// Seek to old offset and exit
	archive->Seek( oldOffset );
	
	if ( ++numUsageAssets == 1 )
	{
		GPackageManager->CheckUsagePackage( path );
	}
	return assetInfo.data;
}

FPackageManager::FPackageManager() :
	cleaningFrequency( 60.f ),
	lastCleaningTime( GStartTime )
{}

void FPackageManager::Init()
{
	FConfigValue		configCleaningFrequency = GEngineConfig.GetValue( TEXT( "Engine.PackageManager" ), TEXT( "CleaningFrequency" ) );
	if ( configCleaningFrequency.IsValid() )
	{
		cleaningFrequency = configCleaningFrequency.GetNumber();
	}
}

void FPackageManager::Tick()
{
	// We clean the bags every 'cleaningFrequency' seconds
	if ( GCurrentTime - lastCleaningTime >= cleaningFrequency )
	{	
		CleanupUnusedPackages();
		lastCleaningTime = GCurrentTime;
	}
}

void FPackageManager::Shutdown()
{}

void FPackageManager::CleanupUnusedPackages()
{
	if ( unusedPackages.empty() )
	{
		return;
	}

	for ( auto itUnusedPackage = unusedPackages.begin(), itUnusedPackageEnd = unusedPackages.end(); itUnusedPackage != itUnusedPackageEnd; ++itUnusedPackage )
	{
		const std::wstring& packagePath = *itUnusedPackage;
		auto						itPackage = packages.find( packagePath );
		if ( itPackage == packages.end() )
		{
			continue;
		}

		LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded package '%s'" ), packagePath.c_str() );
		packages.erase( packagePath );
	}

	unusedPackages.clear();
}

FAssetRef FPackageManager::FindAsset( const std::wstring& InPath, uint32 InHash )
{
	check( InHash != ( uint32 )INVALID_HASH );

	// Find package and open he
	FPackageRef			package;
	FPackageInfo*		packageInfo = nullptr;
	{
		auto		itPackage = packages.find( InPath );
		if ( itPackage == packages.end() )
		{
			package = new FPackage();
			if ( !package->Open( InPath ) )
			{
				package = nullptr;
			}
			else
			{
				package->Serialize();

				packages[ InPath ] = FPackageInfo{ true, package };
				packageInfo = &packages[ InPath ];
				LE_LOG( LT_Log, LC_Package, TEXT( "Package '%s' opened" ), InPath.c_str() );
			}
		}
		else
		{
			packageInfo = &itPackage->second;
			package = itPackage->second.package;
		}
	}

	if ( !package )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "Package '%s' not found" ), InPath.c_str() );
		return nullptr;
	}

	// Find asset in package
	FAssetRef		asset = package->Find( InHash );
	CheckUsagePackage( *packageInfo );
	return asset;
}

void FPackageManager::CheckUsagePackage( FPackageInfo& InOutPackageInfo )
{
	// If package in current time not used, we add he to unused list for remove in future
	check( InOutPackageInfo.package );
	bool					oldIsUnused = InOutPackageInfo.isUnused;
	const std::wstring&		packagePath = InOutPackageInfo.package->GetPath();

	InOutPackageInfo.isUnused = InOutPackageInfo.package->GetNumUsageAssets() <= 0;
	if ( InOutPackageInfo.isUnused && !oldIsUnused )
	{
		unusedPackages.insert( packagePath );
	}
	else if ( !InOutPackageInfo.isUnused && oldIsUnused )
	{
		unusedPackages.erase( packagePath );
	}
}