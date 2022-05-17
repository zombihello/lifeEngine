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
#include "System/ThreadingBase.h"

/**
 * @ingroup Core
 * @brief Object reference counting class
 */
class LE_DEPRECATED( 0.4.0, "FRefCounted is deprecated, use TSharedPtr and TWeakPtr." ) FRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
							FRefCounted();

	/**
	 * @brief Destructor
	 */
	virtual					~FRefCounted();

	/**
	 * @brief Increment reference count
	 */
	FORCEINLINE void		AddRef()					
	{ 
		appInterlockedIncrement( ( int32* )&countReferences );
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
