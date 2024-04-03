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

//
// ASSET
//

/*
==================
CAsset::CAsset
==================
*/
CAsset::CAsset( EAssetType InType ) 
	: bDirty( true )		// by default package is dirty because not serialized package from HDD
	, package( nullptr )
	, guid( Sys_CreateGuid() )
	, type( InType )

#if WITH_EDITOR
	, bOnlyEditor( false )
#endif // WITH_EDITOR
{}

/*
==================
CAsset::~CAsset
==================
*/
CAsset::~CAsset()
{}

/*
==================
CAsset::Serialize
==================
*/
void CAsset::Serialize( class CArchive& InArchive )
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

	if ( InArchive.Ver() >= VER_AssetOnlyEditor )
	{
#if !WITH_EDITOR
		bool		bOnlyEditor;
#endif // !WITH_EDITOR
		InArchive << bOnlyEditor;
	}

	bDirty = false;
}

/*
==================
CAsset::SetAssetName
==================
*/
void CAsset::SetAssetName( const std::wstring& InName )
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

/*
==================
CAsset::MarkDirty
==================
*/
void CAsset::MarkDirty()
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

/*
==================
CAsset::OnFullyLoad
==================
*/
void CAsset::OnFullyLoad()
{}

/*
==================
CAsset::GetDependentAssets
==================
*/
void CAsset::GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter /* = AT_Unknown */ ) const
{}

/*
==================
CAsset::ReloadDependentAssets
==================
*/
void CAsset::ReloadDependentAssets( bool InForce /* = false */ )
{}

/*
==================
CAsset::GetAssetReference
==================
*/
AssetReference CAsset::GetAssetReference() const
{
	return AssetReference( type, guid, package ? package->GetGUID() : CGuid() );
}

/*
==================
CAsset::GetAssetHandle
==================
*/
TAssetHandle<CAsset> CAsset::GetAssetHandle() const
{
	if ( !handle.IsValid() )
	{
		handle = TAssetHandle<CAsset>( SharedThis( this ), MakeSharedPtr<AssetReference>( type, guid, package ? package->GetGUID() : CGuid() ) );
	}
	return handle;
}

//
// ASSET FACTORY
//

/*
==================
CAssetFactory::CAssetFactory
==================
*/
CAssetFactory::CAssetFactory()
{
	Memory::Memzero( constructAssetFns, AT_Count * sizeof( CAssetFactory::ConstructAssetFn_t ) );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CTexture2D>(); },			AT_Texture2D );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CMaterial>(); },			AT_Material );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CScript>(); },				AT_Script );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CStaticMesh>(); },			AT_StaticMesh );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CAudioBank>(); },			AT_AudioBank );
	Register( []() -> TSharedPtr<CAsset> { return MakeSharedPtr<CPhysicsMaterial>(); },		AT_PhysicsMaterial );
}

#if WITH_EDITOR
/*
==================
CAssetFactory::ShowImportSettings
==================
*/
bool CAssetFactory::ShowImportSettings( EAssetType InAssetType, class CImGUILayer* InOwner, CEvent& InEvent, EResultShowImportSettings& OutResult ) const
{
	Assert( InOwner );
	const AssetImporterInfo&	importerInfo = importersInfo[InAssetType];
	if ( importerInfo.bValid && importerInfo.showImportSettingsAssetFn )
	{
		importerInfo.showImportSettingsAssetFn( InOwner, InEvent, OutResult );
		return true;
	}

	return false;
}

/*
==================
CAssetFactory::Import
==================
*/
bool CAssetFactory::Import( const std::wstring& InPath, std::vector<TSharedPtr<CAsset>>& OutResult, std::wstring& OutError ) const
{
	EAssetType		assetType = GetAssetTypeByPath( InPath );
	if ( assetType == AT_Unknown )
	{
		OutError = L_Sprintf( TEXT( "Not exist importer for extension '%s'" ), CFilename( InPath ).GetExtension().c_str() );
		return nullptr;
	}

	const AssetImporterInfo&	importerInfo = importersInfo[assetType];
	Assert( importerInfo.bValid && importerInfo.importAssetFn );
	return importerInfo.importAssetFn( InPath, OutResult, OutError );
}

