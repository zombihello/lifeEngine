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

#include "Misc/Types.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "Misc/Misc.h"
#include "Misc/Guid.h"
#include "Misc/TableOfContents.h"
#include "Misc/CoreGlobals.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * Reference to FAsset
 */
typedef TRefCountPtr< class FAsset >			FAssetRef;

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
	AT_Unknown,			/**< Unknown asset */
	AT_Texture2D,		/**< Texture 2D */
	AT_Material,		/**< Material */
	AT_Script,			/**< Script */
	AT_StaticMesh,		/**< Static mesh */
	AT_AudioBank		/**< Audio bank */
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
class FAsset : public FRefCounted
{
public:
	friend class FPackage;

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
	FORCEINLINE void SetAssetName( const std::wstring& InName )
	{
		name = InName;
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

private:
	class FPackage*		package;		/**< The package where the asset is located */
	std::wstring		name;			/**< Name asset */
	FGuid				guid;			/**< GUID of asset */
	EAssetType			type;			/**< Asset type */
};

/**
 * @ingroup Core
 * Asset info in package
 */
struct FAssetInfo
{
	uint32			offset;		/**< Offset in archive to asset */
	uint32			size;		/**< Size data in archive */
	EAssetType		type;		/**< Asset type */
	std::wstring	name;		/**< Name of asset */
	FAsset*			data;		/**< Pointer to asset (FMaterialRef, FTexture2DRef, etc)*/
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
	FORCEINLINE void Add( FAsset* InAsset )
	{
		check( InAsset );
		checkMsg( InAsset->guid.IsValid(), TEXT( "For add asset to package need GUID is valid" ) );

		// If asset in package already containing, remove from table old GUID
		auto		it = assetGUIDTable.find( InAsset->name );
		if ( it != assetGUIDTable.end() && it->second != InAsset->guid )
		{
			assetsTable.erase( it->second );
		}

		InAsset->package = this;
		assetGUIDTable[ InAsset->name ] = InAsset->guid;
		assetsTable[ InAsset->guid ] = FAssetInfo{ ( uint32 )INVALID_ID, ( uint32 )INVALID_ID, InAsset->type, InAsset->name, InAsset };
		++numUsageAssets;
		bIsDirty = true;
	}

	/**
	 * Remove asset from package
	 * 
	 * @param[in] InAsset Asset to remove
	 */
	FORCEINLINE void Remove( const FAsset& InAsset )
	{
		Remove( InAsset.GetGUID() );
	}

	/**
	 * Remove asset from package by GUID
	 * 
	 * @param[in] InGUID Asset guid
	 */
	void Remove( const FGuid& InGUID );

	/**
	 * Remove asset from package by name
	 * 
	 * @param InName Name asset
	 */
	FORCEINLINE void Remove( const std::wstring& InName )
	{
		auto		itAssetGUID = assetGUIDTable.find( InName );
		if ( itAssetGUID == assetGUIDTable.end() )
		{
			return;
		}

		Remove( itAssetGUID->second );
	}

	/**
	 * Remove all from package
	 */
	void RemoveAll();
	
