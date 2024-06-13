/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENUM_H
#define ENUM_H

#include <vector>

#include "System/Name.h"
#include "Reflection/ObjectHash.h"
#include "Reflection/Field.h"

/**
 * @ingroup Core
 * @brief Enum description for reflection
 */
class CEnum : public CField
{
	DECLARE_CLASS_INTRINSIC( CEnum, CField, 0, CASTCLASS_CEnum, TEXT( "Core" ) )

public:
	/**
	 * @brief Constructor
	 */
	CEnum()
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InEnumName	Enum name
	 * @param InPackageName Package name
	 * @param InEnums		Array of enums
	 */
	CEnum( ENativeConstructor, const CName& InEnumName, const tchar* InPackageName, const std::vector<CName>& InEnums )
		: CField( NativeConstructor, InEnumName, InPackageName )
		, enums( InEnums )
	{}

	/**
	 * @brief Sets the array of enums
	 * @param InEnums		Array of enums
	 */
	FORCEINLINE void SetEnums( const std::vector<CName>& InEnums )
	{
		enums = InEnums;
	}

	/**
	 * @brief Get enum by index
	 * 
	 * @param InIndex	Enum index
	 * @return Return the enum name at the specified index
	 */
	FORCEINLINE CName GetEnum( uint32 InIndex ) const
	{
		if ( InIndex >= 0 && InIndex < enums.size() )
		{
			return enums[InIndex];
		}
		return NAME_None;
	}

	/**
	 * @brief Get array of enums
	 * @return Return array of enums
	 */
	FORCEINLINE const std::vector<CName>& GetEnums() const
	{
		return enums;
	}

	/**
	 * @brief Get enum index by name
	 * 
	 * @param InName	Name
	 * @return Return the index of the specified name, if it exists in the enum names list otherwise will return -1
	 */
	FORCEINLINE uint32 FindEnumIndex( const CName& InName ) const
	{
		for ( uint32 index = 0, count = enums.size(); index < count; ++index )
		{
			if ( InName == enums[index] )
			{
				return index;
			}
		}
		return -1;
	}

	/**
	 * @brief Get number enums
	 * @return Return the number of enum names
	 */
	FORCEINLINE uint32 NumEnums() const
	{
		return enums.size();
	}

private:
	std::vector<CName>		enums;	/**< Array of enums */
};

#endif // !ENUM_H