/*
==================
CAssetFactory::Reimport
==================
*/
bool CAssetFactory::Reimport( const TSharedPtr<CAsset>& InAsset, std::wstring& OutError ) const
{
	Assert( InAsset );
	const AssetImporterInfo&	importerInfo = importersInfo[InAsset->GetType()];
	
	if ( !importerInfo.bValid )
	{
		OutError = L_Sprintf( TEXT( "Not exist importer for asset type 0x%X" ), InAsset->GetType() );
		return false;
	}

	return importerInfo.reimportAssetFn( InAsset, OutError );
}

/*
==================
CAssetFactory::GetAssetTypeByPath
==================
*/
EAssetType CAssetFactory::GetAssetTypeByPath( const std::wstring& InPath ) const
{
	std::wstring		fileExtension = CFilename( InPath ).GetExtension();
	L_Strupr( fileExtension, fileExtension );

	for ( uint32 index = 0; index < AT_Count; ++index )
	{
		const AssetImporterInfo&	importerInfo = importersInfo[index];
		if ( importerInfo.bValid )
		{
			bool	bSupportedFile = false;
			for ( uint32 extensionId = 0, countExtensions = importerInfo.supportedExtensions.size(); extensionId < countExtensions; ++extensionId )
			{
				if ( L_Strupr( importerInfo.supportedExtensions[extensionId] ) == fileExtension )
				{
					bSupportedFile = true;
					break;
				}
			}

			if ( bSupportedFile )
			{
				return ( EAssetType )index;
			}
		}
	}

	return AT_Unknown;
}
#endif // WITH_EDITOR

//
// PACKAGE
//

/*
==================
CPackage::CPackage
==================
*/
CPackage::CPackage( const std::wstring& InName /* = TEXT( "" ) */ ) 
	: bIsDirty( true )			// by default package is dirty because not serialized package from HDD
	, guid( Sys_CreateGuid() )
	, name( InName )
	, numLoadedAssets( 0 )
	, numDirtyAssets( 0 )
{}

/*
==================
CPackage::~CPackage
==================
*/
CPackage::~CPackage()
{
	RemoveAll( true );
}

/*
==================
CPackage::Load
==================
*/
bool CPackage::Load( const std::wstring& InPath )
{
	RemoveAll( true );

	CArchive*		archive = g_FileSystem->CreateFileReader( InPath );
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

/*
==================
CPackage::SetNameFromPath
==================
*/
void CPackage::SetNameFromPath( const std::wstring& InPath )
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

/*
==================
CPackage::Save
==================
*/
bool CPackage::Save( const std::wstring& InPath )
{
	// Before saving package it needs to be fully loaded into memory
	std::vector< TAssetHandle<CAsset> >		loadedAsset;
	FullyLoad( loadedAsset );

	// If package name not setted - take from path name of file
	if ( name.empty() )
	{
		SetNameFromPath( InPath );
	}

	CArchive*		archive = g_FileSystem->CreateFileWriter( InPath );
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

/*
==================
CPackage::FullyLoad
==================
*/
void CPackage::FullyLoad( std::vector< TAssetHandle<CAsset> >& OutAssetArray )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return;
	}

	// Serialize all assets to memory
	CArchive*		archive = g_FileSystem->CreateFileReader( filename, AR_NoFail );
	archive->SerializeHeader();
	SerializeHeader( *archive, true );

	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		AssetInfo&		assetInfo = itAsset->second;

		// If asset info is not valid - return nullptr
		if ( assetInfo.offset == ( uint32 )INVALID_ID || assetInfo.size == ( uint32 )INVALID_ID )
		{
			continue;
		}

		// Load asset
		TAssetHandle<CAsset>		asset = LoadAsset( *archive, itAsset->first, assetInfo );
		if ( asset.IsAssetValid() )
		{
			asset.ToSharedPtr()->OnFullyLoad();
			OutAssetArray.push_back( asset );
		}
		else
		{
			Warnf( TEXT( "Asset '%s' not loaded\n" ), assetInfo.name.c_str() );
		}
	}

	delete archive;
}

