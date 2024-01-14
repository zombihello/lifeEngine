/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PACKAGEFILECACHE_H
#define PACKAGEFILECACHE_H

#include <string>
#include <unordered_map>

#include "Containers/String.h"
#include "System/BaseFileSystem.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Cache of path to package files
 */
class CPackageFileCache
{
public:
	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CPackageFileCache& Get()
	{
		static CPackageFileCache	packageFileCache;
		return packageFileCache;
	}

	/**
	 * @brief Cache all packages in engine and game directory
	 */
	void CachePaths();

	/**
	 * @brief Cache all packages in this path and any directories under it
	 * @param InPath	The path to look in for packages
	 */
	void CachePath( const tchar* InPath );

	/**
	 * @brief Cache package
	 * 
	 * @param InFilename			Path to package file
	 * @param InIsAllowOverride		Is allow override existing mapping with the new path info
	 * @return Return TRUE if the specified path name was successfully added to the lookup table. FALSE if an entry already existed for this package
	 */
	bool CachePackage( const tchar* InFilename, bool InIsAllowOverride = false );

	/**
	 * @brief Find package file by its name
	 * 
	 * @param InPackageName		Package name
	 * @param OutFileName		Output package file
	 * @return Return TRUE if package file was found, otherwise returns FALSE
	 */
	FORCEINLINE bool FindPackageFile( const tchar* InPackageName, std::wstring& OutFileName )
	{
		bool	bFoundFile		= false;
		auto	existingEntry	= fileLookup.find( CString::ToLower( CFilename( InPackageName ).GetBaseFilename() ) );
		if ( existingEntry != fileLookup.end() )
		{
			OutFileName = existingEntry->second;
			bFoundFile = true;
		}

		return bFoundFile;
	}

	/**
	 * @brief Clears all entries from the package file cache
	 */
	FORCEINLINE void ClearPackageCache()
	{
		fileLookup.clear();
	}

private:
	std::unordered_map<std::wstring, std::wstring>		fileLookup;	/**< Map of package files */
};

#endif // !PACKAGEFILECACHE_H