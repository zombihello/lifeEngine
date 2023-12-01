/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTITERATOR_H
#define OBJECTITERATOR_H

#include "Reflection/Object.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectGC.h"

/**
 * @ingroup Core
 * @brief Class for iterating through all objects
 */
class CObjectIterator
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InClass			Iterate over objects of this class
	 * @param InOnlyGCObjects	Is iterate over only GC objects 
	 */
	FORCEINLINE CObjectIterator( CClass* InClass = CObject::StaticClass(), bool InOnlyGCObjects = false )
		: theClass( InClass )
		, currentIndex( InOnlyGCObjects ? GetFirstGCIndex() : INDEX_NONE )
		, exclusionFlags( OBJECT_Unreachable )
	{
		Assert( InClass );
		++*this;
	}

	/**
	 * @brief Override operator ++
	 */
	FORCEINLINE void operator++()
	{
		CObjectGC&		objectGC = CObjectGC::Get();
		while ( ++currentIndex < objectGC.allocatedObjects.size() )
		{
			CObject*	object = objectGC.allocatedObjects[currentIndex];
			if ( !object || object->HasAnyObjectFlags( exclusionFlags ) || ( theClass != CObject::StaticClass() && !IsA( object, theClass ) ) )
			{
				continue;
			}
			break;
		}
	}

	/**
	 * @brief Override operator +=
	 * @param InOffset	Offset
	 */
	FORCEINLINE void operator+=( uint32 InOffset )
	{
		while ( InOffset-- )
		{
			operator++();
		}
	}

	/**
	 * @brief Override operator *
	 * @return Return current object
	 */
	FORCEINLINE CObject* operator*() const
	{
		return GetObject();
	}

	/**
	 * @brief Override operator ->
	 * @return Return current object
	 */
	FORCEINLINE CObject* operator->() const
	{
		return GetObject();
	}

	/**
	 * @brief Override operator cast to bool
	 * @return Return TRUE if iterator is valid, otherwise returns FALSE
	 */
	FORCEINLINE operator bool() const
	{
		return currentIndex < CObjectGC::Get().allocatedObjects.size();
	}

	/**
	 * @brief Override operator !
	 * @return Return TRUE if iterator isn't valid, otherwise returns FALSE
	 */
	FORCEINLINE bool operator !() const
	{
		return !operator bool();
	}
protected:
	/**
	 * @brief Get current object
	 * @return Return current object
	 */
	FORCEINLINE CObject* GetObject() const
	{
		return CObjectGC::Get().allocatedObjects[currentIndex];
	}

	/**
	 * @brief Get first GC object index
	 * @return Return first GC object index
	 */
	FORCEINLINE uint32 GetFirstGCIndex() const
	{
		return CObjectGC::Get().firstGCIndex;
	}

	CClass*			theClass;		/**< We iterate over objects of this class */
	uint32			currentIndex;	/**< Current object index */
	ObjectFlags_t	exclusionFlags;	/**< Exclusion objects with this flags */
};

/**
 * @ingroup Core
 * @brief Class for iterating through all objects which inherit from a specified base class
 */
template<class TClass> 
class TObjectIterator : public CObjectIterator
{
public:
	/**
	 * @brief Constructor
	 * @param InOnlyGCObjects	Is iterate over only GC objects
	 */
	FORCEINLINE TObjectIterator( bool InOnlyGCObjects = false )
		: CObjectIterator( T::StaticClass(), InOnlyGCObjects )
	{}

	/**
	 * @brief Override operator *
	 * @return Return current object
	 */
	FORCEINLINE TClass* operator*() const
	{
		return ( TClass* )CObjectIterator::operator*();
	}

	/**
	 * @brief Override operator ->
	 * @return Return current object
	 */
	FORCEINLINE TClass* operator->() const
	{
		return ( TClass* )CObjectIterator::operator->();
	}
};

#endif // !OBJECTITERATOR_H