/*
==================
CPackage::Find
==================
*/
TAssetHandle<CAsset> CPackage::Find( const CGuid& InGUID )
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
	CArchive*	archive = g_FileSystem->CreateFileReader( filename );
	if ( !archive )
	{
		return nullptr;
	}

	archive->SerializeHeader();
	SerializeHeader( *archive );
	TAssetHandle<CAsset>		asset = LoadAsset( *archive, itAsset->first, itAsset->second );

	delete archive;
	return asset;
}

/*
==================
CPackage::Serialize
==================
*/
void CPackage::Serialize( CArchive& InArchive )
{
	SerializeHeader( InArchive );
	Assert( InArchive.Ver() >= VER_Assets );

	if ( InArchive.IsSaving() )
	{
		for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
		{
			AssetInfo&			assetInfo = itAsset->second;
			if ( !assetInfo.data )
			{
				Warnf( TEXT( "Asset '%s' is not valid, skiped saving to package\n" ), assetInfo.name.c_str() );
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
			AssetInfo			localAssetInfo;
			CGuid				assetGUID;
			Memory::Memzero( &localAssetInfo, sizeof( AssetInfo ) );

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
			AssetInfo&			assetInfo			= assetsTable[ assetGUID ];
			localAssetInfo.data						= assetInfo.data;
			assetGUIDTable[ localAssetInfo.name ]	= assetGUID;
			
			// If the name of the asset in the table cache is different, you need to update the table GUID
			if ( localAssetInfo.name != assetInfo.name )
			{
				assetGUIDTable.erase( assetInfo.name );
			}

			assetInfo			= localAssetInfo;
		}
	}

	bIsDirty		= false;
	numDirtyAssets	= 0;
}

/*
==================
CPackage::SerializeHeader
==================
*/
void CPackage::SerializeHeader( CArchive& InArchive, bool InIsNeedSkip /* = false */ )
{
	Assert( InArchive.Ver() >= VER_NamePackage );
	uint32		packageFileTag = PACKAGE_FILE_TAG;
	InArchive << packageFileTag;
	AssertMsg( packageFileTag == PACKAGE_FILE_TAG, TEXT( "Unknown package file tag. Current package file tag is 0x%X, need 0x%X" ), packageFileTag, PACKAGE_FILE_TAG );

#if ENABLED_ASSERT
	if ( InArchive.IsSaving() )
	{
		AssertMsg( !name.empty(), TEXT( "The package must have a name" ) );
	}
#endif // ENABLED_ASSERT

	if ( !InIsNeedSkip )
	{
		InArchive << guid;
		InArchive << name;
	}
	else
	{
		CGuid			tmpGuid;
		std::wstring	tmpName;
		InArchive << tmpGuid;
		InArchive << tmpName;
	}
}

/*
==================
CPackage::LoadAsset
==================
*/
TAssetHandle<CAsset> CPackage::LoadAsset( CArchive& InArchive, const CGuid& InAssetGUID, AssetInfo& InAssetInfo, bool InNeedReload /* = false */ )
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
		InAssetInfo.data			= g_AssetFactory.Create( InAssetInfo.type );
		Assert( InAssetInfo.data );

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

	Assert( currentOffset - startOffset == InAssetInfo.size );

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

/*
==================
CPackage::MarkAssetDirty
==================
*/
void CPackage::MarkAssetDirty( const CGuid& InGUID )
{
	bIsDirty = true;
	if ( numDirtyAssets != -1 )
	{
		++numDirtyAssets;
	}
}

/*
==================
CPackage::UpdateAssetNameInTable
==================
*/
void CPackage::UpdateAssetNameInTable( const CGuid& InGUID )
{
	// Find asset in package
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return;
	}

	AssetInfo&		assetInfo = itAsset->second;
	Assert( assetInfo.data );

	// Remove from GUID table old name
	assetGUIDTable.erase( assetInfo.name );

	// Add to GUID table new name and update asset info
	assetGUIDTable[ assetInfo.data->name ] = InGUID;
	assetInfo.name = assetInfo.data->name;
}

