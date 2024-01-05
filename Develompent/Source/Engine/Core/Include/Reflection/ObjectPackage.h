/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTPACKAGE_H
#define OBJECTPACKAGE_H

#include "Misc/Guid.h"
#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief the character used to separate a subobject root from its subobjects in a path name
 */
#define SUBOBJECT_DELIMITER				TEXT( ":" )

/**
 * @ingroup Core
 * @brief the character used to separate a subobject root from its subobjects in a path name. Char version
 */
#define SUBOBJECT_DELIMITER_CHAR		':'

/**
 * @ingroup Core
 * @brief Package for contains CObjects
 */
class CObjectPackage : public CObject
{
	DECLARE_CLASS_INTRINSIC( CObjectPackage, CObject, 0, 0, TEXT( "Core" ) )

public:
	/**
	 * @brief Constructor
	 */
	CObjectPackage();

	/**
	 * @brief Find an existing package by name or create it if it doesn't exist
	 * 
	 * @param InOuter			The Outer object to search inside
	 * @param InPackageName		The name of the package to find
	 */
	static CObjectPackage* CreatePackage( CObject* InOuter, const tchar* InPackageName );

    /**
     * @brief Loads a package and all contained objects
     * 
     * @param InOuter       Package to load new package into, usually NULL
     * @param InFilename	Name of file on disk
     * @return Return loaded package if successful, otherwise returns NULL 
     */
    static CObjectPackage* LoadPackage( CObjectPackage* InOuter, const tchar* InFilename );

    /**
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
    static bool SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename );

	/**
	 * @brief Is now saving the package
	 * @return Return TRUE if now saving the package, otherwise returns FALSE
	 */
	static FORCEINLINE bool IsSavingPackage()
	{
		return bIsSavingPackage;
	}

	/**
	 * @brief Get GUID of this package
	 * @return Return GUID of this package. If it wasn't loaded from dist returns invalid GUID
	 */
	FORCEINLINE const CGuid& GetGuid() const
	{
		return guid;
	}

	/**
	 * @brief Is this package dirty
	 * @return Return TRUE if this package dirty, otherwise returns FALSE
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
	}

	/**
	 * @brief Marks/Unmarks the package's bDirty flag
	 * @param InIsDirty		Is this package dirty
	 */
	FORCEINLINE void SetDirtyFlag( bool InIsDirty )
	{
		bDirty = InIsDirty;
	}

private:
	static bool		bIsSavingPackage;	/**< Set while in SavePackage() to detect certain operations that are illegal while saving */
	bool			bDirty;				/**< Is the package dirty and need to save on disk */
	CGuid			guid;				/**< GUID of package if it was loaded from disk */
};

#endif // !OBJECTPACKAGE_H