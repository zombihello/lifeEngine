#include "Reflection/LinkerManager.h"
#include "System/ThreadingBase.h"

/*
==================
CLinkerManager::CLinkerManager
==================
*/
CLinkerManager::CLinkerManager()
	: bHasPendingCleanup( false )
{}

/*
==================
CLinkerManager::DeleteLoaders
==================
*/
void CLinkerManager::DeleteLoaders()
{
	Assert( IsInGameThread() );
	if ( bHasPendingCleanup )
	{
		bHasPendingCleanup = false;
		for ( auto it = pendingCleanupList.begin(), itEnd = pendingCleanupList.end(); it != itEnd; ++it )
		{
			delete *it;
		}

		pendingCleanupList.clear();
	}
}

/*
==================
CLinkerManager::ResetLoaders
==================
*/
void CLinkerManager::ResetLoaders( CObject* InPackage )
{
	// Top level package to reset loaders for
	CObject*	topLevelPackage = InPackage ? InPackage->GetOutermost() : nullptr;

	// Find loader linker associated with toplevel package. We do this upfront as Detach resets LinkerRoot
	if ( topLevelPackage )
	{
		// Linker to reset/detach
		Assert( IsA<CObjectPackage>( topLevelPackage ) );
		CLinkerLoad*	linkerToReset = ( ( CObjectPackage* )topLevelPackage )->GetLinker();
		if ( linkerToReset )
		{
			for ( auto it = objectLoaders.begin(), itEnd = objectLoaders.end(); it != itEnd; ++it )
			{
				// Detach LinkerToReset from other linker's import table
				CLinkerLoad*	linker = *it;
				if ( linker->GetLinkerRoot() != topLevelPackage )
				{
					std::vector<ObjectImport>&	importMap = linker->GetImports();
					for ( uint32 index = 0, count = importMap.size(); index < count; ++index )
					{
						ObjectImport&	objectImport = importMap[index];
						if ( objectImport.sourceLinker == linkerToReset )
						{
							objectImport.sourceLinker	= nullptr;
							objectImport.sourceIndex	= INDEX_NONE;
						}
					}
				}
				else
				{
					Assert( linker == linkerToReset );
				}
			}

			// Detach linker, also removes from array and sets LinkerRoot to NULL
			linkerToReset->Detach();
			DeleteLoader( linkerToReset );
			linkerToReset = nullptr;
		}
	}
	else
	{
		// We just want a copy here
		std::unordered_set<CLinkerLoad*>		linkersToDetach;
		GetLoaders( linkersToDetach );
		for ( auto it = linkersToDetach.begin(), itEnd = linkersToDetach.end(); it != itEnd; ++it )
		{
			// Detach linker, also removes from array and sets LinkerRoot to NULL
			CLinkerLoad*	linker = *it;
			linker->Detach();
		}

		pendingCleanupList.insert( linkersToDetach.begin(), linkersToDetach.end() );
		bHasPendingCleanup = true;
	}
}

/*
==================
CLinkerManager::ResetLoaders
==================
*/
void CLinkerManager::ResetLoaders( const std::unordered_set<CLinkerLoad *>& InLinkerLoadSet )
{
	// Remove import references
	for ( auto it = objectLoaders.begin(), itEnd = objectLoaders.end(); it != itEnd; ++it )
	{
		// Detach LinkerToReset from other linker's import table
		CLinkerLoad*	linker = *it;
		if ( InLinkerLoadSet.find( linker ) == InLinkerLoadSet.end() )
		{
			std::vector<ObjectImport>&	importMap = linker->GetImports();
			for ( uint32 index = 0, count = importMap.size(); index < count; ++index )
			{
				ObjectImport&	objectImport = importMap[index];
				if ( InLinkerLoadSet.find( objectImport.sourceLinker ) != InLinkerLoadSet.end() )
				{
					objectImport.sourceLinker	= nullptr;
					objectImport.sourceIndex	= INDEX_NONE;
				}
			}
		}
	}

	// Detach linker, also removes from array and sets LinkerRoot to NULL
	for ( auto it = InLinkerLoadSet.begin(), itEnd = InLinkerLoadSet.end(); it != itEnd; ++it )
	{
		CLinkerLoad*	linker = *it;
		linker->Detach();
	}

	// Remove all linkers in the specified set
	pendingCleanupList.insert( InLinkerLoadSet.begin(), InLinkerLoadSet.end() );
	bHasPendingCleanup = true;
}