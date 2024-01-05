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
	DECLARE_CLASS_INTRINSIC( CField, CObject, 0, 0, TEXT( "Core" ) )

public:
	/**
	 * @brief Constructor
	 */
	CField()
		: bBinded( false )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InFieldName	Field name
	 * @param InPackageName Package name
	 * @param InFlags		The object flags
	 */
	CField( ENativeConstructor, const CName& InFieldName, const tchar* InPackageName, ObjectFlags_t InFlags = OBJECT_None )
		: CObject( NativeConstructor, InFieldName, InPackageName, InFlags )
		, bBinded( false )
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

	/**
	 * @brief Is this field was binded
	 * @return Return TRUE if this field was binded, otherwise returns FALSE
	 */
	FORCEINLINE bool IsBinded() const
	{
		return bBinded;
	}

private:
	bool	bBinded;	/**< Is this field was binded */
};

#endif // !FIELD_H