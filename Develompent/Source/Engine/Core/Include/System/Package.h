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
	 * @param[in] InHash Hash asset
	 * @param[in] InPath Path to package with asset
	 */
	FORCEINLINE FAssetReference( uint32 InHash = ( uint32 )INVALID_HASH, const std::wstring& InPath = TEXT( "" ) ) :
		hash( InHash ),
		pathPackage( InPath )
	{}

	/**
	 * Is valid
	 * @return Return true if asset reference is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return hash != ( uint32 )INVALID_HASH && !pathPackage.empty();
	}

	uint32			hash;				/**< Asset hash */
	std::wstring	pathPackage;		/**< Path to package */
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
	void SetHash( uint32 InHash );

	/**
	 * Set asset hash from name
	 * 
	 * @param[in] InName Name
	 */
	FORCEINLINE void SetHashFromName( const std::wstring& InName )
	{
		SetHash( appCalcHash( InName ) );
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
	uint32				hash;		/**< Asset hash */
	EAssetType			type;		/**< Asset type */
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
	 */
	FPackage();

	/**
	 * Destructor
	 */
	~FPackage();

	/**
	 * Open package
	 * 
	 * @param[in] InPath Path to package
	 * @param[in] InIsWrite Open package in write mode
	 * @return Return true if package is opened, else return false
	 */
	bool Open( const std::wstring& InPath, bool InIsWrite = false );

	/**
	 * Close package
	 * This method delete handle of archive and clean asset table
	 */
	void Close();

	/**
	 * Serialize package
	 */
	void Serialize();

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
		assetsTable[ InAsset->hash ] = FAssetInfo{ ( uint32 )INVALID_ID, ( uint32 )INVALID_ID, InAsset };
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
	 * Get path to package
	 * @return Return path to package
	 */
	FORCEINLINE const std::wstring& GetPath() const
	{
		return path;
	}

	/**
	 * Get number usage assets from package
	 * @retun Return number assets in usage
	 */
	FORCEINLINE uint32 GetNumUsageAssets() const
	{
		return numUsageAssets;
	}

private:
	/**
	 * Asset info in package
	 */
	struct FAssetInfo
	{
		uint32			offset;		/**< Offset in archive to asset */
		uint32			size;		/**< Size data in archive */
		FAsset*			data;		/**< Pointer to asset (FMaterialRef, FTexture2DRef, etc)*/
	};

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

	FArchive*										archive;			/**< Archive of package */
	uint32											numUsageAssets;		/**< Number assets in usage */
	std::wstring									path;				/**< Path to package */
	std::unordered_map< uint32, FAssetInfo >		assetsTable;		/**< Table of assets in package */
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
	 * Find asset in package
	 * 
	 * @param[in] InPath Package path
	 * @param[in] InHash Asset hash
	 */
	FAssetRef FindAsset( const std::wstring& InPath, uint32 InHash );

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
	typedef std::unordered_map< std::wstring, FPackageInfo >				FPackageList;

	/**
	 * Typedef of unused list packages
	 */
	typedef std::unordered_set< std::wstring >								FUnusedPackageList;

	/**
	 * Check usage package
	 * 
	 * @param[in] InPath Path to package
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

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FAssetReference& InValue )
{
	InArchive << InValue.hash;
	InArchive << InValue.pathPackage;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FAssetReference& InValue )
{
	InArchive << InValue.hash;
	InArchive << InValue.pathPackage;
	return InArchive;
}

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

#endif // !PACKAGE_H