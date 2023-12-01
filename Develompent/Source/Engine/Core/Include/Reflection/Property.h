/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PROPERTY_H
#define PROPERTY_H

#include "Math/Color.h"
#include "Math/Rotator.h"
#include "Misc/RefCountPtr.h"
#include "System/Name.h"
#include "System/Package.h"
#include "Reflection/Field.h"
#include "Reflection/Class.h"
#include "Reflection/Enum.h"

/**
 * @ingroup Core
 * @brief Macro for define Cpp properties
 * 
 * @param InClass	Class name
 * @param InName	Property name
 */
#define CPP_PROPERTY( InClass, InName )	CppProperty, STRUCT_OFFSET( InClass, InName )

/**
 * @ingroup Core
 * @brief Enumeration property flags
 */
enum EPropertyFlags
{
	CPF_None			= 0,		/**< None */
	CPF_Const			= 1 << 0,	/**< Property is constant */
	CPF_EditorOnly		= 1 << 1,	/**< Property only for the editor */
	CPF_Edit			= 1 << 2,	/**< Property is user-settable in the editor */
	CPF_EditFixedSize	= 1 << 3,	/**< Indicates that elements of an array can be modified, but its size cannot be changed */
	CPF_EditConst		= 1 << 4,	/**< Property is uneditable in the editor */
};

/**
 * @ingroup Core
 * @brief Union property value
 */
union UPropertyValue
{
	/**
	 * @brief Constructor
	 */
	UPropertyValue()
		: byteValue( 0 )
		, intValue( 0 )
		, boolValue( false )
		, floatValue( 0.f )
		, colorValue( 0 )
		, objectValue( nullptr )
		, vectorValue( 0.f, 0.f, 0.f )
		, rotatorValue( 0.f, 0.f, 0.f )
		, assetValue( nullptr, nullptr )
		, arrayValue( nullptr )
		, stringValue( nullptr )
	{}

	/**
	 * @brief Destructor
	 */
	~UPropertyValue()
	{}

	byte						byteValue;			/**< Byte */
	int32						intValue;			/**< Int */
	bool						boolValue;			/**< Bool */
	float						floatValue;			/**< Float */
	CColor						colorValue;			/**< Color */
	class CObject*				objectValue;		/**< Object */
	Vector						vectorValue;		/**< Vector */
	CRotator					rotatorValue;		/**< Rotator */
	TAssetHandle<CAsset>		assetValue;			/**< Asset */
	std::vector<byte>*			arrayValue;			/**< Array */
	std::wstring*				stringValue;		/**< String */
};

/**
 * @ingroup Core
 * @brief The base class all of CObject's properties
 */
class CProperty : public CField
{
	DECLARE_CLASS_INTRINSIC( CProperty, CField, CLASS_Abstract, CASTCLASS_CProperty )
	DECLARE_WITHIN_CLASS( CField )

public:
	/**
	 * @brief Constructor
	 */
	CProperty();

	/**
	 * @brief Constructor
	 * 
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 );

	/**
	 * @brief Get category
	 * @return Return category
	 */
	FORCEINLINE const CName& GetCategory() const
	{
		return category;
	}

	/**
	 * @brief Get description
	 * @return Return description
	 */
	FORCEINLINE const std::wstring& GetDescription() const
	{
		return description;
	}

	/**
	 * @brief Set category
	 * @param InNewCategory		New category
	 */
	FORCEINLINE void SetName( const CName& InNewCategory )
	{
		category = InNewCategory;
	}

	/**
	 * @brief Set description
	 * @param InNewDescription		New description
	 */
	FORCEINLINE void SetDescription( const std::wstring& InNewDescription )
	{
		description = InNewDescription;
	}

	/**
	 * @brief Get offset to property
	 * @return Return offset to property
	 */
	FORCEINLINE uint32 GetOffset() const
	{
		return offset;
	}

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const		PURE_VIRTUAL( CProperty::GetElementSize, return 0; );

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const		PURE_VIRTUAL( CProperty::GetMinAlignment, return 1; );

	/**
	 * @brief Is the property contains object reference
	 * @retrun Return TRUE if property (or sub-properties) contain  a CObject reference, otherwise returns FALSE
	 */
	virtual bool IsContainsObjectReference() const;

	/**
	 * @brief Emits tokens used by realtime garbage collection code to passed in InReferenceTokenStream
	 * Emits tokens used by realtime garbage collection code to passed in ReferenceTokenStream. The offset emitted is relative
	 * to the passed in BaseOffset which is used by e.g. arrays of structs
	 * 
	 * @param InReferenceTokenStream	Reference token stream
	 * @param InBaseOffset				Base offset
	 */
	virtual void EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const;

	/**
	 * @brief Get property flags
	 * @return Return property flags
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Get count of persistent variables
	 * @return Return count of persistent variables
	 */
	FORCEINLINE uint32 GetArraySize() const
	{
		return arraySize;
	}

