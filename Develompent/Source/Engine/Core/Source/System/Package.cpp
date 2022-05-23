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

#if WITH_EDITOR
#include "WorldEd.h"
#endif // WITH_EDITOR

FORCEINLINE TAssetHandle<FAsset> GetDefaultAsset( EAssetType InType )
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

FORCEINLINE TSharedPtr<FAsset> AssetFactory( EAssetType InType )
{
	switch ( InType )
	{
	case AT_Texture2D:			return MakeSharedPtr<FTexture2D>();
	case AT_Material:			return MakeSharedPtr<FMaterial>();
	case AT_Script:				return MakeSharedPtr<FScript>();
	case AT_StaticMesh:			return MakeSharedPtr<FStaticMesh>();
	case AT_AudioBank:			return MakeSharedPtr<FAudioBank>();
	case AT_PhysicsMaterial:	return MakeSharedPtr<FPhysicsMaterial>();

	//
	// Instert new asset type her
	//

	default:	
		appErrorf( TEXT( "Unknown asset type %i" ), InType );
		return nullptr;
	}
}

FAsset::FAsset( EAssetType InType ) 
	: bDirty( true )		// by default package is dirty because not serialized package from HDD
	, package( nullptr )
	, guid( appCreateGuid() )
	, type( InType )
{}

FAsset::~FAsset()
{}

void FAsset::Serialize( class FArchive& InArchive )
{
	InArchive << name;

	if ( InArchive.Ver() >= VER_GUIDAssets )
	{
		InArchive << guid;
	}


	if ( InArchive.Ver() >= VER_AssetSourceFiles )
	{
#if !WITH_EDITOR
		std::wstring	sourceFile;
#endif // !WITH_EDITOR

		InArchive << sourceFile;
	}

	bDirty = false;
}

void FAsset::SetAssetName( const std::wstring& InName )
{
	if ( name != InName )
	{
		MarkDirty();
	}

	name = InName;
	if ( package )
	{
		package->UpdateAssetNameInTable( guid );
	}
}

void FAsset::MarkDirty()
{
	// If asset already is dirty, we nothing do
	if ( bDirty )
	{
		return;
	}

	// Else we mark asset is dirty and tell about this to package
	bDirty = true;
	if ( package )
	{
		package->MarkAssetDirty( guid );
	}
}

void FAsset::GetDependentAssets( FSetDependentAssets& OutDependentAssets, EAssetType InFilter /* = AT_Unknown */ ) const
{}

void FAsset::ReloadDependentAssets( bool InForce /* = false */ )
{}

FAssetReference FAsset::GetAssetReference() const
{
	return FAssetReference( type, guid, package ? package->GetGUID() : FGuid() );
}

TAssetHandle<FAsset> FAsset::GetAssetHandle() const
{
	if ( !handle.IsValid() )
	{
		handle = TAssetHandle<FAsset>( SharedThis( this ), MakeSharedPtr<FAssetReference>( type, guid, package ? package->GetGUID() : FGuid() ) );
	}
	return handle;
}

FPackage::FPackage( const std::wstring& InName /* = TEXT( "" ) */ ) 
	: bIsDirty( true )			// by default package is dirty because not serialized package from HDD
	, guid( appCreateGuid() )
	, name( InName )
	, numLoadedAssets( 0 )
	, numDirtyAssets( 0 )
{}

FPackage::~FPackage()
{
	RemoveAll( true );
}

bool FPackage::Load( const std::wstring& InPath )
{
	RemoveAll( true );

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
	std::vector< TAssetHandle<FAsset> >		loadedAsset;
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

void FPackage::FullyLoad( std::vector< TAssetHandle<FAsset> >& OutAssetArray )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return;
	}

	// Serialize all assets to memory
	FArchive*		archive = GFileSystem->CreateFileReader( filename, AR_NoFail );
	archive->SerializeHeader();
	SerializeHeader( *archive, true );

	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;

		// If asset info is not valid - return nullptr
		if ( assetInfo.offset == ( uint32 )INVALID_ID || assetInfo.size == ( uint32 )INVALID_ID )
		{
			continue;
		}

		// Load asset
		TAssetHandle<FAsset>		asset = LoadAsset( *archive, itAsset->first, assetInfo );
		if ( asset.IsAssetValid() )
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

