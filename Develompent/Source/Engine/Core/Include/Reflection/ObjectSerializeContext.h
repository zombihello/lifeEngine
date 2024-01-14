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

#include "Core.h"
#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief Structure that holds the current serialization state of CObjects
 */
struct ObjectSerializeContext
{
public:
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
	 * 
	 * @param OutLoadedObject	Output array of loaded objects with appended new data
	 */
	void AppendLoadedObjectsAndEmpty( std::vector<CObject*>& OutLoadedObject )
	{
		OutLoadedObject.insert( OutLoadedObject.end(), objectsLoaded.begin(), objectsLoaded.end() );
		objectsLoaded.clear();
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

private:
	uint32						objBeginLoadCount;	/**< Count for BeginLoad multiple loads */
	uint32						importCount;		/**< Imports for EndLoad optimization */
	std::vector<CObject*>		objectsLoaded;		/**< Objects that might need preloading */
};

#endif // !OBJECTSERIALIZECONTEXT_H