/*
==================
CPackage::Add
==================
*/
void CPackage::Add( const TAssetHandle<CAsset>& InAsset, AssetInfo* OutAssetInfo /* = nullptr */ )
{
	Assert( InAsset.IsAssetValid() );

	TSharedPtr<CAsset>		assetRef = InAsset.ToSharedPtr();
	AssertMsg( assetRef->guid.IsValid(), TEXT( "For add asset to package need GUID is valid" ) );

	// If asset in package already containing, remove from table old GUID
	auto		it = assetGUIDTable.find( assetRef->name );
	if ( it != assetGUIDTable.end() && it->second != assetRef->guid )
	{
		assetsTable.erase( it->second );
	}

	// Update guid package in asset reference
	InAsset.reference->guidPackage		= guid;

	AssetInfo		assetInfo{ ( uint32 )INVALID_ID, ( uint32 )INVALID_ID, assetRef->type, assetRef->name, assetRef };
	if ( OutAssetInfo )
	{
		*OutAssetInfo = assetInfo;
	}

	bIsDirty							= true;
	assetRef->package					= this;
	assetRef->bDirty					= true;
	assetGUIDTable[ assetRef->name ]	= assetRef->guid;
	assetsTable[ assetRef->guid ]		= assetInfo;
	
	++numLoadedAssets;
	++numDirtyAssets;
}

/*
==================
CPackage::Add
==================
*/
void CPackage::Add( CObject* InObject, ObjectInfo* OutObjectInfo /* = nullptr */ )
{
	Assert( InObject );


}

/*
==================
CPackage::Remove
==================
*/
bool CPackage::Remove( const CGuid& InGUID, bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	auto		itAsset = assetsTable.find( InGUID );
	if ( itAsset == assetsTable.end() )
	{
		return true;
	}

	// Unload asset, if failed we exit from method
	AssetInfo&				assetInfo		= itAsset->second;
	TWeakPtr<CAsset>		assetPtr		= assetInfo.data;
	bool					bIsExistOnHDD	= assetInfo.offset != INVALID_ID && assetInfo.size != INVALID_ID;		// Is asset containing in package on HDD?
	if ( assetInfo.data && !UnloadAsset( assetInfo, InForceUnload, true, InIgnoreDirty ) )
	{
		return false;
	}

	// If the asset remains in memory, then in this case we reset its link to this package
	{
		TSharedPtr<CAsset>		assetRef = assetPtr.Pin();
		if ( assetRef )
		{
			assetRef->package = nullptr;
			assetRef->GetAssetHandle().GetReference()->guidPackage.Invalidate();
		}
	}

	// We remove her asset only if him exist on HDD. Otherwise, deletion from these tables occurs in UnloadAsset
	if ( bIsExistOnHDD )
	{
		auto		itAssetGUID = assetGUIDTable.find( assetInfo.name );
		if ( itAssetGUID != assetGUIDTable.end() )
		{
			assetGUIDTable.erase( itAssetGUID );
		}

		assetsTable.erase( itAsset );
		bIsDirty		= true;
		numDirtyAssets	= -1;
	}

	return true;
}