TAssetHandle<FAsset> FPackage::Find( const FGuid& InGUID )
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
		return itAsset->second.data->GetAssetHandle();
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
	TAssetHandle<FAsset>		asset = LoadAsset( *archive, itAsset->first, itAsset->second );

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
			FAssetInfo			localAssetInfo;
			FGuid				assetGUID;
			appMemzero( &localAssetInfo, sizeof( FAssetInfo ) );

			// Serialize hash with size data
			InArchive << localAssetInfo.type;

			if ( InArchive.Ver() >= VER_AssetName_V2 )
			{
				InArchive << localAssetInfo.name;
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

			InArchive << localAssetInfo.size;
			localAssetInfo.offset = InArchive.Tell();

			// Skip asset data	
			if ( localAssetInfo.size > 0 )
			{
				InArchive.Seek( InArchive.Tell() + localAssetInfo.size );
			}

			// Update asset info in table
			assetGUIDTable[ localAssetInfo.name ] = assetGUID;
			
			FAssetInfo&			assetInfo = assetsTable[ assetGUID ];
			localAssetInfo.data = assetInfo.data;
			assetInfo			= localAssetInfo;
		}
	}

	bIsDirty		= false;
	numDirtyAssets	= 0;
}

void FPackage::SerializeHeader( FArchive& InArchive, bool InIsNeedSkip /* = false */ )
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

	if ( !InIsNeedSkip )
	{
		InArchive << guid;
		InArchive << name;
	}
	else
	{
		FGuid			tmpGuid;
		std::wstring	tmpName;
		InArchive << tmpGuid;
		InArchive << tmpName;
	}
}

TAssetHandle<FAsset> FPackage::LoadAsset( FArchive& InArchive, const FGuid& InAssetGUID, FAssetInfo& InAssetInfo, bool InNeedReload /* = false */ )
{
	uint32		oldOffset = InArchive.Tell();

	// If asset info is not valid - return nullptr
	if ( InAssetInfo.offset == ( uint32 )INVALID_ID || InAssetInfo.size == ( uint32 )INVALID_ID )
	{
		return nullptr;
	}

	// Is already valid asset
	bool		bValidAsset = InAssetInfo.data;

	// Allocate asset if it not valid
	if ( !InAssetInfo.data )
	{
		InAssetInfo.data			= AssetFactory( InAssetInfo.type );
		InAssetInfo.data->guid		= InAssetGUID;
		InAssetInfo.data->package	= this;
	}

	// If asset not need reload - return already created handle
	if ( bValidAsset && !InNeedReload )
	{
		return InAssetInfo.data->GetAssetHandle();
	}

	// Seek to asset data
	InArchive.Seek( InAssetInfo.offset );

	uint32		startOffset = InArchive.Tell();
	InAssetInfo.data->Serialize( InArchive );
	uint32		currentOffset = InArchive.Tell();

	check( currentOffset - startOffset == InAssetInfo.size );

	// If asset is reloaded, we need update GUID table
	if ( InNeedReload )
	{
		UpdateAssetNameInTable( InAssetInfo.data->guid );
	}

	// Take asset name from data
	InAssetInfo.name							= InAssetInfo.data->name;

	// Increment number of loaded assets if we not reloaded him
	if ( !bValidAsset )
	{
		++numLoadedAssets;
	}

	// Seek to old offset and exit
	InArchive.Seek( oldOffset );
	return InAssetInfo.data->GetAssetHandle();
}

