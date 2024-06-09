/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRUCT_H
#define STRUCT_H

#include <vector>

#include "Reflection/ObjectHash.h"
#include "Reflection/Field.h"

#if WITH_EDITOR
#include "Misc/JsonDocument.h"
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * @brief Structure description for reflection
 */
class CStruct : public CField
{
	DECLARE_CLASS( CStruct, CField, 0, 0, TEXT( "Core" ) )

public:
	/**
	 * @brief Constructor
	 * @param InSuperStruct		Pointer to super struct
	 */
	CStruct( CStruct* InSuperStruct = nullptr )
		: propertiesSize( 0 )
		, minAlignment( 1 )
		, superStruct( InSuperStruct )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InStructName			Struct name
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the struct
	 * @param InSuperStruct			Pointer to super struct
	 * @param InPackageName			Package name
	 * @param InFlags				The object flags
	 */
	CStruct( ENativeConstructor, const CName& InStructName, uint32 InPropertiesSize, uint32 InMinAlignment, const tchar* InPackageName, CStruct* InSuperStruct = nullptr )
		: CField( NativeConstructor, InStructName, InPackageName )
		, propertiesSize( InPropertiesSize )
		, minAlignment( InMinAlignment )
		, superStruct( InSuperStruct )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InStructName			Struct name
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the struct
	 * @param InSuperStruct			Pointer to super struct
	 * @param InFlags				The object flags
	 */
	CStruct( uint32 InPropertiesSize, uint32 InMinAlignment, CStruct* InSuperStruct = nullptr )
		: propertiesSize( InPropertiesSize )
		, minAlignment( InMinAlignment )
		, superStruct( InSuperStruct )
	{}

	/**
	 * @brief Set super struct
	 * @param InSuperStruct		Super struct
	 */
	FORCEINLINE void SetSuperStruct( CStruct* InSuperStruct )
	{
		superStruct = InSuperStruct;
	}

	/**
	 * @brief Get super struct
	 * @return Return pointer to super struct. If it is not it will return NULL
	 */
	FORCEINLINE CStruct* GetSuperStruct() const
	{
		return superStruct;
	}

	/**
	 * @brief Set properties size in bytes
	 * @param InPropertiesSize	Properties size in bytes
	 */
	FORCEINLINE void SetPropertiesSize( uint32 InPropertiesSize )
	{
		propertiesSize = InPropertiesSize;
	}

	/**
	 * @brief Get properties size in bytes
	 * @return Return properties size in bytes
	 */
	FORCEINLINE uint32 GetPropertiesSize() const
	{
		return propertiesSize;
	}

	/**
	 * @brief Set minimum alignment for the struct
	 * @param InMinAlignment	Minimum alignment for the struct
	 */
	FORCEINLINE void SetMinAlignment( uint32 InMinAlignment )
	{
		minAlignment = InMinAlignment;
	}

	/**
	 * @brief Get minimum alignment for the struct
	 * @return Return minimum alignment for the struct
	 */
	FORCEINLINE uint32 GetMinAlignment() const
	{
		return minAlignment;
	}

	/**
	 * @brief Is a struct
	 *
	 * @param TClass	Checking struct
	 * @return Return TRUE if this struct either is InStruct, or is a child of InStruct. This will not crash on null structs
	 */
	template<class TClass>
	FORCEINLINE bool IsChildOf() const
	{
		return IsChildOf( TClass::StaticClass() );
	}

	/**
	 * @brief Is a struct
	 *
	 * @param InStruct	Checking struct
	 * @return Return TRUE if this struct either is InStruct, or is a child of InStruct. This will not crash on null structs
	 */
	FORCEINLINE bool IsChildOf( CStruct* InStruct ) const
	{
		// If struct is not valid we return false
		if ( !InStruct )
		{
			return false;
		}

		// Iterate over each struct ancestor
		for ( const CStruct* tempStruct = this; tempStruct; tempStruct = tempStruct->superStruct )
		{
			// Success! This struct contains the struct being checked
			if ( tempStruct == InStruct )
			{
				return true;
			}
		}

		// Otherwise the struct does not contain the struct being checked 
		return false;
	}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Add struct property
	 * @param InProperty	Property
	 */
	virtual void AddProperty( class CProperty* InProperty ) override;

	/**
	 * @brief Serialize struct properties
	 * @param InArchive     Archive for serialize
	 * @param InData		Pointer to object
	 */
	void SerializeProperties( class CArchive& InArchive, byte* InData );

#if WITH_EDITOR
	/**
	 * @brief Export struct properties to JSON
	 * 
	 * @param OutValueString		Output string with the property value
	 * @param InData				Pointer to object
	 * @param InExportRootScope		Export root scope
	 * @param InPortFlags			Export flags (see EPropertyPortFlags)
	 */
	void ExportProperties( std::wstring& OutValueString, byte* InData, CObject* InExportRootScope, uint32 InPortFlags = PPF_None );

	/**
	 * @brief Import struct properties from JSON
	 *
	 * @param InJsonValue			Json struct
	 * @param InData				Pointer to object
	 * @param InImportRootScope		Import root scope
	 * @param InPortFlags			Import flags (see EPropertyPortFlags)
	 */
	void ImportProperty( const CJsonValue* InJsonValue, byte* InData, CObject* InImportRootScope, uint32 InPortFlags = PPF_None );
#endif // WITH_EDITOR

	/**
	 * @brief Get array of struct properties
	 *
	 * @param OutArrayProperties		Output array properties
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 */
	void GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents = true ) const;

	/**
	 * @brief Get number properties in the struct
	 *
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 * @return Return number properties in the struct
	 */
	uint32 GetNumProperties( bool InPropertiesInParents = true ) const;

	/**
	 * @brief Find property
	 *
	 * @param InName				Property name
	 * @param InFindInParents		Take into account properties of the parents
	 * @return Return pointer to struct property. If not found return NULL
	 */
	template<typename TPropertyClass>
	FORCEINLINE TPropertyClass* FindProperty( const CName& InName, bool InFindInParents = true ) const
	{
		return Cast<TPropertyClass>( InternalFindProperty( InName, InFindInParents ) );
	}

private:
	/**
	 * @brief Find property
	 *
	 * @param InName				Property name
	 * @param InFindInParents		Take into account properties of the parents
	 * @return Return pointer to struct property, if not found return NULL
	 */
	class CProperty* InternalFindProperty( const CName& InName, bool InFindInParents = true ) const;

	/**
	 * @brief Serialize struct binary properties
	 * @param InArchive     Archive for serialize
	 * @param InData		Pointer to object
	 */
	void SerializeBinaryProperties( class CArchive& InArchive, byte* InData );

	/**
	 * @brief Serialize struct tagged properties
	 * @param InArchive     Archive for serialize
	 * @param InData		Pointer to object
	 */
	void SerializeTaggedProperties( class CArchive& InArchive, byte* InData );

	uint32								propertiesSize;	/**< Properties size in bytes */
	uint32								minAlignment;	/**< Minimum alignment for the struct */
	CStruct*							superStruct;	/**< Pointer to super struct */
	std::vector<class CProperty*>		properties;		/**< Struct properties */
};

#endif // !STRUCT_H