/*
==================
CPackage::RemoveAll
==================
*/
bool CPackage::RemoveAll( bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	// Fill array of assets to unload. Need it for remove reference to this package if him remained in memory
	std::vector< TWeakPtr<CAsset> >		assetsToUnload;
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		AssetInfo&		assetInfo = itAsset->second;
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
		TSharedPtr<CAsset>		assetRef = assetsToUnload[ index ].Pin();
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

/*
==================
CPackage::UnloadAsset
==================
*/
bool CPackage::UnloadAsset( AssetInfo& InAssetInfo, bool InForceUnload /* = false */, bool InBroadcastEvent /* = true */, bool InIgnoreDirty /* = false */ )
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
		if ( g_IsEditor && InBroadcastEvent )
		{
			CanDeleteAssetResult					result;
			std::vector< TSharedPtr<CAsset>	>		assets = { InAssetInfo.data };
			EditorDelegates::onAssetsCanDelete.Broadcast( assets, result );

			// If this asset is blocked, we exit from method
			if ( !result.Get() )
			{
				return false;
			}

			// Else we tell all what asset is deleted
			EditorDelegates::onAssetsDeleted.Broadcast( assets );
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
			Warnf( TEXT( "An asset '%s' was uploaded that was not recorded on the HDD. This asset has been removed from the package and will not be written\n" ), InAssetInfo.name.c_str() );
			InAssetInfo.data->package = nullptr;
			InAssetInfo.data->GetAssetHandle().GetReference()->guidPackage.Invalidate();
			
			auto		itAssetGUID = assetGUIDTable.find( InAssetInfo.name );
			assetsTable.erase( InAssetInfo.data->guid );
			if ( itAssetGUID != assetGUIDTable.end() )
			{
				assetGUIDTable.erase( itAssetGUID );
			}
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

/*
==================
CPackage::UnloadAllAssetsInternal
==================
*/
bool CPackage::UnloadAllAssetsInternal( bool InForceUnload /* = false */, bool InIgnoreDirty /* = false */ )
{
	bool		bExistDirtyAssets = false;
	std::vector<AssetInfo*>		assetsToUnload;

	// Fill array assets to unload
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		AssetInfo&		assetInfo = itAsset->second;

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
	if ( g_IsEditor )
	{
		// Convert array of AssetInfo to TSharedPtr<CAsset> for events
		std::vector< TSharedPtr<CAsset> >		assets;
		for ( uint32 index = 0, count = assetsToUnload.size(); index < count; ++index )
		{
			assets.push_back( assetsToUnload[ index ]->data );
		}

		// Tell all user of we want delete this assets 
		CanDeleteAssetResult		result;
		EditorDelegates::onAssetsCanDelete.Broadcast( assets, result );

		// If this assets is blocked, we exit from method
		if ( !result.Get() )
		{
			return false;
		}

		// Else we tell all what asset is deleted
		EditorDelegates::onAssetsDeleted.Broadcast( assets );
	}
#endif // WITH_EDITOR

	// Unload all assets
	for ( uint32 index = 0, count = assetsToUnload.size(); index < count; ++index )
	{
		bExistDirtyAssets |= !UnloadAsset( *assetsToUnload[ index ], InForceUnload, false, InIgnoreDirty );
	}

	return !bExistDirtyAssets;
}

/*
==================
CPackage::ReloadAsset
==================
*/
bool CPackage::ReloadAsset( AssetInfo& InAssetInfo )
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
	CArchive*		archive = g_FileSystem->CreateFileReader( filename );
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
	if ( g_IsEditor )
	{
		std::vector< TSharedPtr<CAsset>	>		assets = { InAssetInfo.data };
		EditorDelegates::onAssetsReloaded.Broadcast( assets );
	}
#endif // WITH_EDITOR

	return bResult;
}

/*
==================
CPackage::ReloadPackage
==================
*/
bool CPackage::ReloadPackage( bool InOnlyAsset /* = false */ )
{
	// If we not load package from HDD - exit from function
	if ( filename.empty() )
	{
		return false;
	}

	// Open package for reload asset
	CArchive*		archive = g_FileSystem->CreateFileReader( filename );
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
	std::vector< TSharedPtr<CAsset> >	reloadedAssets;
#endif // WITH_EDITOR

	bool		bNotAllAssetLoaded = false;
	uint32		numNewDirtyAssets = 0;
	for ( auto itAsset = assetsTable.begin(), itAssetEnd = assetsTable.end(); itAsset != itAssetEnd; ++itAsset )
	{
		AssetInfo&		assetInfo = itAsset->second;

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
		if ( g_IsEditor && bLocalResult )
		{
			reloadedAssets.push_back( assetInfo.data );
		}
#endif // WITH_EDITOR
	}
	delete archive;

	// Broadcast event of reloaded assets
#if WITH_EDITOR
	if ( g_IsEditor )
	{
		EditorDelegates::onAssetsReloaded.Broadcast( reloadedAssets );
	}
#endif // WITH_EDITOR

	bIsDirty		= numNewDirtyAssets > 0;
	numDirtyAssets	= numNewDirtyAssets;
	return !bNotAllAssetLoaded;
}

//
// PACKAGE MANAGER
//

/*
==================
CPackageManager::CPackageManager
==================
*/
CPackageManager::CPackageManager()
{}

/*
==================
CPackageManager::Init
==================
*/
void CPackageManager::Init()
{}

/*
==================
CPackageManager::Tick
==================
*/
void CPackageManager::Tick()
{}

/*
==================
CPackageManager::Shutdown
==================
*/
void CPackageManager::Shutdown()
{}

/*
==================
ParseReferenceToAsset
==================
*/
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
		Warnf( TEXT( "Not correct input string '%s', reference to asset must be splitted by '<Asset type>'<Package name>:<Asset name>'\n" ), InString.c_str() );
		return false;
	}

	// Getting asset type
	std::wstring		assetType;
	assetType.resize( posSpliterType );
	Memory::Memcpy( assetType.data(), &InString[ 0 ], sizeof( std::wstring::value_type ) * assetType.size() );
	offset = assetType.size() + 1;				// +1 for skip splitter of type asset
	OutAssetType = ConvertTextToAssetType( assetType );

	// Getting package name
	OutPackageName.resize( packageNameSize );
	Memory::Memcpy( OutPackageName.data(), &InString[ offset ], sizeof( std::wstring::value_type ) * packageNameSize );
	offset += packageNameSize + 1;				// +1 for skip splitter of asset and package

	// Getting asset name
	OutAssetName.resize( assetNameSize );
	Memory::Memcpy( OutAssetName.data(), &InString[ offset ], sizeof( std::wstring::value_type ) * assetNameSize );

	return true;
}

