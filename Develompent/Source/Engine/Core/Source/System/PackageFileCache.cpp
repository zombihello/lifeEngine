#include "System/PackageFileCache.h"
#include "System/System.h"

/*
==================
CPackageFileCache::CachePaths
==================
*/
void CPackageFileCache::CachePaths()
{
	// Clear lookup table, it no more need
	fileLookup.clear();

	// Loop through the specified paths and cache all packages found therein
	const std::vector<std::wstring>&	packagePaths = CSystem::Get().GetPackagePaths();
	for ( uint32 pathIndex = 0, pathCount = packagePaths.size(); pathIndex < pathCount; ++pathIndex )
	{
		CachePath( packagePaths[pathIndex].c_str() );
	}
}

/*
==================
CPackageFileCache::CachePath
==================
*/
void CPackageFileCache::CachePath( const tchar* InPath )
{
	// Find all package in and under the path
	std::vector<std::wstring>	packages;

	// Grab all packages from the path
	g_FileSystem->FindFilesInDirectory( packages, InPath, true, false );

	// Cache all found packages
	for ( uint32 packageIndex = 0, packageCount = packages.size(); packageIndex < packageCount; ++packageIndex )
	{
		CachePackage( packages[packageIndex].c_str() );
	}
}

/*
==================
CPackageFileCache::CachePackage
==================
*/
bool CPackageFileCache::CachePackage( const tchar* InFilename, bool InIsAllowOverride /* = false */ )
{
	CFilename		packageFile( InFilename );
	std::wstring	packageName		= L_Strlwr( packageFile.GetBaseFileName() );
	auto			existingEntry	= fileLookup.find( packageName );
	if ( !InIsAllowOverride && existingEntry != fileLookup.end() )
	{
		std::wstring	tmpBuffer;
		L_MakeAbsolutePath( existingEntry->second, tmpBuffer, TEXT( "" ), false );
		const CFilename	fullExistingEntry	= tmpBuffer;
		L_MakeAbsolutePath( packageFile.GetFullPath(), tmpBuffer, TEXT( "" ), false );
		const CFilename	fullNewPackageFile	= tmpBuffer;
		
		// If the expanded existing entry is the same as the old, ignore the cache request
		if ( fullNewPackageFile.GetBaseFileName() == fullExistingEntry.GetBaseFileName() )
		{
			return true;
		}

		return false;
	}

	fileLookup.insert( std::make_pair( packageName, packageFile.GetFullPath() ) );
	return true;
}

/*
==================
CPackageFileCache::GetPackageFileList
==================
*/
std::vector<std::wstring> CPackageFileCache::GetPackageFileList() const
{
	std::vector<std::wstring>	result;
	for ( auto it = fileLookup.begin(), itEnd = fileLookup.end(); it != itEnd; ++it )
	{
		result.emplace_back( it->second );
	}

	return result;
}