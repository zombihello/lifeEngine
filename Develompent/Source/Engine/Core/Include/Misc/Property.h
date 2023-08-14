/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PROPERTY_H
#define PROPERTY_H

#include "Misc/Object.h"
#include "Math/Color.h"
#include "Math/Rotator.h"
#include "Misc/RefCountPtr.h"
#include "System/Name.h"
#include "System/Package.h"

/**
 * @ingroup Core
 * @brief Macro property declaration
 * 
 * @param Name	Property name in a class
 */
#define CPP_PROPERTY( Name )	STRUCT_OFFSET( ThisClass, Name ), sizeof( Name )

/**
 * @ingroup Core
 * @brief Enumeration property flags
 */
enum EPropertyFlags
{
	CPF_None		= 0,		/**< None */
	CPF_EditorOnly	= 1 << 0	/**< Property only for the editor */
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
		, componentValue( nullptr )
		, vectorValue( 0.f, 0.f, 0.f )
		, assetValue( nullptr, nullptr )
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
	class CActorComponent*		componentValue;		/**< Component */
	Vector						vectorValue;		/**< Vector */
	CRotator					rotatorValue;		/**< Rotator */
	TAssetHandle<CAsset>		assetValue;			/**< Asset */
};

/**
 * @ingroup Core
 * @brief The base class all of CObject's properties
 */
class CProperty : public CObject
{
	DECLARE_CLASS( CProperty, CObject, CLASS_Abstract, CASTCLASS_CProperty )

public:
	/**
	 * @brief Constructor
	 */
	CProperty();

	/**
	 * @brief Constructor
	 * 
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags );

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
	 * @brief Get property size
	 * @return Return property size
	 */
	FORCEINLINE uint32 GetSize() const
	{
		return size;
	}

	/**
	 * @brief Get property flags
	 * @return Return property flags
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
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
	uint32			size;			/**< Size */
	uint32			flags;			/**< Flags */
};

/**
 * @ingroup Core
 * @brief Byte property
 */
class CByteProperty : public CProperty
{
	DECLARE_CLASS( CByteProperty, CProperty, 0, CASTCLASS_CByteProperty )

public:
	/**
	 * @brief Constructor
	 */
	CByteProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CByteProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Int property
 */
class CIntProperty : public CProperty
{
	DECLARE_CLASS( CIntProperty, CProperty, 0, CASTCLASS_CIntProperty )

public:
	/**
	 * @brief Constructor
	 */
	CIntProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CIntProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Float property
 */
class CFloatProperty : public CProperty
{
	DECLARE_CLASS( CFloatProperty, CProperty, 0, CASTCLASS_CFloatProperty )

public:
	/**
	 * @brief Constructor
	 */
	CFloatProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CFloatProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Bool property
 */
class CBoolProperty : public CProperty
{
	DECLARE_CLASS( CBoolProperty, CProperty, 0, CASTCLASS_CBoolProperty )

public:
	/**
	 * @brief Constructor
	 */
	CBoolProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CBoolProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Color property
 */
class CColorProperty : public CProperty
{
	DECLARE_CLASS( CColorProperty, CProperty, 0, CASTCLASS_CColorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CColorProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CColorProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Component property
 */
class CComponentProperty : public CProperty
{
	DECLARE_CLASS( CComponentProperty, CProperty, 0, CASTCLASS_CComponentProperty )

public:
	/**
	 * @brief Constructor
	 */
	CComponentProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CComponentProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Vector property
 */
class CVectorProperty : public CProperty
{
	DECLARE_CLASS( CVectorProperty, CProperty, 0, CASTCLASS_CVectorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CVectorProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass					Class where property is stored
	 * @param InName					Property name
	 * @param InCategory				Category
	 * @param InDescription				Description
	 * @param InOffset					Offset to property
	 * @param InSize					Property size
	 * @param InFlags					Flags (see EPropertyFlags)
	 * @param InDefaultComponentValue	Default component value
	 */
	CVectorProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags, float InDefaultComponentValue = 0.f )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
	DECLARE_CLASS( CRotatorProperty, CProperty, 0, CASTCLASS_CRotatorProperty )

public:
	/**
	 * @brief Constructor
	 */
	CRotatorProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 */
	CRotatorProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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
};

/**
 * @ingroup Core
 * @brief Asset property
 */
class CAssetProperty : public CProperty
{
	DECLARE_CLASS( CAssetProperty, CProperty, 0, CASTCLASS_CAssetProperty )

public:
	/**
	 * @brief Constructor
	 */
	CAssetProperty() {}

	/**
	 * @brief Constructor
	 *
	 * @param InClass			Class where property is stored
	 * @param InName			Property name
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InSize			Property size
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InAssetType		Asset type
	 */
	CAssetProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags, EAssetType InAssetType )
		: CProperty( InClass, InName, InCategory, InDescription, InOffset, InSize, InFlags )
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


#endif // !PROPERTY_H