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
#include "Reflection/Field.h"

/**
 * @ingroup Core
 * @brief Macro for declare enum
 *
 * @param TEnum            Enum
 *
 * Example usage: @code DECLARE_ENUM( ESpriteType ) @endcode
 */
#define DECLARE_ENUM( TEnum ) \
	namespace Enum \
	{ \
		CEnum* Get##TEnum(); \
	}

/**
 * @ingroup Core
 * @brief Macro for implement enum
 *
 * @param TEnum			Enum
 * @param TForEachEnum	Macro of for each enum
 *
 * Example usage: @code IMPLEMENT_ENUM( ESpriteType, FOREACH_ENUM_SPRITETYPE ) @endcode
 */
#define IMPLEMENT_ENUM( TEnum, TForEachEnum ) \
	namespace Enum \
	{ \
		CEnum* Get##TEnum() \
		{ \
			static CEnum*	s_CEnum = nullptr; \
			if ( !s_CEnum ) \
			{ \
				std::vector<CName>		enums; \
				TForEachEnum( INTERNAL_REGISTER_ENUM ) \
				s_CEnum = ::new CEnum( TEXT( #TEnum ), enums ); \
				s_CEnum->SetClass( CEnum::StaticClass() ); \
				s_CEnum->AddObjectFlag( OBJECT_RootSet | OBJECT_DisregardForGC ); \
				CObjectGC::Get().AddObject( s_CEnum ); \
			} \
			return s_CEnum; \
		} \
	} \
    struct Register##TEnum \
    { \
        Register##TEnum() \
        { \
            CReflectionEnvironment::Get().AddEnum( Enum::Get##TEnum() ); \
        } \
    }; \
	static Register##TEnum s_Register##TEnum;

/**
 * @ingroup Core
 * @brief Internal register enum
 * 
 * @param InName	Name
 */
#define INTERNAL_REGISTER_ENUM( InName ) \
	enums.push_back( CName( TEXT( #InName ) ) );

/**
 * @ingroup Core
 * @brief Enum description for reflection
 */
class CEnum : public CField
{
	DECLARE_CLASS_INTRINSIC( CEnum, CField, 0, 0 )

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
	 * @param InEnums		Array of enums
	 */
	CEnum( const CName& InEnumName, const std::vector<CName>& InEnums )
		: CField( InEnumName )
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