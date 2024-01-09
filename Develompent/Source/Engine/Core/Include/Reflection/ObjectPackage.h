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
	 * @brief Set GUID of this package
	 * @param InNewGuid		A new guid for this package
	 */
	FORCEINLINE void SetGuid( const CGuid& InNewGuid )
	{
		guid = InNewGuid;
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

private:
	bool				bDirty;					/**< Is the package dirty and need to save on disk */
	bool				bHasBeenFullyLoaded;	/**< Whether this package has been fully loaded (aka had all it's exports created) at some point */
	CGuid				guid;					/**< GUID of package if it was loaded from disk */
	class CLinkerLoad*	linkerLoad;				/**< Linker load associated with this package */
};

#endif // !OBJECTPACKAGE_H