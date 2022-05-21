/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Misc/Types.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "Misc/SharedPointer.h"
#include "Misc/SharedPointerInternals.h"
#include "Misc/Misc.h"
#include "Misc/Guid.h"
#include "Misc/TableOfContents.h"
#include "Misc/CoreGlobals.h"
#include "System/Delegate.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * Reference to FPackage
 */
typedef TRefCountPtr< class FPackage >			FPackageRef;

/**
 * @ingroup Core
 * Enumeration of type asses in package
 */
enum EAssetType
{
	AT_Unknown,				/**< Unknown asset */
	AT_Texture2D,			/**< Texture 2D */
	AT_Material,			/**< Material */
	AT_Script,				/**< Script */
	AT_StaticMesh,			/**< Static mesh */
	AT_AudioBank,			/**< Audio bank */
	AT_PhysicsMaterial,		/**< Physics material */
};

/**
 * @ingroup Core
 * Convert text to asset type
 * 
 * @param InText	Asset type in text format
 * @return Return converted asset type to enumeration
 */
FORCEINLINE EAssetType ConvertTextToAssetType( const std::wstring& InText )
{
	if ( InText == TEXT( "Texture2D" ) )
	{
		return AT_Texture2D;
	}
	else if ( InText == TEXT( "Material" ) )
	{
		return AT_Material;
	}
	else if ( InText == TEXT( "Script" ) )
	{
		return AT_Script;
	}
	else if ( InText == TEXT( "StaticMesh" ) )
	{
		return AT_StaticMesh;
	}
	else if ( InText == TEXT( "AudioBank" ) )
	{
		return AT_AudioBank;
	}
	else if ( InText == TEXT( "PhysicsMaterial" ) )
	{
		return AT_PhysicsMaterial;
	}
	return AT_Unknown;
}

/**
 * @ingroup Core
 * Convert asset type to text
 * 
 * @param InAssetType	Asset type
 * @return Return converted asset type to text
 */
FORCEINLINE std::wstring ConvertAssetTypeToText( EAssetType InAssetType )
{
	switch ( InAssetType )
	{
	case AT_Texture2D:			return TEXT( "Texture2D" );
	case AT_Material:			return TEXT( "Material" );
	case AT_Script:				return TEXT( "Script" );
	case AT_StaticMesh:			return TEXT( "StaticMesh" );
	case AT_AudioBank:			return TEXT( "AudioBank" );
	case AT_PhysicsMaterial:	return TEXT( "PhysicsMaterial" );

	case AT_Unknown:
	default:
		return TEXT( "" );
	}
}

/**
 * @ingroup Core
 * Asset info in package
 */
struct FAssetInfo
{
	uint32						offset;		/**< Offset in archive to asset */
	uint32						size;		/**< Size data in archive */
	EAssetType					type;		/**< Asset type */
	std::wstring				name;		/**< Name of asset */
	TSharedPtr<class FAsset>	data;		/**< Pointer to asset (FMaterialRef, FTexture2DRef, etc)*/
};

/**
 * @ingroup Core
 * Struct reference to asset
 */
struct FAssetReference
{
	/**
	 * Constructor
	 * 
	 * @param[in] InType Asset type
	 * @param[in] InGUIDAsset GUID of asset
	 * @param[in] InGUIDPackage GUID of the package
	 */
	FORCEINLINE FAssetReference( EAssetType InType = AT_Unknown, const FGuid& InGUIDAsset = FGuid(), const FGuid& InGUIDPackage = FGuid() )
		: type( InType ), guidAsset( InGUIDAsset ), guidPackage( InGUIDPackage )
	{}

	/**
	 * Is valid
	 * @return Return true if asset reference is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != AT_Unknown && guidAsset.IsValid() && guidPackage.IsValid();
	}

	EAssetType		type;				/**< Asset type */
	FGuid			guidAsset;			/**< GUID of asset */
	FGuid			guidPackage;		/**< GUID of the package */
};

/**
 * @ingroup Core
 * Base class for serialize assets in package
 */
class FAsset
{
public:
	friend class FPackage;

	/**
	 * @brief Typedef of dependent assets
	 */
	typedef std::unordered_set< TWeakPtr<FAsset>, TWeakPtr<FAsset>::FHashFunction >		FSetDependentAssets;

	/**
	 * Constructor
	 * 
	 * @param[in] InType Asset type
	 */
	FAsset( EAssetType InType );

	/**
	 * Destructor
	 */
	virtual ~FAsset();

