/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FASTHASH_H
#define FASTHASH_H

#include "Core.h"
#include "Misc/StringTools.h"

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InData	Pointer to data for which is considered hash
 * @param InLength	Length of data
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 FastHash( const void* InData, uint64 InLength, uint64 InHash = 0 )
{
	byte*		data = ( byte* )InData;
	for ( uint64 index = 0; index < InLength; ++index )
	{
		InHash = data[index] + ( InHash << 6 ) + ( InHash << 16 ) - InHash;
	}

	return InHash;
}

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InValue	The value for which is considered hash 
 * @param InHash	Start hash
 * @return Return calculated hash
 */
template<typename TType>
FORCEINLINE uint64 FastHash( const TType& InValue, uint64 InHash = 0 )
{
	return FastHash( &InValue, sizeof( InValue ), InHash );
}

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InValue	The value for which is considered hash
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 FastHash( const achar* InValue, uint64 InHash = 0 )
{
	return FastHash( InValue, L_Strlen( InValue ) * sizeof( achar ), InHash );
}

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InValue	The value for which is considered hash
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 FastHash( const std::string& InValue, uint64 InHash = 0 )
{
	return FastHash( InValue.data(), InValue.size() * sizeof( std::string::value_type ), InHash );
}

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InValue	The value for which is considered hash
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 FastHash( const tchar* InValue, uint64 InHash = 0 )
{
	return FastHash( InValue, L_Strlen( InValue ) * sizeof( tchar ), InHash );
}

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InValue	The value for which is considered hash
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 FastHash( const std::wstring& InValue, uint64 InHash = 0 )
{
	return FastHash( InValue.data(), InValue.size() * sizeof( std::wstring::value_type ), InHash );
}

#endif // !FASTHASH_H