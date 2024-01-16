/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LINKERMANAGER_H
#define LINKERMANAGER_H

#include <unordered_set>

#include "Core.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief Manager of linker loaders
 */
class CLinkerManager
{
public:
	/**
	 * @brief Constructor
	 */
	CLinkerManager();

	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CLinkerManager& Get()
	{
		static CLinkerManager	s_LinkerManager;
		return s_LinkerManager;
	}

	/**
	 * @brief Get all object loaders
	 * @param OutLoaders	Output all object loaders
	 */
	FORCEINLINE void GetLoaders( std::unordered_set<CLinkerLoad*>& OutLoaders )
	{
		OutLoaders = objectLoaders;
	}

	/**
	 * @brief Get all object loaders and empty it
	 * @param OutLoaders	Output all object loaders
	 */
	FORCEINLINE void GetLoadersAndEmpty( std::unordered_set<CLinkerLoad*>& OutLoaders )
	{
		OutLoaders = std::move( objectLoaders );
		Assert( objectLoaders.empty() );
	}

	/**
	 * @brief Get all object loaders with new imports and empty it
	 * @param OutLoaders	Output all object loaders with new imports
	 */
	FORCEINLINE void GetLoadersWithNewImportsAndEmpty( std::unordered_set<CLinkerLoad*>& OutLoaders )
	{
		OutLoaders = std::move( loadersWithNewImports );
		Assert( loadersWithNewImports.empty() );
	}

	/**
	 * @brief Add a new object loader
	 * @param InLinkerLoad	Object loader
	 */
	FORCEINLINE void AddLoader( CLinkerLoad* InLinkerLoad )
	{
		objectLoaders.insert( InLinkerLoad );
	}

	/**
	 * @brief Add a new object loader with new imports
	 * @param InLinkerLoad	Object loader
	 */
	FORCEINLINE void AddLoaderWithNewImports( CLinkerLoad* InLinkerLoad )
	{
		loadersWithNewImports.insert( InLinkerLoad );
	}

	/**
	 * @brief Remove an object loader from object loaders array and loaders with new imports array
	 * @param InLinkerLoad	Object loader
	 */
	FORCEINLINE void RemoveLoaderFromObjectLoadersAndLoadersWithNewImports( CLinkerLoad* InLinkerLoad )
	{
		objectLoaders.erase( InLinkerLoad );
		loadersWithNewImports.erase( InLinkerLoad );
	}

	/**
	 * @brief Adds a object loader to deferred cleanup list
	 * @param InLinkerLoad	Object loader
	 */
	FORCEINLINE void DeleteLoader( CLinkerLoad* InLinkerLoad )
	{
		if ( InLinkerLoad && pendingCleanupList.find( InLinkerLoad ) == pendingCleanupList.end() )
		{
			pendingCleanupList.insert( InLinkerLoad );
			bHasPendingCleanup = true;
		}
	}

	/**
	 * @brief Dissociates all linker import This currently needs to happen as the referred 
	 * objects might be destroyed at any time
	 */
	void DissociateImports();

	/**
	 * @brief Deletes all object loaders that finished loading
	 */
	void DeleteLoaders();

	/**
	 * @brief Remove references to the linker for the given package and delete the linker
	 * Can be called after the package has finished loading
	 * 
	 * @param InPackage		package
	 */
	void ResetLoaders( CObject* InPackage );

	/**
	 * @brief Empty the loaders from the specified set
	 * @param InLinkerLoadSet	Linker load set
	 */
	void ResetLoaders( const std::unordered_set<CLinkerLoad*>& InLinkerLoadSet );

private:
	std::unordered_set<CLinkerLoad*>	objectLoaders;			/**< Map of packages to their open linkers */
	std::unordered_set<CLinkerLoad*>	loadersWithNewImports;	/**< List of loaders that have new imports */
	std::unordered_set<CLinkerLoad*>	pendingCleanupList;		/**< List of linkers to delete */
	bool								bHasPendingCleanup;		/**< Has pending cleanup */
};

#endif // !LINKERMANAGER_H