	/**
	 * Serialize
	 * 
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive );

	/**
	 * Set asset name
	 * 
	 * @param[in] InName Asset name
	 */
	void SetAssetName( const std::wstring& InName );

#if WITH_EDITOR
	/**
	 * Set path to asset source file
	 * @param InPath		Path to asset source file
	 */
	FORCEINLINE void SetAssetSourceFile( const std::wstring& InPath )
	{
		if ( sourceFile != InPath )
		{
			MarkDirty();
		}
		sourceFile = InPath;
	}
#endif // WITH_EDITOR

	/**
	 * Is dirty asset
	 * @return Return TRUE if asset is dirty, else return FALSE
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
	}

	/**
	 * Get asset type
	 * @return Return asset type
	 */
	FORCEINLINE EAssetType GetType() const
	{
		return type;
	}

	/**
	 * Get GUID of asset
	 * @return Return GUID of asset
	 */
	FORCEINLINE const FGuid& GetGUID() const
	{
		return guid;
	}

	/**
	 * Get name asset
	 * @return Return name asset
	 */
	FORCEINLINE const std::wstring& GetAssetName() const
	{
		return name;
	}

	/**
	 * Get package
	 * @return Return package where the asset is located. If asset not located in package return nullptr
	 */
	class FPackage* GetPackage() const
	{
		return package;
	}

	/**
	 * Get reference to asset
	 * @return Return struct info for reference to asset
	 */
	FAssetReference GetAssetReference() const;

#if WITH_EDITOR
	/**
	 * Get path to asset source file
	 * @return Return path to asset source file
	 */
	FORCEINLINE std::wstring GetAssetSourceFile() const
	{
		return sourceFile;
	}
#endif // WITH_EDITOR

	/**
	 * Get dependent assets
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( FSetDependentAssets& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const;

protected:
	/**
	 * Mark dirty asset
	 * @note When changing serializable fields, child classes should mark themselves as dirty
	 */
	void MarkDirty();

private:
	bool						bDirty;				/**< Is asset is dirty */
	class FPackage*				package;			/**< The package where the asset is located */
	std::wstring				name;				/**< Name asset */
	FGuid						guid;				/**< GUID of asset */
	EAssetType					type;				/**< Asset type */

#if WITH_EDITOR
	std::wstring				sourceFile;			/**< Path to source file */
#endif // WITH_EDITOR
};

/**
 * @ingroup Core
 * Class for help add, remove and find assets in package
 */
class FPackage : public FRefCounted
{
public:
	friend class FAsset;
	friend class FPackageManager;

	/**
	 * Destructor
	 */
	~FPackage();

	/**
	 * Save package
	 * 
	 * @param InPath Path to package
	 * @return Return true if package is saved, else false
	 */
	bool Save( const std::wstring& InPath );

	/**
	 * Add asset to package
	 * 
	 * @param[in] InAsset Asset
	 */
	void Add( const TSharedPtr<FAsset>& InAsset );

	/**
	 * Remove asset from package
	 * 
	 * @param InAsset			Asset to remove
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @return Return TRUE if asset seccussed removed from package
	 */
	FORCEINLINE bool Remove( const TSharedPtr<FAsset>& InAsset, bool InForceUnload = false )
	{
		if ( !InAsset.IsValid() )
		{
			return true;
		}
		return Remove( InAsset->GetGUID(), InForceUnload );
	}

	/**
	 * Remove asset from package by GUID
	 * 
	 * @param InGUID			Asset guid
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @return Return TRUE if asset seccussed removed from package
	 */
	bool Remove( const FGuid& InGUID, bool InForceUnload = false );