/*
==================
CPackageManager::FindAsset
==================
*/
TAssetHandle<CAsset> CPackageManager::FindAsset( const std::wstring& InString, EAssetType InType /* = AT_Unknown */ )
{
	std::wstring		packageName;
	std::wstring		assetName;
	EAssetType			assetType;
	if ( !ParseReferenceToAsset( InString, packageName, assetName, assetType ) || ( InType != AT_Unknown && assetType != InType ) )
	{
		return g_AssetFactory.GetDefault( InType );
	}

	// Find in TOC path to the package and calculate hash of asset from him name
	std::wstring	packagePath = g_TableOfContents.GetPackagePath( packageName );
	if ( packagePath.empty() )
	{
		if ( !g_IsCooker )
		{
			Warnf( TEXT( "Package with name '%s' not found in TOC file\n" ), packageName.c_str() );
		}
		return g_AssetFactory.GetDefault( InType );
	}

	return FindAsset( packagePath, assetName, InType );
}

/*
==================
CPackageManager::FindAsset
==================
*/
TAssetHandle<CAsset> CPackageManager::FindAsset( const std::wstring& InPath, const CGuid& InGUIDAsset, EAssetType InType /* = AT_Unknown */ )
{
	Assert( InGUIDAsset.IsValid() );

	// Find package and open he
	TAssetHandle<CAsset>	asset;
	PackageRef_t				package = LoadPackage( InPath );

	// Find asset in package
	if ( package )
	{
		asset = package->Find( InGUIDAsset );
	}

	// If asset is not valid, we return default
	if ( !asset.IsAssetValid() )
	{
		asset = g_AssetFactory.GetDefault( InType );
	}

	return asset;
}

/*
==================
CPackageManager::FindAsset
==================
*/
TAssetHandle<CAsset> CPackageManager::FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType /* = AT_Unknown */ )
{
	Assert( !InAsset.empty() );

	// Find package and open he
	TAssetHandle<CAsset>	asset;
	PackageRef_t				package = LoadPackage( InPath );
	
	// Find asset in package
	if ( package )
	{
		asset = package->Find( InAsset );
		if ( !asset.IsAssetValid() )
		{
			asset = g_AssetFactory.GetDefault( InType );
		}
	}

	// If asset is not valid, we return default
	if ( !asset.IsAssetValid() )
	{
		asset = g_AssetFactory.GetDefault( InType );
	}

	return asset;
}

/*
==================
CPackageManager::FindDefaultAsset
==================
*/
TAssetHandle<CAsset> CPackageManager::FindDefaultAsset( EAssetType InType ) const
{
	return g_AssetFactory.GetDefault( InType );
}

