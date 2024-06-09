/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTHASH_H
#define OBJECTHASH_H

#include "System/Name.h"
#include "Reflection/ObjectMacros.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Find an object, fast version
 * 
 * Fast version of FindObject that relies on the passed in CName being the object name without any group/package qualifiers.
 * This will only find top level packages or subobjects nested directly within a passed in outer
 *
 * @param InClass			The to be found object's class
 * @param InOuter			Outer object to look inside, if NULL this will only look for top level packages
 * @param InName			Object name to look for relative to InOuter
 * @param InIsExactClass	Whether to require an exact match with the passed in class
 * @param InIsAnyPackage	Whether to look in any package
 * @param InExclusiveFlags	Ignores objects that contain any of the specified exclusive flags
 * @return Return a pointer to the found object or NULL if none could be found
 */
class CObject* FindObjectFast( class CClass* InClass, class CObject* InOuter, const CName& InName, bool InIsExactClass = false, bool InIsAnyPackage = false, ObjectFlags_t InExclusiveFlags = OBJECT_None );

/**
 * @brief Add an object to the name hash tables
 * @param InObject		Object to add to the hash tables
 */
void HashObject( class CObject* InObject );

/**
 * @brief Remove an object to the name hash tables
 * @param InObject		Object to remove from the hash tables
 */
void UnhashObject( class CObject* InObject );

/**
 * @ingroup Core
 * @brief Find an object, fast version
 * 
 * Fast version of FindObject that relies on the passed in CName being the object name without any group/package qualifiers.
 * This will only find top level packages or subobjects nested directly within a passed in outer
 * 
 * @param InOuter			Outer object to look inside, if NULL this will only look for top level packages
 * @param InName			Object name to look for relative to InOuter
 * @param InIsExactClass	Whether to require an exact match with the passed in class
 * @param InIsAnyPackage	Whether to look in any package
 * @param InExclusiveFlags	Ignores objects that contain any of the specified exclusive flags
 * @return Return a pointer to the found object or NULL if none could be found
 */
template<class TClass>
FORCEINLINE TClass* FindObjectFast( class CObject* InOuter, const CName& InName, bool InIsExactClass = false, bool InIsAnyPackage = false, ObjectFlags_t InExclusiveFlags = OBJECT_None )
{
	return ( TClass* )FindObjectFast( TClass::StaticClass(), InOuter, InName, InIsExactClass, InIsAnyPackage, InExclusiveFlags );
}

#endif // !OBJECTHASH_H