	/**
	 * Remove asset from package by name
	 * 
	 * @param InName			Name asset
	 * @param InForceUnload		Is need force unload asset if him loaded
	 * @return Return TRUE if asset seccussed removed from package
	 */
	FORCEINLINE bool Remove( const std::wstring& InName, bool InForceUnload = false )
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return true;
		}

		return Remove( itAssetGUID->second, InForceUnload );
	}

	/**
	 * Remove all from package
	 * 
	 * @param InForceUnload		Is need force unload assets if him loaded
	 * @return Return TRUE if all assets seccussed removed from package
	 */
	bool RemoveAll( bool InForceUnload = false );
	
	/**
	 * Unload asset
	 * 
	 * @param InAssetPtr			AssetPtr
	 * @param InForceUnload			Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	FORCEINLINE bool UnloadAsset( const TWeakPtr<FAsset>& InAssetPtr, bool InForceUnload = false )
	{
		FGuid				assetGuid;
		{
			TSharedPtr<FAsset>	assetRef = InAssetPtr.Pin();
			if ( !assetRef )
			{
				return true;
			}
			assetGuid = assetRef->guid;
		}

		return UnloadAsset( assetGuid, InForceUnload );
	}

	/**
	 * Unload asset
	 * 
	 * @param InGuid				Asset guid
	 * @param InForceUnload			Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	FORCEINLINE bool UnloadAsset( const FGuid& InGuid, bool InForceUnload = false )
	{
		auto		itAsset = assetsTable.find( InGuid );
		if ( itAsset == assetsTable.end() )
		{
			return true;
		}

		FAssetInfo&		assetInfo = itAsset->second;
		return UnloadAsset( assetInfo, InForceUnload );
	}

	/**
	 * Unload all assets
	 * 
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if assets is unloaded
	 */
	bool UnloadAllAssets( bool InForceUnload = false );

	/**
	 * Is exist asset with name in package
	 * 
	 * @param InName		Asset name
	 * @return Return TRUE if asset with name InName exist in package, else return FALSE
	 */
	FORCEINLINE bool IsExist( const std::wstring& InName ) const
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return false;
		}

		return true;
	}

	/**
	 * Is exist asset with GUID in package
	 * 
	 * @param InGUID		Asset guid
	 * @return Return TRUE if asset with guid InGUID exist in package, else return FALSE
	 */
	FORCEINLINE bool IsExist( const FGuid& InGUID ) const
	{
		auto		itAsset = assetsTable.find( InGUID );
		if ( itAsset == assetsTable.end() )
		{
			return false;
		}

		return true;
	}

	/**
	 * Find asset
	 * 
	 * @param[in] InGUID GUID of asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	TSharedPtr<FAsset> Find( const FGuid& InGUID );

	/**
	 * Find asset by name
	 * 
	 * @param InName Name asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	FORCEINLINE TSharedPtr<FAsset> Find( const std::wstring& InName )
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return nullptr;
		}

		return Find( itAssetGUID->second );
	}

	/**
	 * Set name of the package
	 * 
	 * @param InName New name of the package
	 */
	FORCEINLINE void SetName( const std::wstring& InName )
	{
		name = InName;
		bIsDirty = true;
		numDirtyAssets = -1;
	}

	/**
	 * Set name from path
	 * 
	 * @param InPath Path
	 */
	void SetNameFromPath( const std::wstring& InPath );

	/**
	 * Get GUID of package
	 * @return Return GUID of package
	 */
	FORCEINLINE FGuid GetGUID() const
	{
		return guid;
	}

	/**
	 * Get number loaded assets in package
	 * @retun Return number loaded assets
	 */
	FORCEINLINE uint32 GetNumLoadedAssets() const
	{
		return numLoadedAssets;
	}

	/**
	 * Get number assets in package
	 * @return Return number assets in package
	 */
	FORCEINLINE uint32 GetNumAssets() const
	{
		return assetsTable.size();
	}

	/**
	 * Is empty package
	 * @return Return true if package is empty, else return false
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return GetNumAssets() == 0;
	}

	/**
	 * Is package dirty
	 * @return Return if package is dirty, else return false
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bIsDirty;
	}

	/**
	 * Get path to the package from which data was last loaded
	 * @return Return path, if package is not loaded from other packages return empty string
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return filename;
	}

	/**
	 * Get name of the package
	 * @return Return name of the package
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * Get asset info by index
	 * 
	 * @param InIndex Index of asset in package
	 * @param OutAssetInfo Output reference to asset info
	 * @param OutGuidAsset Optional output of guid asset
	 */
	FORCEINLINE void GetAssetInfo( uint32 InIndex, FAssetInfo& OutAssetInfo, FGuid* OutGuidAsset = nullptr ) const
	{
		check( !assetsTable.empty() && InIndex >= 0 && InIndex < assetsTable.size() );
		
		uint32		index = 0;
		for ( auto itAsset = assetsTable.cbegin(), itAssetEnd = assetsTable.cend(); itAsset != itAssetEnd; ++itAsset, ++index )
		{
			if ( InIndex == index )
			{
				OutAssetInfo = itAsset->second;
				if ( OutGuidAsset )
				{
					*OutGuidAsset = itAsset->first;
				}
				break;
			}
		}
	}

