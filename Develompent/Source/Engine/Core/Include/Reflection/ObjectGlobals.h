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
#include "Reflection/ObjectSerializeContext.h"

/**
 * @ingroup Core
 * @brief Find an existing package by name or create it if it doesn't exist
 * 
 * @param InOuter			The Outer object to search inside
 * @param InPackageName		The name of the package to find
 */
CObjectPackage* CreatePackage( CObject* InOuter, const tchar* InPackageName );

/**
 * @ingroup Core
 * @brief Loads a package and all contained objects
 * 
 * @param InOuter		Package to load new package into, usually NULL
 * @param InFilename	File name on disk
 * @return Return loaded package if successful, otherwise returns NULL 
 */
CObjectPackage* LoadPackage( CObjectPackage* InOuter, const tchar* InFilename );

/**
 * @ingroup Core
 * @brief Begin loading packages
 * @warning Objects may not be destroyed between BeginLoad/EndLoad call
 * 
 * @param InDebugContext	Debug context
 */
void BeginLoad( const tchar* InDebugContext = nullptr );

/**
 * @ingroup Core
 * @brief End loading packages
 */
void EndLoad();

/**
 * @ingroup Core
 * @brief Save one specific object (along with any objects it references contained within the same Outer) into a LifeEngine package
 * 
 * @param InOuter           The outer to use for the new package
 * @param InBase            The object that should be saved into the package
 * @param InTopLevelFlags   For all objects which are not referenced (either directly, or indirectly) through Base, only objects
 *							that contain any of these flags will be saved.  If 0 is specified, only objects which are referenced
 *							by Base will be saved into the package
 * @param InFilename        The name to use for the new package file
 * @return Return TRUE if the package was saved successfully, otherwise returns FALSE
 */
bool SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename );

/**
 * @ingroup Core
 * @brief Is any packages are saving
 * @return Return TRUE if any packages are saving, otherwise returns FALSE
 */
bool IsSavingPackage();

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