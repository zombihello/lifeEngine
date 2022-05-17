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
#include <string>

#include "System/Package.h"
#include "Containers/String.h"
#include "Core.h"

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

	/**
	 * Serialize TOC
	 * 
	 * @param InIsSave	Is save TOC file
	 * @return Return TRUE if TOC file serialized seccussed, else return FALSE
	 */
	bool SerializeTOC( bool InIsSave = false );

	/**
	 * Get game content directory
	 * @return Return game content directory
	 */
	FORCEINLINE std::wstring GetGameContentDir() const
	{
		return FString::Format( TEXT( "%s/Content/" ), appGameDir().c_str() );
	}

	/**
	 * Get engine content directory
	 * @return Return engine content directory
	 */
	FORCEINLINE std::wstring GetEngineContentDir() const
	{
		return FString::Format( TEXT( "%s/Engine/Content/" ), appBaseDir().c_str() );
	}

private:
	/**
	 * Add asset to data base
	 * @param InAsset		Asset
	 */
	void AddAsset( FAsset* InAsset );

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
	 * Add TOC entries
	 * @param InRootDir		Root directory
	 */
	void AddTOCEntries( const std::wstring& InRootDir );

	/**
	 * Typedef of map loaded assets
	 */
	typedef std::unordered_map< FPackage*, std::list< FAssetRef > >		FLoadedAssetsMap;

	FLoadedAssetsMap		loadedAssetsMap;		/**< Map of loaded assets */
};

#endif // !ASSETDATABASE_H