void FPackage::MarkAssetDirty( const FGuid& InGUID )
{
	bIsDirty = true;
	if ( numDirtyAssets != -1 )
	{
		++numDirtyAssets;
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

	// Remove from GUID table old name
	assetGUIDTable.erase( assetInfo.name );

	// Add to GUID table new name and update asset info
	assetGUIDTable[ assetInfo.data->name ] = InGUID;
	assetInfo.name = assetInfo.data->name;
}

void FPackage::Add( const TAssetHandle<FAsset>& InAsset )
{
	check( InAsset.IsAssetValid() );

	TSharedPtr<FAsset>		assetRef = InAsset.ToSharedPtr();
	checkMsg( assetRef->guid.IsValid(), TEXT( "For add asset to package need GUID is valid" ) );

	// If asset in package already containing, remove from table old GUID
	auto		it = assetGUIDTable.find( assetRef->name );
	if ( it != assetGUIDTable.end() && it->second != assetRef->guid )
	{
		assetsTable.erase( it->second );
	}

	// Update guid package in asset reference
	InAsset.reference->guidPackage		= guid;

	bIsDirty							= true;
	assetRef->package					= this;
	assetRef->bDirty					= true;
	assetGUIDTable[ assetRef->name ]	= assetRef->guid;
	assetsTable[ assetRef->guid ]		= FAssetInfo{ ( uint32 )INVALID_ID, ( uint32 )INVALID_ID, assetRef->type, assetRef->name, assetRef };
	++numLoadedAssets;
}

bool FPackage::Remove( const FGuid& InGUID, bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return true;
	}

	// Unload asset, if failed we exit from method
	FAssetInfo&				assetInfo = itAsset->second;
	TWeakPtr<FAsset>		assetPtr = assetInfo.data;
	if ( assetInfo.data && !UnloadAsset( assetInfo, InForceUnload, true, InIgnoreDirty ) )
	{
		return false;
	}

	// If the asset remains in memory, then in this case we reset its link to this package
	{
		TSharedPtr<FAsset>		assetRef = assetPtr.Pin();
		if ( assetRef )
		{
			assetRef->package = nullptr;
			assetRef->GetAssetHandle().GetReference()->guidPackage.Invalidate();
		}
	}

	// Remove from package
	auto		itAssetGUID = assetGUIDTable.find( assetInfo.name );
	if ( itAssetGUID != assetGUIDTable.end() )
	{
		assetGUIDTable.erase( itAssetGUID );
	}

	assetsTable.erase( itAsset );

	bIsDirty		= true;
	numDirtyAssets	= -1;
	return true;
}

bool FPackage::RemoveAll( bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	// Fill array of assets to unload. Need it for remove reference to this package if him remained in memory
	std::vector< TWeakPtr<FAsset> >		assetsToUnload;
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;
		if ( assetInfo.data )
		{
			assetsToUnload.push_back( assetInfo.data );
		}
	}

	// If we were unable to unload all the assets, we exit the method
	if ( !UnloadAllAssetsInternal( InForceUnload, InIgnoreDirty ) )
	{
		return false;
	}

	// If it is still used, unbind the package
	for ( uint32 index = 0, count = assetsToUnload.size(); index < count; ++index )
	{
		TSharedPtr<FAsset>		assetRef = assetsToUnload[ index ].Pin();
		if ( assetRef )
		{
			assetRef->package = nullptr;
			assetRef->GetAssetHandle().GetReference()->guidPackage.Invalidate();
		}
	}

	assetGUIDTable.clear();
	assetsTable.clear();
	numLoadedAssets	= 0;
	bIsDirty		= true;
	numDirtyAssets	= -1;
	return true;
}

