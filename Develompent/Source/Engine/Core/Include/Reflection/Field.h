/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FIELD_H
#define FIELD_H

#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief Base class to describe reflection
 */
class CField : public CObject
{
	DECLARE_CLASS_INTRINSIC( CField, CObject, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CField()
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InFieldName	Field name
	 * @param InFlags		The object flags
	 */
	CField( const CName& InFieldName, ObjectFlags_t InFlags = OBJECT_None )
		: CObject( InFieldName, InFlags )
	{}

	/**
	 * @brief Add property
	 * @param InProperty	Property
	 */
	virtual void AddProperty( class CProperty* InProperty );

	/**
	 * @brief Add function
	 * @param InFunction	Function
	 */
	virtual void AddFunction( class CFunction* InFunction );

	/**
	 * @brief Bind field
	 */
	virtual void Bind();

	/**
	 * @brief Get owner class
	 * @return Return owner class
	 */
	class CClass* GetOwnerClass() const;
};

#endif // !FIELD_H