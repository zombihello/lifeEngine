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
	AT_StaticMesh		/**< Static mesh */
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
	 * @param[in] InHash Hash asset
	 * @param[in] InGUID GUID of the package
	 */
	FORCEINLINE FAssetReference( EAssetType InType = AT_Unknown, uint32 InHash = ( uint32 )INVALID_HASH, const FGuid& InGUID = FGuid() ) 
		: type( InType ), hash( InHash ), guidPackage( InGUID )
	{}

	/**
	 * Is valid
	 * @return Return true if asset reference is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return type != AT_Unknown && hash != ( uint32 )INVALID_HASH && guidPackage.IsValid();
	}

	EAssetType		type;				/**< Asset type */
	uint32			hash;				/**< Asset hash */
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
	 * Set asset hash
	 * 
	 * @param[in] InHash Hash asset
	 */
	void SetAssetHash( uint32 InHash );

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
	 * Get hash asset
	 * @return Return hash asset, if hash not setted return INVALID_HASH
	 */
	FORCEINLINE uint32 GetHash() const
	{
		return hash;
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
	class FPackage*		package;	/**< The package where the asset is located */
	std::wstring		name;		/**< Name asset */
	uint32				hash;		/**< Asset hash */
	EAssetType			type;		/**< Asset type */
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

	/**
	 * Constructor
	 * 
	 * @param InName Name of the package
	 */
	FPackage( const std::wstring& InName = TEXT( "" ) );

	/**
	 * Destructor
	 */
	~FPackage();

	/**
	 * Load package
	 * 
	 * @param InPath Path to package
	 * @return Return true if package is loaded, else false
	 */
	bool Load( const std::wstring& InPath );

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
		checkMsg( InAsset->hash != INVALID_HASH, TEXT( "For add asset to package need hash is valid" ) );

		InAsset->package = this;
		assetsTable[ InAsset->hash ] = FAssetInfo{ ( uint32 )INVALID_ID, ( uint32 )INVALID_ID, InAsset->type, InAsset->name, InAsset };
	}

	/**
	 * Remove asset from package
	 * 
	 * @param[in] InAsset Asset to remove
	 */
	FORCEINLINE void Remove( const FAsset& InAsset )
	{
		Remove( InAsset.GetHash() );
	}

	/**
	 * Remove asset from package by hash
	 * 
	 * @param[in] InHash Asset hash
	 */
	void Remove( uint32 InHash );

	/**
	 * Remove all from package
	 */
	void RemoveAll();
	
	/**
	 * Find asset
	 * 
	 * @param[in] InHash Hash of asset
	 * @return Return pointer to asset in package, if not found return nullptr
	 */
	FAssetRef Find( uint32 InHash );

	/**
	 * Set name of the package
	 * 
	 * @param InName New name of the package
	 */
	FORCEINLINE void SetName( const std::wstring& InName )
	{
		name = InName;
	}

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
	 * Typedef map of assets table
	 */
	typedef std::unordered_map< uint32, FAssetInfo >		FAssetTable;

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
	 * @param InAssetHash Asset hash
	 * @param InAssetInfo Asset info
	 * @return Return loaded asset from package, if failed returning nullptr
	 */
	FAssetRef LoadAsset( FArchive& InArchive, uint32 InAssetHash, FAssetInfo& InAssetInfo );

	/**
	 * Mark that the asset is unloaded
	 * 
	 * @param[in] InHash Hash of asset
	 */
	void MarkAssetUnlnoad( uint32 InHash );

	/**
	 * Mark that the hash asset is updated
	 * 
	 * @param[in] InOldHash Old asset hash
	 * @param[in] InNewHash New asset hash
	 */
	void MarkHashAssetUpdate( uint32 InOldHash, uint32 InNewHash );

	FGuid			guid;				/**< GUID of package */
	std::wstring	filename;			/**< Path to the package from which data was last loaded */
	std::wstring	name;				/**< Package name */
	uint32			numUsageAssets;		/**< Number assets in usage */
	FAssetTable		assetsTable;		/**< Table of assets in package */
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
	 * Contructor
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
	 * @param InString
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FAssetRef FindAsset( const std::wstring& InString, EAssetType InType = AT_Unknown );

	/**
	 * Find asset in package
	 * 
	 * @param InGUID GUID of the package
	 * @param InHash Asset hash
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 * @return Return finded asset. If not found returning nullptr
	 */
	FORCEINLINE FAssetRef FindAsset( const FGuid& InGUID, uint32 InHash, EAssetType InType = AT_Unknown )
	{
		std::wstring		path = GTableOfContents.GetPackagePath( InGUID );
		if ( path.empty() )
		{
			return nullptr;
		}

		return FindAsset( path, InHash, InType );
	}

	/**
	 * Find asset in package
	 * 
	 * @param InPath Path to the package
	 * @param InHash Asset hash
	 * @param InType Asset type. Optional parameter, if setted return default asset in case fail
	 */
	FAssetRef FindAsset( const std::wstring& InPath, uint32 InHash, EAssetType InType = AT_Unknown );

	/**
	 * Open package
	 * 
	 * @param InPath Package path
	 * @return Return opened package. If not found returning nullptr
	 */
	FPackageRef OpenPackage( const std::wstring& InPath );

	/**
	 * Close package
	 * 
	 * @param InPath Package path
	 * @return Return true if package is closed. If in package used asset(s) or package not found return false
	 */
	bool ClosePackage( const std::wstring& InPath );

	/**
	 * Is package opened
	 * 
	 * @param InPath package path
	 * @return Return true if package is opened, else return false
	 */
	FORCEINLINE bool IsPackageOpened( const std::wstring& InPath ) const
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
	 * Typedef of list loaded packages
	 */
	typedef std::unordered_map< std::wstring, FPackageInfo >			FPackageList;

	/**
	 * Typedef of unused list packages
	 */
	typedef std::unordered_set< std::wstring >							FUnusedPackageList;

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
	InArchive << InValue.hash;
	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAssetReference& InValue )
{
	check( InArchive.IsSaving() && InArchive.Ver() >= VER_GUIDPackages );
	InArchive << InValue.type;
	InArchive << InValue.hash;
	InArchive << InValue.guidPackage;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAssetRef& InValue )
{
	if ( InArchive.IsSaving() )
	{
		InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	}
	else
	{
		FAssetReference			assetReference;
		InArchive << assetReference;
		if ( assetReference.IsValid() )
		{
			InValue = GPackageManager->FindAsset( assetReference.guidPackage, assetReference.hash, assetReference.type );
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