bool FPackage::UnloadAsset( FAssetInfo& InAssetInfo, bool InForceUnload /* = false */, bool InBroadcastEvent /* = true */, bool InIgnoreDirty /* = false */ )
{
	// We must unload only not dirty assets and with unique shared reference
	if ( InAssetInfo.data && ( !InIgnoreDirty ? !InAssetInfo.data->bDirty : true ) )
	{
		// If we not must force unload and is not unique - exit from method
		if ( !InForceUnload && !InAssetInfo.data.IsUnique() )
		{
			return false;
		}

		// Is can delete this asset
#if WITH_EDITOR
		if ( GIsEditor && InBroadcastEvent )
		{
			FCanDeleteAssetResult					result;
			std::vector< TSharedPtr<FAsset>	>		assets = { InAssetInfo.data };
			FEditorDelegates::onAssetsCanDelete.Broadcast( assets, result );

			// If this asset is blocked, we exit from method
			if ( !result.Get() )
			{
				return false;
			}

			// Else we tell all what asset is deleted
			FEditorDelegates::onAssetsDeleted.Broadcast( assets );
		}
#endif // WITH_EDITOR

		// If the asset is dirty, then we reduce the number of dirty assets in the package, 
		// if the package itself has not been changed (name change, etc)
		if ( numDirtyAssets != -1 && InAssetInfo.data->IsDirty() && --numDirtyAssets <= 0 )
		{
			bIsDirty = false;
		}

		// If the asset was added to the package only in memory, then we remove its mention 
		// from the package itself, since data is needed to write to the HDD, which is now unloading
		if ( InAssetInfo.offset == INVALID_ID && InAssetInfo.size == INVALID_ID )
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "An asset was uploaded that was not recorded on the HDD. This asset has been removed from the package and will not be written" ) );
			InAssetInfo.data->package = nullptr;
			InAssetInfo.data->GetAssetHandle().GetReference()->guidPackage.Invalidate();
			assetsTable.erase( InAssetInfo.data->GetGUID() );
		}
		// Else we only destroy asset
		else
		{
			InAssetInfo.data.Reset();
		}

		// Decrement number of loaded assets
		if ( numLoadedAssets > 0 )
		{
			--numLoadedAssets;
		}
	
		return true;
	}

	return false;
}

bool FPackage::UnloadAllAssetsInternal( bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	bool		bExistDirtyAssets = false;
	std::vector<FAssetInfo*>		assetsToUnload;

	// Fill array assets to unload
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;

		// If asset is not loaded - skip it
		if ( !assetInfo.data )
		{
			continue;
		}

		// We must unload only not dirty assets
		if ( !assetInfo.data->bDirty )
		{
			assetsToUnload.push_back( &assetInfo );
		}
		else
		{
			bExistDirtyAssets = true;
		}
	}

	// If we in editor, fill array for checking is available unload this assets
#if WITH_EDITOR
	if ( GIsEditor )
	{
		// Convert array of FAssetInfo to TSharedPtr<FAsset> for events
		std::vector< TSharedPtr<FAsset> >		assets;
		for ( uint32 index = 0, count = assetsToUnload.size(); index < count; ++index )
		{
			assets.push_back( assetsToUnload[ index ]->data );
		}

		// Tell all user of we want delete this assets 
		FCanDeleteAssetResult		result;
		FEditorDelegates::onAssetsCanDelete.Broadcast( assets, result );

		// If this assets is blocked, we exit from method
		if ( !result.Get() )
		{
			return false;
		}

		// Else we tell all what asset is deleted
		FEditorDelegates::onAssetsDeleted.Broadcast( assets );
	}
#endif // WITH_EDITOR

	// Unload all assets
	for ( uint32 index = 0, count = assetsToUnload.size(); index < count; ++index )
	{
		bExistDirtyAssets |= !UnloadAsset( *assetsToUnload[ index ], InForceUnload, false, InIgnoreDirty );
	}

	return !bExistDirtyAssets;
}

bool FPackage::ReloadAsset( FAssetInfo& InAssetInfo )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return false;
	}

	// We must reload only already loaded assets
	if ( !InAssetInfo.data )
	{
		return false;
	}

	// Open package for reload asset
	FArchive*		archive = GFileSystem->CreateFileReader( filename );
	if ( !archive )
	{
		return false;
	}
	
	// Serialize header of archive and package
	archive->SerializeHeader();
	SerializeHeader( *archive, true );

	// Reload asset
	bool	bDirtyAsset	= InAssetInfo.data->bDirty;
	bool	bResult		= LoadAsset( *archive, InAssetInfo.data->guid, InAssetInfo, true ).IsAssetValid();
	delete archive;
	
	// If the asset is not dirty, then we reduce the number of dirty assets in the package, 
	// if the package itself has not been changed (name change, etc)
	if ( numDirtyAssets != -1 && bDirtyAsset && !InAssetInfo.data->bDirty && --numDirtyAssets <= 0 )
	{
		bIsDirty = false;
	}

	// Broadcast event of reloaded assets
