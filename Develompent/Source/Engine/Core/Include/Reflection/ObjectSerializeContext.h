/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTSERIALIZECONTEXT_H
#define OBJECTSERIALIZECONTEXT_H

#include <vector>
#include <unordered_set>

#include "Core.h"
#include "Reflection/Object.h"
#include "Reflection/LinkerLoad.h"

/**
 * @ingroup Core
 * @brief Structure that holds the current serialization state of CObjects
 */
struct ObjectSerializeContext
{
public:
	friend class CObjectPackage;

	/**
	 * @brief Constructor
	 */
	ObjectSerializeContext()
		: objBeginLoadCount( 0 )
		, importCount( 0 )
	{}

	/**
	 * @brief Checks if object loading has started
	 * @return Return TRUE if object loading has started, otherwise returns FALSE
	 */
	FORCEINLINE bool HasStartedLoading() const
	{
		return objBeginLoadCount > 0;
	}

	/**
	 * @brief Get load count objects now
	 * @return Return load count objects now
	 */
	FORCEINLINE uint32 GetBeginLoadCount() const
	{
		return objBeginLoadCount;
	}

	/**
	 * @brief Increment begin load count objects
	 * @return Return begin load count objects
	 */
	FORCEINLINE uint32 IncrementBeginLoadCount()
	{
		return ++objBeginLoadCount;
	}

	/**
	 * @brief Decrement begin load count objects
	 * @return Return begin load count objects
	 */
	FORCEINLINE uint32 DecrementBeginLoadCount()
	{
		Assert( HasStartedLoading() );
		return --objBeginLoadCount;
	}

	/**
	 * @brief Add a new loaded object
	 * @param InObject	Object
	 */
	FORCEINLINE void AddLoadedObject( CObject* InObject )
	{
		objectsLoaded.push_back( InObject );
	}

	/**
	 * @brief Append loaded objects and empty the one
	 * @param OutLoadedObject	Output array of loaded objects with appended new data
	 */
	void AppendLoadedObjectsAndEmpty( std::vector<CObject*>& OutLoadedObject )
	{
		OutLoadedObject.insert( OutLoadedObject.end(), objectsLoaded.begin(), objectsLoaded.end() );
		objectsLoaded.clear();
	}

	/**
	 * @brief Add a delayed linker to close package
	 * @param InLinker	Delayed linker to close package
	 */
	FORCEINLINE void AddDelayedLinkerClosePackage( CLinkerLoad* InLinker )
	{
		delayedLinkerClosePackages.insert( InLinker );
	}

	/**
	 * @brief Remove a delayed linker to close package from list
	 * @param InLinker	Delayed linker to close package
	 */
	FORCEINLINE void RemoveDelayedLinkerClosePackage( CLinkerLoad* InLinker )
	{
		delayedLinkerClosePackages.erase( InLinker );
	}

	/**
	 * @brief Get list of delayed linker to close package and empty it
	 * @param OutDelayedLinkerClosePackages		Output list with delayed linker to close package
	 */
	FORCEINLINE void GetDelayedLinkerClosePackagesAndEmpty( std::unordered_set<CLinkerLoad*>& OutDelayedLinkerClosePackages )
	{
		OutDelayedLinkerClosePackages = std::move( delayedLinkerClosePackages );
	}

	/**
	 * @brief Has loaded objects
	 * @return Return TRUE if has loaded objects, otherwise returns FALSE
	 */
	FORCEINLINE bool HasLoadedObjects() const
	{
		return !objectsLoaded.empty();
	}

	/**
	 * @brief Get number of loaded objects
	 * @return Return number of loaded objects
	 */
	FORCEINLINE uint32 GetNumObjectsLoaded() const
	{
		return objectsLoaded.size();
	}

	/**
	 * @brief Increment import count
	 * @return Return new import count
	 */
	FORCEINLINE uint32 IncrementImportCount()
	{
		return ++importCount;
	}

	/**
	 * @brief Reset import count
	 */
	FORCEINLINE void ResetImportCount()
	{
		importCount = 0;
	}

	/**
	 * @brief Has pending imports
	 * @Return TRUE if has pending imports, otherwise returns FALSE
	 */
	FORCEINLINE bool HasPendingImports() const
	{
		return importCount > 0;
	}

	/**
	 * @brief Is routing PostLoad
	 * @return Return TRUE when we are routing ConditionalPostLoad/PostLoad to objects, otherwise returns FALSE
	 */
	FORCEINLINE bool IsRoutingPostLoad() const
	{
		return bRoutingPostLoad;
	}

#if !SHIPPING_BUILD
	/**
	 * @brief Add an object where must be to call PostLoad
	 * @param InObject	Object where must be to call PostLoad
	 */
	FORCEINLINE void AddDebugPostLoad( CObject* InObject )
	{
		debugPostLoad.insert( InObject );
	}

	/**
	 * @brief Remove an object from PostLoad chain
	 * @param InObject	Object
	 */
	FORCEINLINE void RemoveDebugPostLoad( CObject* InObject )
	{
		debugPostLoad.erase( InObject );
	}

	/**
	 * @brief Is debug postload chain contains an object
	 * 
	 @param InObject	Object to check
	 @return Return TRUE if postload chain contains InObject, otherwise returns FALSE
	 */
	FORCEINLINE bool ContainsDebugPostLoad( CObject* InObject ) const
	{
		return debugPostLoad.find( InObject ) != debugPostLoad.end();
	}
#endif // !SHIPPING_BUILD

private:
	bool								bRoutingPostLoad;			/**< TRUE when we are routing ConditionalPostLoad/PostLoad to objects */
	uint32								objBeginLoadCount;			/**< Count for BeginLoad multiple loads */
	uint32								importCount;				/**< Imports for EndLoad optimization */
	std::vector<CObject*>				objectsLoaded;				/**< Objects that might need preloading */
	std::unordered_set<CLinkerLoad*>	delayedLinkerClosePackages;	/**< List of linkers that we want to close the loaders for (to free file handles). Needs to be delayed until EndLoadPackage is called with objBeginLoadCount of 0 */
	
#if !SHIPPING_BUILD
	std::unordered_set<CObject*>		debugPostLoad;				/**< Used to verify that the Super::PostLoad chain is intact */
#endif // !SHIPPING_BUILD
};

#endif // !OBJECTSERIALIZECONTEXT_H