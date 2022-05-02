#include "System/Config.h"
#include "Misc/CoreGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"
#include "System/Package.h"
#include "System/BaseEngine.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/StaticMesh.h"
#include "Scripts/Script.h"
#include "System/AudioBank.h"
#include "System/PhysicsMaterial.h"
#include "System/PhysicsEngine.h"

FORCEINLINE FAssetRef GetDefaultAsset( EAssetType InType )
{
	switch ( InType )
	{
	case AT_Texture2D:			return GEngine->GetDefaultTexture();
	case AT_Material:			return GEngine->GetDefaultMaterial();
	case AT_PhysicsMaterial:	return GPhysicsEngine.GetDefaultPhysMaterial();
	default:					
		/*appErrorf( TEXT( "Unknown asset type 0x%X" ), InType );*/		// TODO BS yehor.pohuliaka - Unknoment this error when fixed getting default asset in serialization
		return nullptr;
	}
}

FORCEINLINE FAsset* AssetFactory( EAssetType InType )
{
	switch ( InType )
	{
	case AT_Texture2D:			return new FTexture2D();
	case AT_Material:			return new FMaterial();
	case AT_Script:				return new FScript();
	case AT_StaticMesh:			return new FStaticMesh();
	case AT_AudioBank:			return new FAudioBank();
	case AT_PhysicsMaterial:	return new FPhysicsMaterial();

	//
	// Instert new asset type her
	//

	default:	
		appErrorf( TEXT( "Unknown asset type %i" ), InType );
		return nullptr;
	}
}

FAsset::FAsset( EAssetType InType ) 
	: package( nullptr )
	, guid( appCreateGuid() )
	, type( InType )
{}

FAsset::~FAsset()
{
	// If asset in package, mark unload state
	if ( package )
	{
		package->MarkAssetUnlnoad( guid );
	}
}

void FAsset::Serialize( class FArchive& InArchive )
{
	if ( InArchive.IsLoading() && ( InArchive.Ver() >= VER_AssetName && InArchive.Ver() < VER_AssetName_V2 ) )
	{
		InArchive << name;
	}

	if ( InArchive.Ver() >= VER_GUIDAssets )
	{
		InArchive << guid;
	}

#if WITH_EDITOR
	if ( !GIsCooker && InArchive.Ver() >= VER_AssetSourceFiles )
	{
		InArchive << sourceFile;
	}
#endif // WITH_EDITOR
}

void FAsset::SetAssetName( const std::wstring& InName )
{
	name = InName;
	if ( package )
	{
		package->UpdateAssetNameInTable( guid );
	}
}

FAssetReference FAsset::GetAssetReference() const
{
	return FAssetReference( type, guid, package ? package->GetGUID() : FGuid() );
}

FPackage::FPackage( const std::wstring& InName /* = TEXT( "" ) */ ) 
	: bIsDirty( false )
	, guid( appCreateGuid() )
	, name( InName )
	, numUsageAssets( 0 )
{}

FPackage::~FPackage()
{
	RemoveAll();
}

bool FPackage::Load( const std::wstring& InPath )
{
	RemoveAll();

	FArchive*		archive = GFileSystem->CreateFileReader( InPath );
	if ( !archive )
	{
		return false;
	}

	filename		= InPath;

	// Serialize header of archive
	archive->SerializeHeader();
	Serialize( *archive );

	delete archive;
	return true;
}

void FPackage::SetNameFromPath( const std::wstring& InPath )
{
	name = InPath;

	// Find and remove first section of the string (before last '/')
	{
		std::size_t			posSlash = name.find_last_of( TEXT( "/" ) );
		if ( posSlash == std::wstring::npos )
		{
			posSlash = name.find_last_of( TEXT( "\\" ) );
		}

		if ( posSlash != std::wstring::npos )
		{
			name.erase( 0, posSlash + 1 );
		}
	}

	// Find and remove second section of the string (after last '.')
	{
		std::size_t			posDot = name.find_last_of( TEXT( "." ) );
		if ( posDot != std::wstring::npos )
		{
			name.erase( posDot, name.size() );
		}
	}
}