	/**
	 * @brief Get property flags
	 * @param InFlags	New flags
	 */
	FORCEINLINE void SetFlags( uint32 InFlags )
	{
		flags |= InFlags;
	}

	/**
	 * @brief Clear flags
	 * @param InFlags	New flags
	 */
	FORCEINLINE void ClearFlags( uint32 InFlags )
	{
		flags &= ~InFlags;
	}

	/**
	 * @brief Has any flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all class flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllFlags( uint32 InCheckFlags ) const
	{
		return ( flags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Is property only for the editor
	 * @return Return TRUE if property has the flag CPF_EditorOnly
	 */
	FORCEINLINE bool IsEditorOnlyProperty() const
	{
		return HasAnyFlags( CPF_EditorOnly );
	}

	/**
	 * @brief Get property value
	 * 
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const			PURE_VIRTUAL( CProperty::GetPropertyValue, return false; );

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )			PURE_VIRTUAL( CProperty::SetPropertyValue, return false; );

protected:
	CName			category;		/**< Category */
	std::wstring	description;	/**< Description */
	uint32			offset;			/**< Offset */
	uint32			flags;			/**< Flags */
	uint32			arraySize;		/**< Count of persistent variables */
};

/**
 * @ingroup Core
 * @brief Byte property
 */
class CByteProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CByteProperty, CProperty, 0, CASTCLASS_CByteProperty )

public:
	/**
	 * @brief Constructor
	 */
	CByteProperty() 
		: cenum( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InEnum			If InEnum not null then it's mean this property is enum
	 * @param InArraySize		Count of persistent variables
	 */
	CByteProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CEnum* InEnum = nullptr, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, cenum( InEnum )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue & InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Get enum
	 * @return Return enum if this property is enum
	 */
	FORCEINLINE CEnum* GetEnum() const
	{
		return cenum;
	}

private:
	CEnum*		cenum;		/**< Enum */
};

/**
 * @ingroup Core
 * @brief Int property
 */
class CIntProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CIntProperty, CProperty, 0, CASTCLASS_CIntProperty )

public:
	/**
	 * @brief Constructor
	 */
	CIntProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CIntProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue & InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

/**
 * @ingroup Core
 * @brief Float property
 */
class CFloatProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CFloatProperty, CProperty, 0, CASTCLASS_CFloatProperty )

public:
	/**
	 * @brief Constructor
	 */
	CFloatProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CFloatProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue & InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

/**
 * @ingroup Core
 * @brief Bool property
 */
class CBoolProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CBoolProperty, CProperty, 0, CASTCLASS_CBoolProperty )

public:
	/**
	 * @brief Constructor
	 */
	CBoolProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CBoolProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue & InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

/**
 * @ingroup Core
 * @brief Color property
 */
class CColorProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CColorProperty, CProperty, 0, CASTCLASS_CColorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CColorProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CColorProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

/**
 * @ingroup Core
 * @brief Object property
 */
class CObjectProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CObjectProperty, CProperty, 0, CASTCLASS_CObjectProperty )

public:
	/**
	 * @brief Constructor
	 */
	CObjectProperty() 
		: propertyClass( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InPropertyClass	Property class
	 * @param InArraySize		Count of persistent variables
	 */
	CObjectProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CClass* InPropertyClass, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, propertyClass( InPropertyClass )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Is the property contains object reference
	 * @retrun Return TRUE if property (or sub-properties) contain  a CObject reference, otherwise returns FALSE
	 */
	virtual bool IsContainsObjectReference() const override;

	/**
	 * @brief Emits tokens used by realtime garbage collection code to passed in InReferenceTokenStream
	 * Emits tokens used by realtime garbage collection code to passed in ReferenceTokenStream. The offset emitted is relative
	 * to the passed in BaseOffset which is used by e.g. arrays of structs
	 *
	 * @param InReferenceTokenStream	Reference token stream
	 * @param InBaseOffset				Base offset
	 */
	virtual void EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const override;

	/**
	 * @brief Get property class
	 * @return Return property class
	 */
	FORCEINLINE CClass* GetPropertyClass() const
	{
		return propertyClass;
	}

private:
	CClass*		propertyClass;		/**< Property class */
};

/**
 * @ingroup Core
 * @brief Vector property
 */
class CVectorProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CVectorProperty, CProperty, 0, CASTCLASS_CVectorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CVectorProperty() 
		: defaultComponentValue( 0.f )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset					Offset to property
	 * @param InCategory				Category
	 * @param InDescription				Description
	 * @param InFlags					Flags (see EPropertyFlags)
	 * @param InDefaultComponentValue	Default component value
	 * @param InArraySize				Count of persistent variables
	 */
	CVectorProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, float InDefaultComponentValue = 0.f, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, defaultComponentValue( InDefaultComponentValue )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Get default component value
	 * @return Return default component value
	 */
	FORCEINLINE float GetDefaultComponentValue() const
	{
		return defaultComponentValue;
	}

private:
	float	defaultComponentValue;		/**< Default component value */
};

