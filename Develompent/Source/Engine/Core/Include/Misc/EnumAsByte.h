/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENUMASBYTE_H
#define ENUMASBYTE_H

#include "Core.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief Template to store enumeration values as bytes in a type-safe way
 */
template<typename TEnum>
class TEnumAsByte
{
public:
	/**
	 * @brief Typedef of enum type
	 */
	typedef TEnum		EnumType;

	/**
	 * @brief Constructor
	 */
	TEnumAsByte() 
	{}

	/**
	 * @brief Constructor of copy
	 * @param InOther	Other value
	 */
	TEnumAsByte( const TEnumAsByte& InOther )
		: value( InOther.value )
	{}

	/**
	 * @brief Override operator of copy
	 * 
	 * @param InOther	Other value
	 * @return Return reference to self
	 */
	FORCEINLINE TEnumAsByte& operator=( const TEnumAsByte& InOther )
	{
		value = InOther.value;
		return *this;
	}

	/**
	 * @brief Constructor
	 * @param InValue	Value
	 */
	FORCEINLINE TEnumAsByte( TEnum InValue )
		: value( ( byte )InValue )
	{ }

	/**
	 * @brief Constructor
	 * @param InValue	Value
	 */
	FORCEINLINE explicit TEnumAsByte( uint32 InValue )
		: value( ( byte )InValue )
	{ }

	/**
	 * @brief Constructor
	 * @param InValue	Value
	 */
	FORCEINLINE explicit TEnumAsByte( byte InValue )
		: value( InValue )
	{ }

	/**
	 * @brief Compares two enumeration values for equality
	 *
	 * @param InValue	The value to compare with
	 * @return Return TRUE if the two values are equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( TEnum InValue ) const
	{
		return ( TEnum )value == InValue;
	}

	/**
	 * @brief Compares two enumeration values for equality
	 *
	 * @param InValue	The value to compare with
	 * @return Return TRUE if the two values are equal, otherwise FALSE
	 */
	FORCEINLINE bool operator==( TEnumAsByte InValue ) const
	{
		return value == InValue.value;
	}

	/**
	 * @brief Implicit conversion to TEnum
	 */
	FORCEINLINE operator TEnum() const
	{
		return ( TEnum )value;
	}

	/**
	 * @brief Get the enumeration value
	 * @return Return the enumeration value
	 */
	FORCEINLINE TEnum GetValue() const
	{
		return ( TEnum )value;
	}

	/**
	 * @brief Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, TEnumAsByte& InValue )
	{
		if ( InArchive.Ver() < VER_EnumAsByte )
		{
			TEnum	enumValue;
			InArchive.Serialize( &enumValue, sizeof( TEnum ) );
			InValue = enumValue;
		}
		else
		{
			InArchive.Serialize( &InValue, sizeof( TEnumAsByte ) );
		}
		return InArchive;
	}

	/**
	 * @brief Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, const TEnumAsByte& InValue )
	{
		Assert( InArchive.IsSaving() );
		InArchive.Serialize( &InValue, sizeof( TEnumAsByte ) );
		return InArchive;
	}

private:
	byte	value;	/**< Holds the value as a byte */
};

#endif // !ENUMASBYTE_H