/*
==================
CPackageManager::LoadPackage
==================
*/
PackageRef_t CPackageManager::LoadPackage( const std::wstring& InPath, bool InCreateIfNotExist /*= false */ )
{
	PackageRef_t			package;
	auto				itPackage = packages.find( InPath );
	
	if ( itPackage == packages.end() )
	{
		package = new CPackage();
		if ( !package->Load( InPath ) && !InCreateIfNotExist )
		{
			package = nullptr;
		}
		else
		{
			packages[ InPath ] = package;
			Logf( TEXT( "Package '%s' opened\n" ), InPath.c_str() );
			
			package->SetNameFromPath( InPath );

			// If package is not virtual, we add entry to TOC
			if ( !InPath.empty() )
			{
				g_TableOfContents.AddEntry( package->GetGUID(), package->GetName(), InPath );
			}
		}
	}
	else
	{
		package = itPackage->second;
	}

	if ( !package )
	{
		Warnf( TEXT( "Package '%s' not found\n" ), InPath.c_str() );
	}
	return package;
}

/*
==================
CPackageManager::UnloadPackage
==================
*/
bool CPackageManager::UnloadPackage( const std::wstring& InPath, bool InForceUnload /* = false */ )
{
	auto		itPackage = packages.find( InPath );
	if ( itPackage == packages.end() )
	{
		return true;
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

	Logf( TEXT( "Unloaded package '%s'\n" ), InPath.c_str() );
	return true;
}

/*
==================
CPackageManager::UnloadAllPackages
==================
*/
bool CPackageManager::UnloadAllPackages( bool InForceUnload /* = false */ )
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

		Logf( TEXT( "Unloaded package '%s'\n" ), itPackage->first.ToString().c_str() );
		itPackage = packages.erase( itPackage );
	}

	return !bUnloadedNotAll;
}

/*
==================
CPackageManager::UnloadAsset
==================
*/
bool CPackageManager::UnloadAsset( const TAssetHandle<CAsset>& InAssetPtr, bool InForceUnload /* = false */ )
{
	PackageRef_t		package;
	CGuid			guidAsset;
	{
		// If asset already unload, we exit from function
		TSharedPtr<CAsset>		assetRef = InAssetPtr.ToSharedPtr();
		if ( !assetRef )
		{
			return true;
		}

		// Getting from asset him package and guid
		package		= assetRef->GetPackage();
		guidAsset	= assetRef->GetGUID();
		Assert( package );
	}

	// Try unload asset in package
	return package->UnloadAsset( guidAsset, InForceUnload );
}

/*
==================
CPackageManager::ReloadAsset
==================
*/
bool CPackageManager::ReloadAsset( const TAssetHandle<CAsset>& InAssetPtr )
{
	// If asset handle is not valid, we exit from function
	if ( !InAssetPtr.IsValid() )
	{
		return false;
	}

	PackageRef_t		package;
	CGuid			guidAsset;
	{
		// If asset is not loaded, we exit
		TSharedPtr<CAsset>		assetRef = InAssetPtr.ToSharedPtr();
		if ( !assetRef )
		{
			return false;
		}

		// Getting from asset him package and guid
		package		= assetRef->GetPackage();
		guidAsset	= assetRef->GetGUID();
		Assert( package );
	}

	// Reload asset from package
	return package->ReloadAsset( guidAsset );
}

/*
==================
CPackageManager::ReloadPackage
==================
*/
bool CPackageManager::ReloadPackage( const std::wstring& InPath )
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
		Logf( TEXT( "Reloaded package '%s'\n" ), itPackage->first.ToString().c_str() );
	}

	return bResult;
}

/*
==================
CPackageManager::ReloadAllPackages
==================
*/
bool CPackageManager::ReloadAllPackages()
{
	bool	bResult = false;
	for ( auto itPackage = packages.begin(); itPackage != packages.end(); )
	{
		// Reload all assets in package
		bool	bLocalResult = itPackage->second->ReloadPackage();
		bResult |= bLocalResult;

		if ( bLocalResult )
		{
			Logf( TEXT( "Reloaded package '%s'\n" ), itPackage->first.ToString().c_str() );
		}
	}

	return bResult;
}