/**
 * @ingroup Core
 * @brief Rotator property
 */
class CRotatorProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CRotatorProperty, CProperty, 0, CASTCLASS_CRotatorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CRotatorProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CRotatorProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

/**
 * @ingroup Core
 * @brief Asset property
 */
class CAssetProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CAssetProperty, CProperty, 0, CASTCLASS_CAssetProperty )

public:
	/**
	 * @brief Constructor
	 */
	CAssetProperty() 
		: assetType( AT_Unknown )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InAssetType		Asset type
	 * @param InArraySize		Count of persistent variables
	 */
	CAssetProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, EAssetType InAssetType, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, assetType( InAssetType )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Get asset type
	 * @return Return asset type
	 */
	FORCEINLINE EAssetType GetAssetType() const
	{
		return assetType;
	}

private:
	EAssetType			assetType;		/**< Asset type */
};

/**
 * @ingroup Core
 * @brief Array property
 */
class CArrayProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CArrayProperty, CProperty, 0, CASTCLASS_CArrayProperty )

public:
	/**
	 * @brief Constructor
	 */
	CArrayProperty()
		: innerProperty( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CArrayProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, innerProperty( nullptr )
	{}

	/**
	 * @brief Add class property
	 * @param InProperty	Property
	 */
	virtual void AddProperty( class CProperty* InProperty ) override;

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Is the property contains object reference
	 * @retrun Return TRUE if property (or sub-properties) contain  a CObject reference, otherwise returns FALSE
	 */
	virtual bool IsContainsObjectReference() const override;

	/**
	 * @brief Emits tokens used by realtime garbage collection code to passed in InReferenceTokenStream
	 * Emits tokens used by realtime garbage collection code to passed in ReferenceTokenStream. The offset emitted is relative
	 * to the passed in BaseOffset which is used by e.g. arrays of structs
	 *
	 * @param InReferenceTokenStream	Reference token stream
	 * @param InBaseOffset				Base offset
	 */
	virtual void EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const override;

	/**
	 * @brief Get inner property
	 * @return Return inner property
	 */
	FORCEINLINE CProperty* GetInnerProperty() const
	{
		return innerProperty;
	}

private:
	CProperty*		innerProperty;	/**< Inner property */
};

/**
 * @ingroup Core
 * @brief Struct property
 */
class CStructProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CStructProperty, CProperty, 0, CASTCLASS_CStructProperty )

public:
	/**
	 * @brief Constructor
	 */
	CStructProperty()
		: propertyStruct( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InPropertyStruct	Property struct
	 * @param InArraySize		Count of persistent variables
	 */
	CStructProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CStruct* InPropertyStruct, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
		, propertyStruct( InPropertyStruct )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;

	/**
	 * @brief Is the property contains object reference
	 * @retrun Return TRUE if property (or sub-properties) contain  a CObject reference, otherwise returns FALSE
	 */
	virtual bool IsContainsObjectReference() const override;

	/**
	 * @brief Emits tokens used by realtime garbage collection code to passed in InReferenceTokenStream
	 * Emits tokens used by realtime garbage collection code to passed in ReferenceTokenStream. The offset emitted is relative
	 * to the passed in BaseOffset which is used by e.g. arrays of structs
	 *
	 * @param InReferenceTokenStream	Reference token stream
	 * @param InBaseOffset				Base offset
	 */
	virtual void EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const override;

	/**
	 * @brief Get property struct
	 * @return Return property struct
	 */
	FORCEINLINE CStruct* GetPropertyStruct() const
	{
		return propertyStruct;
	}

private:
	CStruct*	propertyStruct;		/**< Property struct */
};

/**
 * @ingroup Core
 * @brief String property
 */
class CStringProperty : public CProperty
{
	DECLARE_CLASS_INTRINSIC( CStringProperty, CProperty, 0, CASTCLASS_CStringProperty )

public:
	/**
	 * @brief Constructor
	 */
	CStringProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CStringProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( CppProperty, InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Get property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param OutPropertyValue		Will be filled with the value located at InObjectAddress+Offset
	 * @return Return TRUE if OutPropertyValue was filled with a property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const override;

	/**
	 * @brief Set property value
	 *
	 * @param InObjectAddress		The address of a object where the value of this property is stored
	 * @param InPropertyValue		Contains the value that should be copied into InObjectAddress+Offset
	 * @return Return TRUE if InPropertyValue was copied successfully into property value. FALSE if this CProperty type doesn't support the union (structs and maps) or the address is invalid
	 */
	virtual bool SetPropertyValue( byte* InObjectAddress, const UPropertyValue & InPropertyValue ) override;

	/**
	 * @brief Get property's one element size
	 * @return Return property's one element size
	 */
	virtual uint32 GetElementSize() const override;

	/**
	 * @brief Get minimal alignment for property
	 * @return Return minimal alignment for property
	 */
	virtual uint32 GetMinAlignment() const override;
};

#endif // !PROPERTY_H