private:
	/**
	 * Typedef map of asset name to GUID
	 */
	typedef std::unordered_map< std::wstring, FGuid >								FAssetNameToGUID;

	/**
	 * Typedef map of assets table
	 */
	typedef std::unordered_map< FGuid, FAssetInfo, FGuid::FGuidKeyFunc >			FAssetTable;

	/**
	 * Constructor
	 *
	 * @param InName Name of the package
	 */
	FPackage( const std::wstring& InName = TEXT( "" ) );

	/**
	 * Load package
	 *
	 * @param InPath Path to package
	 * @return Return true if package is loaded, else false
	 */
	bool Load( const std::wstring& InPath );

	/**
	 * Fully load
	 * @param OutAssetArray Array of loaded asset from package
	 */
	void FullyLoad( std::vector< TSharedPtr<FAsset> >& OutAssetArray );

	/**
	 * Unload asset by asset info
	 *
	 * @param InAssetInfo		Asset info
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @param InBroadcastEvent	Is need broadcast event OnAssetsCanDelete and OnAssetsDeleted. @note Only for editor
	 * @return Return TRUE if asset is unloaded
	 */
	bool UnloadAsset( FAssetInfo& InAssetInfo, bool InForceUnload = false, bool InBroadcastEvent = true );

	/**
	 * Serialize package
	 * 
	 * @param InArchive Archive
	 */
	void Serialize( FArchive& InArchive );

	/**
	 * Serialize header of the package
	 * 
	 * @param InArchive		Archive
	 * @param InIsNeedSkip	Is need skip header
	 */
	void SerializeHeader( FArchive& InArchive, bool InIsNeedSkip = false );

	/**
	 * Load asset from package
	 * 
	 * @param InArchive					Archive
	 * @param InAssetGUID				Asset GUID
	 * @param InAssetInfo				Asset info
	 * @return Return loaded asset from package, if failed returning nullptr
	 */
	TSharedPtr<FAsset> LoadAsset( FArchive& InArchive, const FGuid& InAssetGUID, FAssetInfo& InAssetInfo );

	/**
	 * Update asset name in table
	 * @warning Must called from FAsset
	 * 
	 * @param InGUID		GUID of asset
	 */
	void UpdateAssetNameInTable( const FGuid& InGUID );

	/**
	 * Mark dirty asset in package
	 * @warning Must called from FAsset
	 * 
	 * @param InGUID		GUID of asset
	 */
	void MarkAssetDirty( const FGuid& InGUID );

	bool				bIsDirty;			/**< Is dirty package */
	FGuid				guid;				/**< GUID of package */
	std::wstring		filename;			/**< Path to the package from which data was last loaded */
	std::wstring		name;				/**< Package name */
	uint32				numLoadedAssets;	/**< Number loaded assets */
	uint32				numDirtyAssets;		/**< Number dirty assets in package */
	FAssetNameToGUID	assetGUIDTable;		/**< Table for converting asset GUID to name */
	FAssetTable			assetsTable;		/**< Table of assets in package */
};

/**
 * @ingroup Core
 * Class manager all packages in engine
 */
class FPackageManager
{
public:
	friend FPackage;

	/**
	 * Constructor
	 */
	FPackageManager();

	/**
	 * Init package manager
	 */
	void Init();

	/**
	 * Update package manager
	 * This method removes unused packages
	 */
	void Tick();

	/**
	 * Shutdown package manager
	 */
	void Shutdown();

	/**
	 * Find asset in package by <AssetType>'<PackageName>:<AssetName>
	 * 
	 * @param InString Reference to asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	TSharedPtr<FAsset> FindAsset( const std::wstring& InString, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 * 
	 * @param InGUIDPackage GUID of the package
	 * @param InGUIDAsset GUID of asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FORCEINLINE TSharedPtr<FAsset> FindAsset( const FGuid& InGUIDPackage, const FGuid& InGUIDAsset, EAssetType InType = AT_Unknown )
	{
		std::wstring		path = GTableOfContents.GetPackagePath( InGUIDPackage );
		if ( path.empty() )
		{
			return nullptr;
		}

		return FindAsset( path, InGUIDAsset, InType );
	}

	/**
	 * Find asset in package
	 * 
	 * @param InPath Path to the package
	 * @param InGUIDAsset GUID of asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	TSharedPtr<FAsset> FindAsset( const std::wstring& InPath, const FGuid& InGUIDAsset, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 *
	 * @param InPath Path to the package
	 * @param InAsset Asset name in the package
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	TSharedPtr<FAsset> FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType = AT_Unknown );

	/**
	 * Find default asset
	 * 
	 * @param InType Default asset for type
	 * @return Return finded asset. If not found returning nullptr
	 */
	TSharedPtr<FAsset> FindDefaultAsset( EAssetType InType ) const;