#if WITH_EDITOR
	if ( GIsEditor )
	{
		std::vector< TSharedPtr<FAsset>	>		assets = { InAssetInfo.data };
		FEditorDelegates::onAssetsReloaded.Broadcast( assets );
	}
#endif // WITH_EDITOR

	return bResult;
}

bool FPackage::ReloadPackage( bool InOnlyAsset /* = false */ )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return false;
	}

	// Open package for reload asset
	FArchive*		archive = GFileSystem->CreateFileReader( filename );
	if ( !archive )
	{
		return false;
	}

	// Serialize header of archive
	archive->SerializeHeader();

	// If we reload all package - need serialize asset table
	if ( !InOnlyAsset )
	{
		// TODO BS yehor.pohuliaka - Maybe need clear asset table and unload all assets when not exist in package on HDD?
		Serialize( *archive );
	}
	// Else we serialize only header
	else
	{
		SerializeHeader( *archive, InOnlyAsset );
	}

	// Reload all assets
#if WITH_EDITOR
	std::vector< TSharedPtr<FAsset> >	reloadedAssets;
#endif // WITH_EDITOR

	bool		bNotAllAssetLoaded = false;
	uint32		numNewDirtyAssets = 0;
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		FAssetInfo&		assetInfo = itAsset->second;

		// If asset is not loaded - skip it
		if ( !assetInfo.data )
		{
			continue;
		}

		bool	bDirtyAsset		= assetInfo.data->bDirty;
		bool	bLocalResult	= LoadAsset( *archive, assetInfo.data->guid, assetInfo, true ).IsAssetValid();
		bNotAllAssetLoaded |= !bLocalResult;
		
		// If the asset is not dirty, then we reduce the number of dirty assets in the package, 
		// if the package itself has not been changed (name change, etc)
		if ( numDirtyAssets != -1 && bDirtyAsset && !assetInfo.data->bDirty && --numDirtyAssets <= 0 )
		{
			bIsDirty = false;
		}

		// If asset already dirty - increment number of new dirty assets
		if ( assetInfo.data->bDirty )
		{
			++numNewDirtyAssets;
		}

		// If asset is reloaded, we add to array for broadcast (only in editor)
#if WITH_EDITOR
		if ( GIsEditor && bLocalResult )
		{
			reloadedAssets.push_back( assetInfo.data );
		}
#endif // WITH_EDITOR
	}
	delete archive;

	// Broadcast event of reloaded assets
#if WITH_EDITOR
	if ( GIsEditor )
	{
		FEditorDelegates::onAssetsReloaded.Broadcast( reloadedAssets );
	}
#endif // WITH_EDITOR

	bIsDirty		= numNewDirtyAssets > 0;
	numDirtyAssets	= numNewDirtyAssets;
	return !bNotAllAssetLoaded;
}

FPackageManager::FPackageManager()
{}

void FPackageManager::Init()
{}

void FPackageManager::Tick()
{}

void FPackageManager::Shutdown()
{}

