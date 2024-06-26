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
	DECLARE_CLASS_INTRINSIC( CField, CObject, 0, CASTCLASS_CField, TEXT( "Core" ) )

public:
	/**
	 * @brief Constructor
	 */
	CField()
		: nextField( nullptr )
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
		, nextField( nullptr )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

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
	 * @brief Set next field
	 * @param InNextField	Next field
	 */
	FORCEINLINE void SetNextField( CField* InNextField )
	{
		nextField = InNextField;
	}

	/**
	 * @brief Get next field
	 * @return Return pointer to next field, if not exist returns NULL
	 */
	FORCEINLINE CField* GetNextField() const
	{
		return nextField;
	}

protected:
	/**
	 * @brief Do any object-specific cleanup required immediately after loading an object
	 * @note This is not called for newly-created objects, and by default will always execute on the game thread
	 */
	virtual void PostLoad() override;

private:
	CField*			nextField;		/**< Next field */
};

#endif // !FIELD_H