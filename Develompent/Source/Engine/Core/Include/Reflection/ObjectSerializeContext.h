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

private:
	uint32						objBeginLoadCount;	/**< Count for BeginLoad multiple loads */
	std::vector<CObject*>		objectsLoaded;		/**< Objects that might need preloading */
};

#endif // !OBJECTSERIALIZECONTEXT_H