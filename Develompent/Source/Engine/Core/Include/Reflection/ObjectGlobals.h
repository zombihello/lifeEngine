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
 * @brief Resolve a package and name
 * 
 * @param InOutPackage		Outer package
 * @param InOutName			Object name
 * @param InIsCreate		Is need create a package
 * @return Return TRUE if a package and name was resolved, otherwise returns FASLE
 */
bool ResolvePackageAndName( CObject*& InOutPackage, std::wstring& InOutName, bool InIsCreate );

/**
 * @ingroup Core
 * @brief Find an object
 * 
 * Tries to find an object in memory. This will handle fully qualified paths of the form packagename.object:subobject and resolve references for you.
 *
 * @param InClass			The to be found object's class
 * @param InOuter			Outer object to look inside. If this is ANY_PACKAGE it will search all in memory packages, if this is NULL then InName should start with a package name
 * @param InName			The object path to search for an object, relative to InOuter
 * @param InIsExactClass	Whether to require an exact match with the passed in class
 * @return Return a pointer to the found object or NULL if none could be found
 */
CObject* FindObject( CClass* InClass, CObject* InOuter, const tchar* InName, bool InIsExactClass = false );

/**
 * @ingroup Core
 * @brief Find or load an object
 * 
 * @param InClass							The class of the object to be loaded
 * @param InOuter							An optional object to narrow where to find/load the object from
 * @param InName							Name of the object. If it's not fully qualified, InOuter and/or InFilename will be needed
 * @param InFilename						An optional file to load from (or find in the file's package object)
 * @param InLoadFlags						Flags controlling how to handle loading from disk (see ELoadFlags)
 * @param InIsAllowObjectReconciliation		Whether to allow the object to be found via FindObject in the case of seek free loading
 * @return Return the object that was loaded or found, NULL for a failure
 */
CObject* LoadObject( CClass* InClass, CObject* InOuter, const tchar* InName, const tchar* InFilename = nullptr, uint32 InLoadFlags = LOAD_None, bool InIsAllowObjectReconciliation = true );

/**
 * @ingroup Core
 * @brief Version of LoadObject() that will load classes
 * 
 * @param InBaseClass						The base class of the class to be loaded
 * @param InOuter							An optional object to narrow where to find/load the class from
 * @param InName							Name of the class. If it's not fully qualified, InOuter and/or InFilename will be needed
 * @param InFilename						An optional file to load from (or find in the file's package object)
 * @param InLoadFlags						Flags controlling how to handle loading from disk (see ELoadFlags)
 * @param InIsAllowObjectReconciliation		Whether to allow the class to be found via FindObject in the case of seek free loading
 * @return Return the class that was loaded or found, NULL for a failure
 */
CClass* LoadClass( CClass* InBaseClass, CObject* InOuter, const tchar* InName, const tchar* InFilename = nullptr, uint32 InLoadFlags = LOAD_None );

/**
 * @ingroup Core
 * @brief Create a new instance of an object
 *
 * @param InClass	The class of the object to create
 * @param InOuter   The object to create this object within
 * @param InName	The name to give the new object
 * @param InFlags   The object flags
 * @return Return allocated object, it will be fully initialized
 */
FORCEINLINE CObject* NewObject( CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None )
{
	return CObject::StaticConstructObject( InClass, InOuter, InName, InFlags );
}

/**
 * @ingroup Core
 * @brief Create a new instance of an object
 *
 * @param InOuter   The object to create this object within
 * @param InName	The name to give the new object
 * @param InFlags   The object flags
 * @return Return allocated object, it will be fully initialized
 */
template<class TClass>
FORCEINLINE TClass* NewObject( CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None )
{
	return ( TClass* )CObject::StaticConstructObject( TClass::StaticClass(), InOuter, InName, InFlags );
}

/**
 * @ingroup Core
 * @brief Find an object
 *
 * Tries to find an object in memory. This will handle fully qualified paths of the form packagename.object:subobject and resolve references for you.
 *
 * @param InOuter			Outer object to look inside. If this is ANY_PACKAGE it will search all in memory packages, if this is NULL then InName should start with a package name
 * @param InName			The object path to search for an object, relative to InOuter
 * @param InIsExactClass	Whether to require an exact match with the passed in class
 * @return Return a pointer to the found object or NULL if none could be found
 */
template<class TClass>
FORCEINLINE TClass* FindObject( CObject* InOuter, const tchar* InName, bool InIsExactClass = false )
{
	return ( TClass* )FindObject( TClass::StaticClass(), InOuter, InName, InIsExactClass );
}

/**
 * @ingroup Core
 * @brief Find or load an object
 *
 * @param InOuter							An optional object to narrow where to find/load the object from
 * @param InName							Name of the object. If it's not fully qualified, InOuter and/or InFilename will be needed
 * @param InFilename						An optional file to load from (or find in the file's package object)
 * @param InLoadFlags						Flags controlling how to handle loading from disk (see ELoadFlags)
 * @param InIsAllowObjectReconciliation		Whether to allow the object to be found via FindObject in the case of seek free loading
 * @return Return the object that was loaded or found, NULL for a failure
 */
template<class TClass>
FORCEINLINE TClass* LoadObject( CObject* InOuter, const tchar* InName, const tchar* InFilename = nullptr, uint32 InLoadFlags = LOAD_None, bool InIsAllowObjectReconciliation = true )
{
	return ( TClass* )LoadObject( TClass::StaticClass(), InOuter, InName, InFilename, InLoadFlags, InIsAllowObjectReconciliation );
}

#endif // !OBJECTRESOURCE_H