bool FPackage::Save( const std::wstring& InPath )
{
	// Before saving package it needs to be fully loaded into memory
	std::vector< FAssetRef >		loadedAsset;
	FullyLoad( loadedAsset );

	// If package name not setted - take from path name of file
	if ( name.empty() )
	{
		SetNameFromPath( InPath );
	}

	FArchive*		archive = GFileSystem->CreateFileWriter( InPath );
	if ( !archive )
	{
		return false;
	}

	// Serialize header of archive
	archive->SetType( AT_Package );
	archive->SerializeHeader();
	Serialize( *archive );

	delete archive;
	filename = InPath;
	return true;
}

void FPackage::FullyLoad( std::vector<FAssetRef>& OutAssetArray )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return;
	}

	// Serialize all assets to memory
	FArchive*		archive = GFileSystem->CreateFileReader( filename, AR_NoFail );
	archive->SerializeHeader();
	SerializeHeader( *archive );

	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;

		// If asset info is not valid - return nullptr
		if ( assetInfo.offset == ( uint32 )INVALID_ID || assetInfo.size == ( uint32 )INVALID_ID )
		{
			continue;
		}

		// Load asset
		FAssetRef		asset = LoadAsset( *archive, itAsset->first, assetInfo );
		if ( asset )
		{
			OutAssetArray.push_back( asset );
		}
		else
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "Asset '%s' not loaded" ), assetInfo.name.c_str() );
		}
	}

	delete archive;
}

FAssetRef FPackage::Find( const FGuid& InGUID )
{
	// Find asset in table
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return nullptr;
	}

	// If asset already in memory - return it
	if ( itAsset->second.data )
	{
		return itAsset->second.data;
	}

	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return nullptr;
	}

	// Serialize asset from package
	FArchive*	archive = GFileSystem->CreateFileReader( filename );
	if ( !archive )
	{
		return nullptr;
	}

	archive->SerializeHeader();
	SerializeHeader( *archive );
	FAssetRef		asset = LoadAsset( *archive, itAsset->first, itAsset->second );

	delete archive;
	return asset;
}

void FPackage::Serialize( FArchive& InArchive )
{
	SerializeHeader( InArchive );
	check( InArchive.Ver() >= VER_Assets );

	if ( InArchive.IsSaving() )
	{
		for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
		{
			FAssetInfo&			assetInfo = itAsset->second;
			if ( !assetInfo.data )
			{
				LE_LOG( LT_Warning, LC_Package, TEXT( "Asset '%s' is not valid, skiped saving to package" ), assetInfo.name.c_str() );
				continue;
			}

			// Serialize asset header	
			InArchive << assetInfo.type;
			InArchive << assetInfo.name;
			InArchive << assetInfo.data->guid;
			InArchive << assetInfo.size;

			// Serialize asset
			assetInfo.offset = InArchive.Tell();
			assetInfo.data->Serialize( InArchive );
			uint32		currentOffset = InArchive.Tell();

			// Update asset size in header
			assetInfo.size = currentOffset - assetInfo.offset;
			InArchive.Seek( assetInfo.offset - sizeof( assetInfo.size ) );
			InArchive << assetInfo.size;
			InArchive.Seek( currentOffset );
		}
	}
	else
	{
		// Build asset table
		while ( !InArchive.IsEndOfFile() )
		{
			// Serialize asset header and add to table
			FAssetInfo			assetInfo;
			FGuid				assetGUID;
			appMemzero( &assetInfo, sizeof( FAssetInfo ) );

			// Serialize hash with size data
			InArchive << assetInfo.type;

			if ( InArchive.Ver() >= VER_AssetName_V2 )
			{
				InArchive << assetInfo.name;
			}

			if ( InArchive.Ver() < VER_GUIDAssets )
			{
				uint32		hash = 0;
				InArchive << hash;
				assetGUID.Set( hash, 0, 0, 0 );
			}
			else
			{
				InArchive << assetGUID;
			}

			InArchive << assetInfo.size;
			assetInfo.offset = InArchive.Tell();

			// Skip asset data	
			if ( assetInfo.size > 0 )
			{
				InArchive.Seek( InArchive.Tell() + assetInfo.size );
			}

			// Add asset info in table
			assetGUIDTable[ assetInfo.name ] = assetGUID;
			assetsTable[ assetGUID ] = assetInfo;
		}
	}

	bIsDirty = false;
}

