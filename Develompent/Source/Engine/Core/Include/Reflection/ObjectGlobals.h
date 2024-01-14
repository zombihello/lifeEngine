/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTGLOBALS_H
#define OBJECTGLOBALS_H

#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"

/**
 * @ingroup Core
 * @brief Find an object
 * 
 * Tries to find an object in memory. This will handle fully qualified paths of the form /path/packagename.object:subobject and resolve references for you.
 *
 * @param InClass			The to be found object's class
 * @param InOuter			Outer object to look inside. If this is ANY_PACKAGE it will search all in memory packages, if this is NULL then InName should start with a package name
 * @param InName			The object path to search for an object, relative to InOuter
 * @param InIsExactClass	Whether to require an exact match with the passed in class
 *
 * @return Return a pointer to the found object or NULL if none could be found
 */
CObject* FindObject( CClass* InClass, CObject* InOuter, const tchar* InName, bool InIsExactClass = false );

#endif // !OBJECTRESOURCE_H