bool ParseReferenceToAsset( const std::wstring& InString, std::wstring& OutPackageName, std::wstring& OutAssetName, EAssetType& OutAssetType )
{
	// If string is empty, we nothing do
	if ( InString.empty() )
	{
		return false;
	}

	// Divide the string into three parts: asset type, package name and asset name
	uint32				offset						= 0;
	std::size_t			posSpliterType				= InString.find( TEXT( "'" ) );
	std::size_t			posSpliterPackageAndAsset	= InString.find( TEXT( ":" ) );
	uint32				packageNameSize				= posSpliterPackageAndAsset - posSpliterType - 1;
	uint32				assetNameSize				= InString.size() - posSpliterPackageAndAsset - 1;
	if ( posSpliterType == std::wstring::npos || posSpliterPackageAndAsset == std::wstring::npos || packageNameSize <= 0 || assetNameSize <= 0 )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "Not correct input string '%s', reference to asset must be splitted by '<Asset type>'<Package name>:<Asset name>'" ), InString.c_str() );
		return false;
	}

	// Getting asset type
	std::wstring		assetType;
	assetType.resize( posSpliterType );
	memcpy( assetType.data(), &InString[ 0 ], sizeof( std::wstring::value_type ) * assetType.size() );
	offset = assetType.size() + 1;				// +1 for skip splitter of type asset
	OutAssetType = ConvertTextToAssetType( assetType );

	// Getting package name
	OutPackageName.resize( packageNameSize );
	memcpy( OutPackageName.data(), &InString[ offset ], sizeof( std::wstring::value_type ) * packageNameSize );
	offset += packageNameSize + 1;				// +1 for skip splitter of asset and package

	// Getting asset name
	OutAssetName.resize( assetNameSize );
	memcpy( OutAssetName.data(), &InString[ offset ], sizeof( std::wstring::value_type ) * assetNameSize );

	return true;
}

TAssetHandle<FAsset> FPackageManager::FindAsset( const std::wstring& InString, EAssetType InType /* = AT_Unknown */ )
{
	std::wstring		packageName;
	std::wstring		assetName;
	EAssetType			assetType;
	if ( !ParseReferenceToAsset( InString, packageName, assetName, assetType ) || ( InType != AT_Unknown && assetType != InType ) )
	{
		return GetDefaultAsset( InType );
	}

	// Find in TOC path to the package and calculate hash of asset from him name
	std::wstring	packagePath = GTableOfContents.GetPackagePath( packageName );
	if ( packagePath.empty() )
	{
		if ( !GIsCooker )
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "Package with name '%s' not found in TOC file" ), packageName.c_str() );
		}
		return GetDefaultAsset( InType );
	}

	return FindAsset( packagePath, assetName, InType );
}

TAssetHandle<FAsset> FPackageManager::FindAsset( const std::wstring& InPath, const FGuid& InGUIDAsset, EAssetType InType /* = AT_Unknown */ )
{
	check( InGUIDAsset.IsValid() );

	// Find package and open he
	TAssetHandle<FAsset>	asset;
	FPackageRef				package = LoadPackage( InPath );

	// Find asset in package
	if ( package )
	{
		asset = package->Find( InGUIDAsset );
	}

	// If asset is not valid, we return default
	if ( !asset.IsAssetValid() )
	{
		asset = GetDefaultAsset( InType );
	}

	return asset;
}

TAssetHandle<FAsset> FPackageManager::FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType /* = AT_Unknown */ )
{
	check( !InAsset.empty() );

	// Find package and open he
	TAssetHandle<FAsset>	asset;
	FPackageRef				package = LoadPackage( InPath );
	
	// Find asset in package
	if ( package )
	{
		asset = package->Find( InAsset );
		if ( !asset.IsAssetValid() )
		{
			asset = GetDefaultAsset( InType );
		}
	}

	// If asset is not valid, we return default
	if ( !asset.IsAssetValid() )
	{
		asset = GetDefaultAsset( InType );
	}

	return asset;
}

TAssetHandle<FAsset> FPackageManager::FindDefaultAsset( EAssetType InType ) const
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
			packages[ InPath ] = package;
			LE_LOG( LT_Log, LC_Package, TEXT( "Package '%s' opened" ), InPath.c_str() );
			
			package->SetNameFromPath( InPath );
			GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), InPath );
		}
	}
	else
	{
		package = itPackage->second;
	}

	if ( !package )
	{
		LE_LOG( LT_Warning, LC_Package, TEXT( "Package '%s' not found" ), InPath.c_str() );
	}
	return package;
}

bool FPackageManager::UnloadPackage( const std::wstring& InPath, bool InForceUnload /* = false */ )
{
	auto		itPackage = packages.find( InPath );
	if ( itPackage == packages.end() )
	{
		return false;
	}

	// Unload all assets, if any of them was not unloaded or is dirty package, exit from the function
	if ( itPackage->second->IsDirty() || !itPackage->second->UnloadAllAssets( InForceUnload ) )
	{
		return false;
	}

	// If package in using, we not unload him
	if ( itPackage->second->GetNumLoadedAssets() > 0 )
	{
		return false;
	}

	packages.erase( itPackage );

	LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded package '%s'" ), InPath.c_str() );
	return true;
}