	/**
	 * Find asset
	 * 
	 * @param[in] InGUID GUID of asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	FAssetRef Find( const FGuid& InGUID );

	/**
	 * Find asset by name
	 * 
	 * @param InName Name asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	FORCEINLINE FAssetRef Find( const std::wstring& InName )
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
	 * Get number usage assets from package
	 * @retun Return number assets in usage
	 */
	FORCEINLINE uint32 GetNumUsageAssets() const
	{
		return numUsageAssets;
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
	 */
	FORCEINLINE void GetAssetInfo( uint32 InIndex, FAssetInfo& OutAssetInfo ) const
	{
		check( !assetsTable.empty() );

		uint32		index = 0;
		for ( auto itAsset = assetsTable.cbegin(), itAssetEnd = assetsTable.cend(); itAsset != itAssetEnd; ++itAsset, ++index )
		{
			if ( InIndex == index )
			{
				OutAssetInfo = itAsset->second;
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
	void FullyLoad( std::vector< FAssetRef >& OutAssetArray );

	/**
	 * Serialize package
	 * 
	 * @param InArchive Archive
	 */
	void Serialize( FArchive& InArchive );

	/**
	 * Serialize header of the package
	 * 
	 * @param InArchive Archive
	 */
	void SerializeHeader( FArchive& InArchive );

	/**
	 * Load asset from package
	 * 
	 * @param InArchive Archive
	 * @param InAssetGUID Asset GUID
	 * @param InAssetInfo Asset info
	 * @return Return loaded asset from package, if failed returning nullptr
	 */
	FAssetRef LoadAsset( FArchive& InArchive, const FGuid& InAssetGUID, FAssetInfo& InAssetInfo );

	/**
	 * Mark that the asset is unloaded
	 * 
	 * @param[in] InGUID GUID of asset
	 */
	void MarkAssetUnlnoad( const FGuid& InGUID );

	bool				bIsLoaded;			/**< Is package loaded from HDD */
	bool				bIsDirty;			/**< Is dirty package */
	FGuid				guid;				/**< GUID of package */
	std::wstring		filename;			/**< Path to the package from which data was last loaded */
	std::wstring		name;				/**< Package name */
	uint32				numUsageAssets;		/**< Number assets in usage */
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
	 * Find asset in package by <PackageName>:<AssetName>
	 * 
	 * @param InString Reference to asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FAssetRef FindAsset( const std::wstring& InString, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 * 
	 * @param InGUIDPackage GUID of the package
	 * @param InGUIDAsset GUID of asset
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FORCEINLINE FAssetRef FindAsset( const FGuid& InGUIDPackage, const FGuid& InGUIDAsset, EAssetType InType = AT_Unknown )
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
	FAssetRef FindAsset( const std::wstring& InPath, const FGuid& InGUIDAsset, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 *
	 * @param InPath Path to the package
	 * @param InAsset Asset name in the package
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	FAssetRef FindAsset( const std::wstring& InPath, const std::wstring& InAsset, EAssetType InType = AT_Unknown );

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
	 * @param InPath Package path
	 * @return Return true if package is unloaded. If in package used asset(s) or package not found return false
	 */
	bool UnloadPackage( const std::wstring& InPath );

	/**
	 * Is package loaded
	 * 
	 * @param InPath package path
	 * @return Return true if package is loaded, else return false
	 */
	FORCEINLINE bool IsPackageLoaded( const std::wstring& InPath ) const
	{
		auto		itPackage = packages.find( InPath );
		return itPackage != packages.end();
	}

	/**
	 * Forcibly cleanup unused packages
	 */
	void CleanupUnusedPackages();

private:
	/**
	 * Struct package info
	 */
	struct FPackageInfo
	{
		bool				isUnused;				/**< Is package not used */
		FPackageRef			package;				/**< Package */
	};

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
	typedef std::unordered_map< FNormalizedPath, FPackageInfo, FNormalizedPath::FNormalizedPathKeyFunc >			FPackageList;

	/**
	 * Typedef of unused list packages
	 */
	typedef std::unordered_set< FNormalizedPath, FNormalizedPath::FNormalizedPathKeyFunc >							FUnusedPackageList;

	/**
	 * Check usage package
	 * 
	 * @param InPath Path to the package
	 */
	FORCEINLINE void CheckUsagePackage( const std::wstring& InPath )
	{
		auto		itPackage = packages.find( InPath );
		if ( itPackage == packages.end() )
		{
			return;
		}

		CheckUsagePackage( itPackage->second );
	}

	/**
	 * Check usage package
	 * 
	 * @param[in/out] InOutPackageInfo Info package
	 */
	void CheckUsagePackage( FPackageInfo& InOutPackageInfo );

	float					cleaningFrequency;		/**< Rate of removes unused packages */
	double					lastCleaningTime;		/**< Last time removes unused packages */
	FUnusedPackageList		unusedPackages;			/**< List unused packages. After a specified time will be unloaded */
	FPackageList			packages;				/**< Opened packages */
};

/**
 * @ingroup Core
 * @brief Parse reference to asset in format <PackageName>:<AssetName>
 *
 * @param InString Reference to asset
 * @param OutPackageName Package name
 * @param OutAssetName Asset name
 * @return Return true if InString parsed is seccussed, else returning false
 */
bool ParseReferenceToAsset( const std::wstring& InString, std::wstring& OutPackageName, std::wstring& OutAssetName );

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

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAssetRef& InValue )
{
	if ( InArchive.IsSaving() )
	{
		InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );

#if DO_CHECK
		if ( InValue )
		{
			FAssetReference		assetRef = InValue->GetAssetReference();
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
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAssetRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	return InArchive;
}

#endif // !PACKAGE_H