/*
==================
CPackageManager::GarbageCollector
==================
*/
void CPackageManager::GarbageCollector()
{
	double		startGCTime = Sys_Seconds();
	Logf( TEXT( "Collecting garbage of packages\n" ) );

	uint32															numUnloadedAssets	= 0;
	uint32															numUnloadedPackages = 0;
	std::unordered_set< PackageRef_t, PackageRef_t::HashFunction >	packagesToUnload;

	// We go through all the packages and unload unused assets
	Logf( TEXT( "Assets:\n" ) );
	for ( auto itPackage = packages.begin(), itPackageEnd = packages.end(); itPackage != itPackageEnd; ++itPackage )
	{
		PackageRef_t		package		= itPackage->second;
		uint32			numAssets	= package->GetNumAssets();		
		for ( uint32 index = 0; index < numAssets; ++index )
		{
			AssetInfo		assetInfo;
			package->GetAssetInfo( index, assetInfo );

			// Skip asset if him is not loaded
			if ( !assetInfo.data )
			{
				continue;
			}

			// Unload asset only if weak references is not exist
			bool		bCanUnloadAsset = assetInfo.data.GetSharedReferenceCount() <= 2 && assetInfo.data.GetWeakReferenceCount() <= 3;		// 2 shared reference this is two AssetInfo, one in current section, other in package
																																			// 3 weak references containing in SharedThis, GetAssetHandle and self this resource
			if ( bCanUnloadAsset && package->UnloadAsset( assetInfo.data->GetGUID(), true ) )
			{
				// Try unload dependent assets
				CAsset::SetDependentAssets_t		dependentAssets;
				CAsset::SetDependentAssets_t		reservedAssets;
				assetInfo.data->GetDependentAssets( dependentAssets );

				for ( auto itDependentAsset = dependentAssets.begin(), itDependentAssetEnd = dependentAssets.end(); itDependentAsset != itDependentAssetEnd; ++itDependentAsset )
				{
					// If depended asset already unloaded - skip it
					TSharedPtr<CAsset>		dependetAsset = itDependentAsset->ToSharedPtr();
					if ( !dependetAsset )
					{
						continue;
					}

					// Is we can unload this asset?
					bool	bCanUnloadDependentAsset = dependetAsset.GetSharedReferenceCount() <= 2 && dependetAsset.GetWeakReferenceCount() <= 5;		// Shared reference: 2 because one in current section, other in package
																																						// Weak reference: 5 because one in current section (SetDependentAssets_t), second in parent asset and other self this resource, SharedThis and GetAssetHandle
					if ( !bCanUnloadDependentAsset )
					{
						// Get dependent asset again for reserve assets of parent
						dependetAsset->GetDependentAssets( reservedAssets );
						continue;
					}

					// If in asset package is not exist - skip it
					PackageRef_t		dependetPackage = dependetAsset->GetPackage();
					if ( !dependetPackage )
					{
						continue;
					}

					// If we succeed unload this asset then increment counter
					if ( dependetPackage->UnloadAsset( dependetAsset->GetGUID(), true ) )
					{
						Logf( TEXT( "  Asset '%s:%s' unloaded\n" ), dependetPackage->GetName().c_str(), dependetAsset->GetAssetName().c_str() );
						++numUnloadedAssets;
					}

					// If unloaded all assets we must remove this package
					if ( dependetPackage->GetNumLoadedAssets() <= 0 )
					{
						packagesToUnload.insert( dependetPackage );
					}
				}

				Logf( TEXT( "  Asset '%s:%s' unloaded\n" ), package->GetName().c_str(), assetInfo.name.c_str() );
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
	Logf( TEXT( "Packages:\n" ) );
	
	for ( auto itPackage = packagesToUnload.begin(), itPackageEnd = packagesToUnload.end(); itPackage != itPackageEnd; ++itPackage )
	{
		PackageRef_t		package = *itPackage;
		
		Logf( TEXT( "  Package '%s' unloaded\n" ), package->GetName().c_str() );
		packages.erase( package->GetFileName() );
	}

	double		endGCTime = Sys_Seconds();
	Logf( TEXT( "Unloaded %i assets and %i packages\n" ), numUnloadedAssets, numUnloadedPackages );
	Logf( TEXT( "%f ms for realtime GC\n" ), ( endGCTime - startGCTime ) / 1000.f );
}