	/**
	 * Is default asset
	 * 
	 * @param InAsset	Asset
	 * @return Return TRUE if InAsset is default asset, else return FALSE
	 */
	FORCEINLINE bool IsDefaultAsset( const TSharedPtr<FAsset>& InAsset ) const
	{
		if ( !InAsset )
		{
			return false;
		}
		return InAsset == FindDefaultAsset( InAsset->GetType() );
	}

	/**
	 * Load package
	 * 
	 * @param InPath Package path
	 * @param InCreateIfNotExist Is need create package if not exist
	 * @return Return loaded package. If not found returning nullptr
	 */
	FPackageRef LoadPackage( const std::wstring& InPath, bool InCreateIfNotExist = false );

	/**
	 * Unload package
	 * 
	 * @param InPath			Package path
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return true if package is unloaded. If in package exist dirty asset(s) or package not found return false
	 */
	bool UnloadPackage( const std::wstring& InPath, bool InForceUnload = false );

	/**
	 * Unload all packages
	 * 
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if all packages is unloaded. If in package exist dirty asset(s) or package not found return FALSE
	 */
	bool UnloadAllPackages( bool InForceUnload = false );

	/**
	 * Unload asset
	 *
	 * @param InAssetPtr		Asset ptr
	 * @param InForceUnload		Is need force unload (ignored shared references)
	 * @return Return TRUE if asset is unloaded
	 */
	bool UnloadAsset( const TWeakPtr<FAsset>& InAssetPtr, bool InForceUnload = false );

	/**
	 * Garbage collector of unused packages and assets
	 */
	void GarbageCollector();

	/**
	 * Is package loaded
	 * 
	 * @param InPath Package path
	 * @return Return true if package is loaded, else return false
	 */
	FORCEINLINE bool IsPackageLoaded( const std::wstring& InPath ) const
	{
		auto		itPackage = packages.find( InPath );
		return itPackage != packages.end();
	}

private:	
	/**
	 * Struct of normalized path in file system
	 */
	struct FNormalizedPath
	{
		/**
		 * @brief Functions to extract the FNormalizedPath as a key for std::unordered_map and std::unordered_set
		 */
		struct FNormalizedPathKeyFunc
		{
			/**
			 * @brief Calculate hash of the FNormalizedPath
			 *
			 * @param InPath Path
			 * @return Return hash of this FNormalizedPath
			 */
			FORCEINLINE std::size_t operator()( const FNormalizedPath& InPath ) const
			{
				return InPath.GetTypeHash();
			}

			/**
			 * @brief Compare FNormalizedPath
			 *
			 * @param InA First FNormalizedPath
			 * @param InB Second FNormalizedPath
			 * @return Return true if InA and InB equal, else returning false
			 */
			FORCEINLINE bool operator()( const FNormalizedPath& InA, const FNormalizedPath& InB ) const
			{
				return InA.GetTypeHash() < InB.GetTypeHash();
			}
		};

		/**
		 * Constructor
		 * 
		 * @param InPath Path
		 */
		FORCEINLINE FNormalizedPath( const std::wstring& InPath )
			: path( InPath )
		{
			appNormalizePathSeparators( path );
		}

		/**
		 * Set new path
		 *
		 * @param InPath Path
		 */
		FORCEINLINE void Set( const std::wstring& InPath )
		{
			path = InPath;
			appNormalizePathSeparators( path );
		}

		/**
		 * Convert path in string
		 * @return Return path in string
		 */
		FORCEINLINE const std::wstring& ToString() const
		{
			return path;
		}

		/**
		 * Get hash of type
		 * @return Return hash of this FNormalizedPath
		 */
		FORCEINLINE uint64 GetTypeHash() const
		{
			return appMemFastHash( ( const void* ) path.c_str(), path.size() );
		}

		/**
		 * Override operator for set new path
		 */
		FORCEINLINE FNormalizedPath& operator=( const std::wstring& InRight )
		{
			Set( InRight );
			return *this;
		}

		/**
		 * Override operator for compare to FNormalizedPath
		 */
		FORCEINLINE bool operator==( const FNormalizedPath& InRight ) const
		{
			return path == InRight.path;
		}

