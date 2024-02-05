/**
 * @file
 * @addtogroup Core Core
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ENUM_H
#define ENUM_H

#include <vector>
#include "Core/System/Name.h"

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
				s_CEnum = new CEnum( enums ); \
			} \
			return s_CEnum; \
		} \
	}

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
class CEnum
{
public:
	/**
	 * @brief Constructor
	 */
	CEnum()
	{}

	/**
	 * @brief Constructor
	 * @param InEnums		Array of enums
	 */
	CEnum( const std::vector<CName>& InEnums )
		: enums( InEnums )
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