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

#if WITH_EDITOR
#include "Misc/JsonDocument.h"
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CProperty, CField, CLASS_Abstract, CASTCLASS_CProperty, TEXT( "Core" ) )
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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 );

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 * 
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of a object where the value of this property is stored
	 */
	FORCEINLINE void SerializeProperty( class CArchive& InArchive, byte* InObjectAddress )
	{
		if ( ShouldSerializeValue( InArchive ) )
		{
			byte*		data = InObjectAddress + offset;
			if ( InArchive.WantBinaryPropertySerialization() || InArchive.IsObjectReferenceCollector() )
			{
				SerializeBinaryProperties( InArchive, data );
			}
			else
			{
				SerializeTaggedProperties( InArchive, data );
			}
		}
	}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive		Archive for serialize
	 * @param InData		The address of property start
	 * @param InArrayIdx	Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const		PURE_VIRTUAL( CProperty::SerializeValue, );

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 * 
	 * @param OutValueString		Output string with the property value
	 * @param InObjectAddress		Address of a object where the value of this property is stored
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	void ExportProperty( std::wstring& OutValueString, byte* InObjectAddress, CObject* InExportRootScope, uint32 InPortFlags = PPF_None );

	/**
	 * @brief Export property's data to JSON
	 * 
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) PURE_VIRTUAL( CProperty::ExportValue, );

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InObjectAddress		Address of a object where the value of this property is stored
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	void ImportProperty( const CJsonValue* InJsonValue, byte* InObjectAddress, CObject* InImportRootScope, uint32 InPortFlags = PPF_None );

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) PURE_VIRTUAL( CProperty::ImportValue, );
#endif // WITH_EDITOR

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
	FORCEINLINE void SetCategory( const CName& InNewCategory )
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
	 * @brief Add property flag
	 * @param InFlag	New flag
	 */
	FORCEINLINE void AddFlag( uint32 InFlag )
	{
		flags |= InFlag;
	}

	/**
	 * @brief Remove flag
	 * @param InFlag	New flag
	 */
	FORCEINLINE void RemoveFlag( uint32 InFlag )
	{
		flags &= ~InFlag;
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

	/**
	 * @brief Is should serialize this value
	 * 
	 * @param InArchive		Archive
	 * @return Return TRUE if this value should serialize, otherwise returns FALSE
	 */
	bool ShouldSerializeValue( CArchive& InArchive ) const;

#if WITH_EDITOR
	/**
	 * @brief Is should export/import this value
	 * 
	 * @param InPortFlags		Export/import flags (see EPropertyPortFlags)
	 * @return Return TRUE if this value should export/import, otherwise returns FALSE
	 */
	bool ShouldPort( uint32 InPortFlags = PPF_None ) const;
#endif // WITH_EDITOR

protected:
	CName			category;		/**< Category */
	std::wstring	description;	/**< Description */
	uint32			offset;			/**< Offset */
	uint32			flags;			/**< Flags */
	uint32			arraySize;		/**< Count of persistent variables */

private:
	/**
	 * @brief Serialize binary property instead of safer but slower tagged form
	 * @param InArchive     Archive for serialize
	 * @param InData		Pointer to property data
	 */
	FORCEINLINE void SerializeBinaryProperties( class CArchive& InArchive, byte* InData )
	{
		for ( uint32 index = 0; index < arraySize; ++index )
		{
			SerializeValue( InArchive, InData, index );
		}
	}

	/**
	 * @brief Serialize tagged property instead of unsafer but faster binary form
	 * @param InArchive     Archive for serialize
	 * @param InData		Pointer to property data
	 */
	FORCEINLINE void SerializeTaggedProperties( class CArchive& InArchive, byte* InData )
	{
		// In tagged form we serialize array size because it may was changed in newly classes
		uint32		serialArraySize = arraySize;
		InArchive << serialArraySize;
		for ( uint32 index = 0; index < serialArraySize; ++index )
		{
			SerializeValue( InArchive, InData, index );
		}
	}
};

/**
 * @ingroup Core
 * @brief Property tag used in serialization of properties
 */
class CPropertyTag
{
public:
	/**
	 * @brief Functions to extract a property tag as a key for std::unordered_map and std::unordered_set
	 */
	struct KeyFunc
	{
		/**
		 * @brief Calculate hash of a property tag
		 *
		 * @param InPropertyTag		Property tag
		 * @return Return hash of this property tag
		 */
		FORCEINLINE std::size_t operator()( const CPropertyTag& InPropertyTag ) const
		{
			return InPropertyTag.GetHash();
		}

		/**
		 * @brief Compare property tags
		 *
		 * @param InA	First property tag
		 * @param InB	Second property tag
		 * @return Return TRUE if InA and InB equal, otherwise returns FALSE
		 */
		FORCEINLINE bool operator()( const CPropertyTag& InA, const CPropertyTag& InB ) const
		{
			return InA.GetHash() < InB.GetHash();
		}
	};

	/**
	 * @brief Constructor
	 * @param InProperty	Property
	 */
	CPropertyTag( CProperty* InProperty = nullptr )
		: property( InProperty )
		, propertyName( InProperty ? InProperty->GetCName() : NAME_None )
		, className( InProperty ? InProperty->GetClass()->GetCName() : NAME_None )
		, serialSize( 0 )
	{}

	/**
	 * @brief Set property name
	 * @param InPropertyName	Property name
	 */
	FORCEINLINE void SetPropertyName( const CName& InPropertyName )
	{
		propertyName = InPropertyName;
	}

	/**
	 * @brief Set property class name
	 * @param InClassName	Class name
	 */
	FORCEINLINE void SetClassName( const CName& InClassName )
	{
		className = InClassName;
	}