		/**
		 * @brief Overloaded operator std::wstring
		 */
		FORCEINLINE operator std::wstring() const
		{
			return path;
		}

	private:
		std::wstring		path;			/**< Normalized path */
	};

	/**
	 * Typedef of list loaded packages
	 */
	typedef std::unordered_map< FNormalizedPath, FPackageRef, FNormalizedPath::FNormalizedPathKeyFunc >			FPackageList;

	FPackageList			packages;				/**< Opened packages */
};

/**
 * @ingroup Core
 * @brief Parse reference to asset in format <AssetType>'<PackageName>:<AssetName>
 *
 * @param InString Reference to asset
 * @param OutPackageName Package name
 * @param OutAssetName Asset name
 * @param OutAssetType Asset type
 * @return Return true if InString parsed is seccussed, else returning false
 */
bool ParseReferenceToAsset( const std::wstring& InString, std::wstring& OutPackageName, std::wstring& OutAssetName, EAssetType& OutAssetType );

/**
 * @ingroup Core
 * @brief Make reference to asset in format <AssetType>'<PackageName>:<AssetName>
 * 
 * @param InPackageName		Package name
 * @param InAssetName		Asset name
 * @param InAssetType		Asset type
 * @param OutString			Output string with reference
 * @return Return TRUE if reference created is seccussed, else returning FALSE
 */
FORCEINLINE bool MakeReferenceToAsset( const std::wstring& InPackageName, const std::wstring& InAssetName, EAssetType InAssetType, std::wstring& OutString )
{
	if ( InPackageName.empty() || InAssetName.empty() )
	{
		return false;
	}

	OutString = ConvertAssetTypeToText( InAssetType ) + TEXT( "'" ) + InPackageName + TEXT(":") + InAssetName;
	return true;
}

/**
 * @ingroup Core
 * @brief Make reference to asset in format <AssetType>'<PackageName>:<AssetName>
 * 
 * @param InAsset	Asset
 * @param OutString			Output string with reference
 * @return Return TRUE if reference created is seccussed, else returning FALSE
 */
FORCEINLINE bool MakeReferenceToAsset( const TSharedPtr<FAsset>& InAsset, std::wstring& OutString )
{
	if ( !InAsset || !InAsset->GetPackage() || InAsset->GetAssetName().empty() )
	{
		return false;
	}
	return MakeReferenceToAsset( InAsset->GetPackage()->GetName(), InAsset->GetAssetName(), InAsset->GetType(), OutString );
}

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, EAssetType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( EAssetType ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const EAssetType& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( EAssetType ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAssetReference& InValue )
{
	check( InArchive.Ver() >= VER_GUIDPackages );
	InArchive << InValue.type;

	if ( InArchive.IsLoading() && InArchive.Ver() < VER_GUIDAssets )
	{
		uint32		hash = 0;
		InArchive << hash;
		InValue.guidAsset.Set( hash, 0, 0, 0 );
	}
	else
	{
		InArchive << InValue.guidAsset;
	}

	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAssetReference& InValue )
{
	check( InArchive.IsSaving() && InArchive.Ver() >= VER_GUIDPackages );
	InArchive << InValue.type;
	InArchive << InValue.guidAsset;
	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, TWeakPtr<FAsset>& InValue )
{
	if ( InArchive.IsSaving() )
	{
		TSharedPtr<FAsset>		asset = InValue.Pin();
		InArchive << ( asset ? asset->GetAssetReference() : FAssetReference() );

#if DO_CHECK
		if ( asset )
		{
			FAssetReference		assetRef = asset->GetAssetReference();
			check( assetRef.guidAsset.IsValid() && assetRef.guidPackage.IsValid() );
		}
#endif // DO_CHECK
	}
	else
	{
		FAssetReference			assetReference;
		InArchive << assetReference;
		if ( assetReference.IsValid() )
		{
			InValue = GPackageManager->FindAsset( assetReference.guidPackage, assetReference.guidAsset, assetReference.type );
		}
		else
		{
			InValue = GPackageManager->FindDefaultAsset( assetReference.type );
		}
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const TWeakPtr<FAsset>& InValue )
{
	check( InArchive.IsSaving() );
	
	TSharedPtr<FAsset>		asset = InValue.Pin();
	InArchive << ( asset ? asset->GetAssetReference() : FAssetReference() );
	return InArchive;
}

#endif // !PACKAGE_H
