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
#include "Reflection/ObjectSerializeContext.h"
#include "System/BaseTargetPlatform.h"

/**
 * @ingroup Core
 * @brief The character used to separate a subobject root from its subobjects in a path name
 */
#define SUBOBJECT_DELIMITER				TEXT( ":" )

/**
 * @ingroup Core
 * @brief The character used to separate a subobject root from its subobjects in a path name. Char version
 */
#define SUBOBJECT_DELIMITER_CHAR		':'

/**
 * @ingroup Core
 * @brief Package for contains CObjects
 */
class CObjectPackage : public CObject
{
	DECLARE_CLASS( CObjectPackage, CObject, 0, 0, TEXT( "Core" ) )

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
	 * @return Return an existing package or create it if it doesn't exist
	 */
	static CObjectPackage* CreatePackage( CObject* InOuter, const tchar* InPackageName );

	/**
	 * @brief Loads a package and all contained objects
	 *
	 * @param InOuter		Package to load new package into, usually NULL
	 * @param InFilename	File name on disk
	 * @param InLoadFlags	Flags controlling loading behavior (see ELoadFlags)
	 * @return Return loaded package if successful, otherwise returns NULL
	 */
	static CObjectPackage* LoadPackage( CObjectPackage* InOuter, const tchar* InFilename, uint32 InLoadFlags );

	/**
	 * @brief Begin loading packages
	 * @warning Objects may not be destroyed between BeginLoadPackage/EndLoadPackage call
	 */
	static void BeginLoadPackage();

	/**
	 * @brief End loading packages
	 */
	static void EndLoadPackage();

	/**
	 * @brief Save one specific object (along with any objects it references contained within the same Outer) into a LifeEngine package
	 *
	 * @param InOuter					The outer to use for the new package
	 * @param InBase					The object that should be saved into the package
	 * @param InTopLevelFlags			For all objects which are not referenced (either directly, or indirectly) through InBase, only objects
	 *									that contain any of these flags will be saved. If 0 is specified, only objects which are referenced
	 *									by InBase will be saved into the package
	 * @param InFilename				The name to use for the new package file
	 * @param InSaveFlags				Flags to control saving (see ESaveFlags)
	 * @param InCookingTarget			The platform being saved for when cooking, or NULL if not cooking
	 * @return Return TRUE if the package was saved successfully, otherwise returns FALSE
	 */
	static bool SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename, uint32 InSaveFlags, CBaseTargetPlatform* InCookingTarget = nullptr );

	/**
	 * @brief Get the serialization context of objects
	 * @return Return the serialization context of objects
	 */
	static FORCEINLINE ObjectSerializeContext& GetObjectSerializeContext()
	{
		static ObjectSerializeContext	s_ObjectSerializeContext;
		return s_ObjectSerializeContext;
	}

	/**
	 * @brief Is any packages are saving
	 * @return Return TRUE if any packages are saving, otherwise returns FALSE
	 */
	static FORCEINLINE bool IsSavingPackage()
	{
		return bIsSavingPackage;
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

	/**
	 * @brief Set linker load associated with this package
	 * @param InLinker	Linker load for associated with this package
	 */
	FORCEINLINE void SetLinker( class CLinkerLoad* InLinker )
	{
		linkerLoad = InLinker;
	}

	/**
	 * @brief Get linker load associated with this package
	 * @return Return linker load associated with this package. If this package not associated with any linker loads returns NULL
	 */
	FORCEINLINE class CLinkerLoad* GetLinker() const
	{
		return linkerLoad;
	}

	/**
	* @brief Mark this package as being fully loaded
	*/
	FORCEINLINE void MarkAsFullyLoaded()
	{
		bHasBeenFullyLoaded = true;
	}

	/**
	* @brief Whether the package is fully loaded.
	* @return Return TRUE if fully loaded otherwise FALSE
	*/
	FORCEINLINE bool IsFullyLoaded() const
	{
		return bHasBeenFullyLoaded;
	}

	/**
	 * @brief Set package flags
	 * @param InNewFlags	New flags
	 */
	FORCEINLINE void SetPackageFlags( uint32 InNewFlags )
	{
		packageFlags = InNewFlags;
	}

	/**
	 * @brief Add package flag
	 * @param InNewFlag		New flag
	 */
	FORCEINLINE void AddPackageFlag( uint32 InNewFlag )
	{
		packageFlags |= InNewFlag;
	}

	/**
	 * @brief Remove package flag
	 * @param InFlag	Flag to remove
	 */
	FORCEINLINE void RemovePackageFlag( uint32 InFlag )
	{
		packageFlags &= ~InFlag;
	}

	/**
	 * @brief Has any package flags
	 *
	 * @param InCheckFlags		Package flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyPackageFlags( uint32 InCheckFlags ) const
	{
		return ( packageFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all package flags
	 *
	 * @param InCheckFlags		Package flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllPackageFlags( uint32 InCheckFlags ) const
	{
		return ( packageFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Get package flags
	 * @return Return package flags
	 */
	FORCEINLINE uint32 GetPackageFlags() const
	{
		return packageFlags;
	}

	/**
	 * @brief Set Path to the file that was associated with this package
	 * @param InPackagePath		Path to file
	 */
	FORCEINLINE void SetPackagePath( const std::wstring& InPackagePath )
	{
		packagePath = InPackagePath;
	}

	/**
	 * @brief Path to the file that was associated with this package
	 * @return Return path to the file that was associated with this package
	 */
	FORCEINLINE const std::wstring& GetPackagePath() const
	{
		return packagePath;
	}

private:
	static bool			bIsSavingPackage;		/**< Set while in SavePackage() to detect certain operations that are illegal while saving */
	bool				bHasBeenFullyLoaded;	/**< Whether this package has been fully loaded (aka had all it's exports created) at some point */
	bool				bDirty;					/**< Is the package dirty and need to save on disk */
	uint32				packageFlags;			/**< Package flags */
	std::wstring		packagePath;			/**< Path to the file for this package */
	class CLinkerLoad*	linkerLoad;				/**< Linker load associated with this package */
};

#endif // !OBJECTPACKAGE_H