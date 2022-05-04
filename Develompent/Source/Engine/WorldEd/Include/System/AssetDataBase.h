/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASSETDATABASE_H
#define ASSETDATABASE_H

#include <qstring>
#include <unordered_map>
#include <list>

#include "System/Package.h"

/**
 * @ingroup WorldEd
 * Asset data base
 */
class FAssetDataBase
{
public:
	friend class FPackage;
	friend class FPackageManager;

	/**
	 * Init data base
	 */
	void Init();

	/**
	 * Shutdown data base
	 */
	void Shutdown();

private:
	/**
	 * Add asset to data base
	 * @param InAsset		Asset
	 */
	FORCEINLINE void AddAsset( FAsset* InAsset )
	{
		checkMsg( InAsset && InAsset->GetPackage(), TEXT( "For add asset to data base must be valid InAsset and containing in package" ) );
		loadedAssetsMap[ InAsset->GetPackage() ].push_back( InAsset );
	}

	/**
	 * Remove asset from data base
	 * @param InAsset		Asset
	 */
	void RemoveAsset( FAsset* InAsset );

	/**
	 * Remove all assets of package
	 * @param InPackage		Package
	 */
	void RemoveAssets( FPackage* InPackage );

	/**
	 * Typedef of map loaded assets
	 */
	typedef std::unordered_map< FPackage*, std::list< FAssetRef > >		FLoadedAssetsMap;

	FLoadedAssetsMap		loadedAssetsMap;		/**< Map of loaded assets */
};

#endif // !ASSETDATABASE_H