void FPackage::SerializeHeader( FArchive& InArchive )
{
	check( InArchive.Ver() >= VER_NamePackage );
	uint32		packageFileTag = PACKAGE_FILE_TAG;
	InArchive << packageFileTag;
	checkMsg( packageFileTag == PACKAGE_FILE_TAG, TEXT( "Unknown package file tag. Current package file tag is 0x%X, need 0x%X" ), packageFileTag, PACKAGE_FILE_TAG );

#if DO_CHECK
	if ( InArchive.IsSaving() )
	{
		checkMsg( !name.empty(), TEXT( "The package must have a name" ) );
	}
#endif // DO_CHECK

	InArchive << guid;
	InArchive << name;
}

FAssetRef FPackage::LoadAsset( FArchive& InArchive, const FGuid& InAssetGUID, FAssetInfo& InAssetInfo )
{
	uint32		oldOffset = InArchive.Tell();

	// If asset info is not valid - return nullptr
	if ( InAssetInfo.offset == ( uint32 )INVALID_ID || InAssetInfo.size == ( uint32 )INVALID_ID )
	{
		return nullptr;
	}

	// If asset already created - return it
	if ( InAssetInfo.data )
	{
		return InAssetInfo.data;
	}

	// Else we create asset from package	
	InAssetInfo.data = AssetFactory( InAssetInfo.type );

	// Init asset
	InAssetInfo.data->guid = InAssetGUID;
	InAssetInfo.data->package = this;
	InAssetInfo.data->name = InAssetInfo.name;

	// Seek to asset data
	InArchive.Seek( InAssetInfo.offset );

	uint32		startOffset = InArchive.Tell();
	InAssetInfo.data->Serialize( InArchive );
	uint32		currentOffset = InArchive.Tell();

	check( currentOffset - startOffset == InAssetInfo.size );

	if ( InArchive.Ver() < VER_AssetName_V2 )
	{
		InAssetInfo.name = InAssetInfo.data->name;
	}

	// Seek to old offset and exit
	InArchive.Seek( oldOffset );

	if ( ++numUsageAssets == 1 )
	{
		GPackageManager->CheckUsagePackage( filename );		// TODO BS yehor.pohuliaka - AHTUNG! Need change 'filename' to GUID of the package
	}
	return InAssetInfo.data;
}

void FPackage::MarkAssetUnlnoad( const FGuid& InGUID )
{
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	assetInfo.data = nullptr;
	if ( --numUsageAssets <= 0 )
	{
		GPackageManager->CheckUsagePackage( filename );		// TODO BS yehor.pohuliaka - AHTUNG! Need change 'filename' to GUID of the package
	}

	if ( assetInfo.offset == INVALID_HASH && assetInfo.size == INVALID_HASH )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "An asset was uploaded that was not recorded on the HDD. This asset has been removed from the package and will not be written" ) );
		assetsTable.erase( itAsset );
	}
}

void FPackage::UpdateAssetNameInTable( const FGuid& InGUID )
{
	// Find asset in package
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	check( assetInfo.data );
	FAsset*			asset = ( FAsset* )assetInfo.data;

	// Remove from GUID table old name
	assetGUIDTable.erase( assetInfo.name );

	// Add to GUID table new name and update asset info
	assetGUIDTable[ asset->GetAssetName() ] = InGUID;
	assetInfo.name = asset->GetAssetName();
}

void FPackage::Remove( const FGuid& InGUID )
{
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	FAssetInfo&		assetInfo = itAsset->second;
	if ( assetInfo.data )
	{
		assetInfo.data->package = nullptr;
		--numUsageAssets;
	}

	auto		itAssetGUID = assetGUIDTable.find( assetInfo.name );
	if ( itAssetGUID != assetGUIDTable.end() )
	{
		assetGUIDTable.erase( itAssetGUID );
	}

	assetsTable.erase( itAsset );
	bIsDirty = true;
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

	assetGUIDTable.clear();
	assetsTable.clear();
	numUsageAssets = 0;
	bIsDirty = true;
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
	// We clean the packages every 'cleaningFrequency' seconds and only for build game (in editor not execute)
	if ( !GIsEditor && GCurrentTime - lastCleaningTime >= cleaningFrequency )
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
		const std::wstring&		packagePath = *itUnusedPackage;
		auto					itPackage = packages.find( packagePath );
		if ( itPackage == packages.end() )
		{
			continue;
		}

		LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded package '%s'" ), packagePath.c_str() );
		packages.erase( packagePath );
	}

	unusedPackages.clear();
}