	/**
	 * @brief Set size of serialized data in bytes
	 * @param InSize	Size of serialized data in bytes
	 */
	FORCEINLINE void SetSerialSize( uint32 InSize )
	{
		serialSize = InSize;
	}

	/**
	 * @brief Set associated property with this tag
	 * @param InProperty	Associated property with this tag
	 */
	FORCEINLINE void SetProperty( CProperty* InProperty )
	{
		property = InProperty;
	}

	/**
	 * @brief Get property name
	 * @return Return property name
	 */
	FORCEINLINE const CName& GetPropertyName() const
	{
		return propertyName;
	}

	/**
	 * @brief Get property class name
	 * @return Return property class name
	 */
	FORCEINLINE const CName& GetClassName() const
	{
		return className;
	}

	/**
	 * @brief Calculate and returns hash for this property tag
	 * @return Return calculated hash for this property tag
	 */
	FORCEINLINE uint64 GetHash() const
	{
		uint64	hash = propertyName.GetHash();
		FastHash( className.GetHash(), hash );
		FastHash( serialSize, hash );
		return hash;
	}

	/**
	 * @brief Get associated property with this tag
	 * @return Return associated property with this tag. If isn't returns NULL
	 */
	FORCEINLINE CProperty* GetProperty() const
	{
		return property;
	}

	/**
	 * @brief Get size of serialized data in bytes
	 * @return Return size of serialized data in bytes
	 */
	FORCEINLINE uint32 GetSerialSize() const
	{
		return serialSize;
	}

	/**
	 * @brief Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, CPropertyTag& InValue )
	{
		InArchive << InValue.propertyName;
		InArchive << InValue.className;
		InArchive << InValue.serialSize;
		return InArchive;
	}

	/**
	 * @brief Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, const CPropertyTag& InValue )
	{
		Assert( InArchive.IsSaving() );
		InArchive << InValue.propertyName;
		InArchive << InValue.className;
		InArchive << InValue.serialSize;
		return InArchive;
	}

private:
	CProperty*	property;		/**< Associated property with this tag. Transient */
	CName		propertyName;	/**< Property name */
	CName		className;		/**< Property class name */
	uint32		serialSize;		/**< Size of serialized data in bytes */
};

/**
 * @ingroup Core
 * @brief Byte property
 */
class CByteProperty : public CProperty
{
	DECLARE_CLASS( CByteProperty, CProperty, 0, CASTCLASS_CByteProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InEnum			If InEnum not null then it's mean this property is enum
	 * @param InArraySize		Count of persistent variables
	 */
	CByteProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CEnum* InEnum = nullptr, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, cenum( InEnum )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CIntProperty, CProperty, 0, CASTCLASS_CIntProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CIntProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CFloatProperty, CProperty, 0, CASTCLASS_CFloatProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CFloatProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CBoolProperty, CProperty, 0, CASTCLASS_CBoolProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InOffset			Offset to property
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CBoolProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CColorProperty, CProperty, 0, CASTCLASS_CColorProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CColorProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CObjectProperty, CProperty, 0, CASTCLASS_CObjectProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InPropertyClass	Property class
	 * @param InArraySize		Count of persistent variables
	 */
	CObjectProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CClass* InPropertyClass, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, propertyClass( InPropertyClass )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CVectorProperty, CProperty, 0, CASTCLASS_CVectorProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset					Offset to property
	 * @param InCategory				Category
	 * @param InDescription				Description
	 * @param InFlags					Flags (see EPropertyFlags)
	 * @param InDefaultComponentValue	Default component value
	 * @param InArraySize				Count of persistent variables
	 */
	CVectorProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, float InDefaultComponentValue = 0.f, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, defaultComponentValue( InDefaultComponentValue )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CRotatorProperty, CProperty, 0, CASTCLASS_CRotatorProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CRotatorProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CAssetProperty, CProperty, 0, CASTCLASS_CAssetProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InAssetType		Asset type
	 * @param InArraySize		Count of persistent variables
	 */
	CAssetProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, EAssetType InAssetType, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, assetType( InAssetType )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CArrayProperty, CProperty, 0, CASTCLASS_CArrayProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CArrayProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, innerProperty( nullptr )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CStructProperty, CProperty, 0, CASTCLASS_CStructProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InPropertyStruct	Property struct
	 * @param InArraySize		Count of persistent variables
	 */
	CStructProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, CStruct* InPropertyStruct, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
		, propertyStruct( InPropertyStruct )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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
	DECLARE_CLASS( CStringProperty, CProperty, 0, CASTCLASS_CStringProperty, TEXT( "Core" ) )

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
	 * @brief Constructor
	 *
	 * @param InOffset			Offset to property
	 * @param InCategory		Category
	 * @param InDescription		Description
	 * @param InFlags			Flags (see EPropertyFlags)
	 * @param InArraySize		Count of persistent variables
	 */
	CStringProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize = 1 )
		: CProperty( InOffset, InCategory, InDescription, InFlags, InArraySize )
	{}

	/**
	 * @brief Serialize property's data
	 *
	 * @param InArchive			Archive for serialize
	 * @param InObjectAddress	The address of property start
	 * @param InArrayIdx		Array slot of persistent variables
	 */
	virtual void SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const override;

#if WITH_EDITOR
	/**
	 * @brief Export property's data to JSON
	 *
	 * @param OutValueString		Output string with the property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	virtual void ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags = PPF_None ) override;

	/**
	 * @brief Import property's data from JSON
	 *
	 * @param InJsonValue			Json property value
	 * @param InData				The address of property start
	 * @param InArrayIdx			Array slot of persistent variables
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	virtual void ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags = PPF_None ) override;
#endif // WITH_EDITOR

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