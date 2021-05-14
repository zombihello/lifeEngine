/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include "Core.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Object reference counting class
 */
class RefCounted
{
public:
	/**
	 * @brief Constructor
	 */
							RefCounted();

	/**
	 * @brief Destructor
	 */
	virtual					~RefCounted();

	/**
	 * @brief Increment reference count
	 */
	FORCEINLINE void		AddRef()					
	{ 
		++countReferences; 
	}

	/**
	 * @brief Decrement reference count and delete self if no more references
	 */
	void					ReleaseRef();

	/**
	 * @brief Get reference count
	 * @return Return reference count
	 */
	FORCEINLINE uint32		GetRefCount() const		
	{ 
		return countReferences; 
	}

private:
	uint32			countReferences;			/**< Count references on object */
};

#endif // !REFCOUNTED_H