bool FPackageManager::UnloadAllPackages( bool InForceUnload /* = false */ )
{
	bool	bUnloadedNotAll = false;
	for ( auto itPackage = packages.begin(); itPackage != packages.end(); )
	{
		// Unload all assets, if any of them was not unloaded or is dirty package - we not remove current package
		if ( itPackage->second->IsDirty() || !itPackage->second->UnloadAllAssets( InForceUnload ) )
		{
			bUnloadedNotAll = true;
			++itPackage;
			continue;
		}

		// If package in using, we not unload him
		if ( itPackage->second->GetNumLoadedAssets() > 0 )
		{
			bUnloadedNotAll = true;
			++itPackage;
			continue;
		}

		LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded package '%s'" ), itPackage->first.ToString().c_str() );
		itPackage = packages.erase( itPackage );
	}

	return !bUnloadedNotAll;
}

bool FPackageManager::UnloadAsset( const TAssetHandle<FAsset>& InAssetPtr, bool InForceUnload /* = false */ )
{
	FPackageRef		package;
	FGuid			guidAsset;
	{
		// If asset already unload, we exit from function
		TSharedPtr<FAsset>		assetRef = InAssetPtr.ToSharedPtr();
		if ( !assetRef )
		{
			return true;
		}

		// Getting from asset him package and guid
		package		= assetRef->GetPackage();
		guidAsset	= assetRef->GetGUID();
		check( package );
	}

	// Try unload asset in package
	return package->UnloadAsset( guidAsset, InForceUnload );
}

bool FPackageManager::ReloadAsset( const TAssetHandle<FAsset>& InAssetPtr )
{
	// If asset handle is not valid, we exit from function
	if ( !InAssetPtr.IsValid() )
	{
		return false;
	}

	FPackageRef		package;
	FGuid			guidAsset;
	{
		// If asset is not loaded, we exit
		TSharedPtr<FAsset>		assetRef = InAssetPtr.ToSharedPtr();
		if ( !assetRef )
		{
			return false;
		}

		// Getting from asset him package and guid
		package		= assetRef->GetPackage();
		guidAsset	= assetRef->GetGUID();
		check( package );
	}

	// Reload asset from package
	return package->ReloadAsset( guidAsset );
}

bool FPackageManager::ReloadPackage( const std::wstring& InPath )
{
	auto		itPackage = packages.find( InPath );
	if ( itPackage == packages.end() )
	{
		return false;
	}

	// Reload all assets from package
	bool		bResult = itPackage->second->ReloadPackage();
	if ( bResult )
	{
		LE_LOG( LT_Log, LC_Package, TEXT( "Reloaded package '%s'" ), itPackage->first.ToString().c_str() );
	}

	return bResult;
}

bool FPackageManager::ReloadAllPackages()
{
	bool	bResult = false;
	for ( auto itPackage = packages.begin(); itPackage != packages.end(); )
	{
		// Reload all assets in package
		bool	bLocalResult = itPackage->second->ReloadPackage();
		bResult |= bLocalResult;

		if ( bLocalResult )
		{
			LE_LOG( LT_Log, LC_Package, TEXT( "Reloaded package '%s'" ), itPackage->first.ToString().c_str() );
		}
	}

	return bResult;
}