bool ParseReferenceToAsset( const std::wstring& InString, std::wstring& OutPackageName, std::wstring& OutAssetName )
{
	// Divide the string into two parts: package name and asset name
	std::size_t			posSpliter = InString.find( TEXT( ":" ) );
	if ( posSpliter == std::wstring::npos )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "Not correct input string '%s', reference to asset must be splitted by '<Package name>:<Asset name>'" ), InString.c_str() );
		return false;
	}

	OutPackageName = InString;
	OutAssetName = InString;
	OutPackageName.erase( posSpliter, OutPackageName.size() );
	OutAssetName.erase( 0, posSpliter + 1 );
	return true;
}

FAssetRef FPackageManager::FindAsset( const std::wstring& InString, EAssetType InType /* = AT_Unknown */ )
{
	std::wstring		packageName;
	std::wstring		assetName;
	if ( !ParseReferenceToAsset( InString, packageName, assetName ) )
	{
		return nullptr;
	}

	// Find in TOC path to the package and calculate hash of asset from him name
	std::wstring	packagePath = GTableOfContents.GetPackagePath( packageName );
	if ( packagePath.empty() )
	{
		if ( !GIsCooker )
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "Package with name '%s' not found in TOC file" ), packageName.c_str() );
		}
		return nullptr;
	}

	return FindAsset( packagePath, assetName, InType );
}

FAssetRef FPackageManager::FindAsset( const std::wstring& InPath, const FGuid& InGUIDAsset, EAssetType InType /* = AT_Unknown */ )
{
	check( InGUIDAsset.IsValid() );

	// Find package and open he
	FAssetRef			asset;
	FPackageRef			package = LoadPackage( InPath );
	if ( !package )
	{
		asset = GetDefaultAsset( InType );
	}

	// Find asset in package
	if ( package )
	{
		FPackageInfo* packageInfo = &packages[ InPath ];
		asset = package->Find( InGUIDAsset );
		if ( !asset )
		{
			asset = GetDefaultAsset( InType );
		}
		else
		{
			CheckUsagePackage( *packageInfo );
		}
	}

	return asset;
}

FAssetRef FPackageManager::FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType /* = AT_Unknown */ )
{
	check( !InAsset.empty() );

	// Find package and open he
	FAssetRef			asset;
	FPackageRef			package = LoadPackage( InPath );
	if ( !package )
	{
		asset = GetDefaultAsset( InType );
	}

	// Find asset in package
	if ( package )
	{
		FPackageInfo* packageInfo = &packages[ InPath ];
		asset = package->Find( InAsset );
		if ( !asset )
		{
			asset = GetDefaultAsset( InType );
		}
		else
		{
			CheckUsagePackage( *packageInfo );
		}
	}

	return asset;
}

FAssetRef FPackageManager::FindDefaultAsset( EAssetType InType )
{
	return GetDefaultAsset( InType );
}

FPackageRef FPackageManager::LoadPackage( const std::wstring& InPath, bool InCreateIfNotExist /*= false */ )
{
	FPackageRef			package;
	auto				itPackage = packages.find( InPath );
	
	if ( itPackage == packages.end() )
	{
		package = new FPackage();
		if ( !package->Load( InPath ) && !InCreateIfNotExist )
		{
			package = nullptr;
		}
		else
		{
			packages[ InPath ] = FPackageInfo{ false, package };
			LE_LOG( LT_Log, LC_Package, TEXT( "Package '%s' opened" ), InPath.c_str() );
			
			package->SetNameFromPath( InPath );
			GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), InPath );
		}
	}
	else
	{
		package = itPackage->second.package;
	}

	if ( !package )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "Package '%s' not found" ), InPath.c_str() );
	}
	else
	{
		CheckUsagePackage( packages[ InPath ] );
	}

	return package;
}

bool FPackageManager::UnloadPackage( const std::wstring& InPath )
{
	auto		itPackage = packages.find( InPath );
	if ( itPackage == packages.end() || itPackage->second.package->GetNumUsageAssets() > 0 )
	{
		return false;
	}

	unusedPackages.erase( InPath );
	packages.erase( itPackage );

	LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded package '%s'" ), InPath.c_str() );
	return true;
}

void FPackageManager::CheckUsagePackage( FPackageInfo& InOutPackageInfo )
{
	// If package in current time not used, we add he to unused list for remove in future
	check( InOutPackageInfo.package );
	bool					oldIsUnused = InOutPackageInfo.isUnused;
	const std::wstring&		packagePath = InOutPackageInfo.package->GetFileName();

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