void FPackageManager::GarbageCollector()
{
	double		startGCTime = appSeconds();
	LE_LOG( LT_Log, LC_Package, TEXT( "Collecting garbage of packages" ) );

	uint32															numUnloadedAssets	= 0;
	uint32															numUnloadedPackages = 0;
	std::unordered_set< FPackageRef, FPackageRef::FHashFunction >	packagesToUnload;

	// We go through all the packages and unload unused assets
	LE_LOG( LT_Log, LC_Package, TEXT( " Assets:" ) );
	for ( auto itPackage = packages.begin(), itPackageEnd = packages.end(); itPackage != itPackageEnd; ++itPackage )
	{
		FPackageRef		package		= itPackage->second;
		uint32			numAssets	= package->GetNumAssets();		
		for ( uint32 index = 0; index < numAssets; ++index )
		{
			FAssetInfo		assetInfo;
			package->GetAssetInfo( index, assetInfo );

			// Skip asset if him is not loaded
			if ( !assetInfo.data )
			{
				continue;
			}

			// Unload asset only if weak references is not exist
			bool		bCanUnloadAsset = assetInfo.data.GetSharedReferenceCount() <= 2 && assetInfo.data.GetWeakReferenceCount() <= 3;		// 2 shared reference this is two FAssetInfo, one in current section, other in package
																																			// 3 weak references containing in SharedThis, GetAssetHandle and self this resource
			if ( bCanUnloadAsset && package->UnloadAsset( assetInfo.data->GetGUID(), true ) )
			{
				// Try unload dependent assets
				FAsset::FSetDependentAssets		dependentAssets;
				assetInfo.data->GetDependentAssets( dependentAssets );

				for ( auto itDependentAsset = dependentAssets.begin(), itDependentAssetEnd = dependentAssets.end(); itDependentAsset != itDependentAssetEnd; ++itDependentAsset )
				{
					// If depended asset already unloaded - skip it
					TSharedPtr<FAsset>		dependetAsset = itDependentAsset->ToSharedPtr();
					if ( !dependetAsset )
					{
						continue;
					}

					// Is we can unload this asset?
					bool	bCanUnloadDependentAsset = dependetAsset.GetSharedReferenceCount() <= 2 && dependetAsset.GetWeakReferenceCount() <= 5;		// Shared reference: 2 because one in current section, other in package
																																						// Weak reference: 5 because one in current section (FSetDependentAssets), second in parent asset and other self this resource, SharedThis and GetAssetHandle
					if ( !bCanUnloadDependentAsset )
					{
						continue;
					}

					// If in asset package is not exist - skip it
					FPackageRef		dependetPackage = dependetAsset->GetPackage();
					if ( !dependetPackage )
					{
						continue;
					}

					// If we succeed unload this asset then increment counter
					if ( dependetPackage->UnloadAsset( dependetAsset->GetGUID(), true ) )
					{
						LE_LOG( LT_Log, LC_Package, TEXT( "  Asset '%s:%s' unloaded" ), dependetPackage->GetName().c_str(), dependetAsset->GetAssetName().c_str() );
						++numUnloadedAssets;
					}

					// If unloaded all assets we must remove this package
					if ( dependetPackage->GetNumLoadedAssets() <= 0 )
					{
						packagesToUnload.insert( dependetPackage );
					}
				}

				LE_LOG( LT_Log, LC_Package, TEXT( "  Asset '%s:%s' unloaded" ), package->GetName().c_str(), assetInfo.name.c_str() );
				++numUnloadedAssets;
			}
		}

		// If unloaded all assets we must remove this package
		if ( package->GetNumLoadedAssets() <= 0 )
		{
			packagesToUnload.insert( itPackage->second );
		}
	}

	// Unload all not used packages
	numUnloadedPackages		= packagesToUnload.size();
	LE_LOG( LT_Log, LC_Package, TEXT( " Packages:" ) );
	
	for ( auto itPackage = packagesToUnload.begin(), itPackageEnd = packagesToUnload.end(); itPackage != itPackageEnd; ++itPackage )
	{
		FPackageRef		package = *itPackage;
		
		LE_LOG( LT_Log, LC_Package, TEXT( "  Package '%s' unloaded" ), package->GetName().c_str() );
		packages.erase( package->GetFileName() );
	}

	double		endGCTime = appSeconds();
	LE_LOG( LT_Log, LC_Package, TEXT( "Unloaded %i assets and %i packages" ), numUnloadedAssets, numUnloadedPackages );
	LE_LOG( LT_Log, LC_Package, TEXT( "%f ms for realtime GC" ), ( endGCTime - startGCTime ) / 1000.f );
}