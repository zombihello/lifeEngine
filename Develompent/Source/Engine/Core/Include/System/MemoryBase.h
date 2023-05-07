/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MEMORYBASE_H
#define MEMORYBASE_H

#include "../CoreDefines.h"

#ifndef DEFINED_Sys_Memzero
	/**
	 * @ingroup Core
	 * @brief Fill memory zero
	 * 
	 * @param[in] InDest Destination for fill zero
	 * @param[in] InCount Size of data
	 */
	#define Sys_Memzero( InDest, InCount )		memset( InDest, 0, InCount )
#endif

/**
 * @ingroup Core
 * @brief Fast memory hashing function that doesn't require a table lookup for each element
 *
 * @param[in] InData Pointer to data for which is considered hash
 * @param[in] InLength Length of data
 * @param[in] InHash Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 Sys_MemFastHash( const void* InData, uint64 InLength, uint64 InHash = 0 )
{
	byte*		data = ( byte* )InData;
	for ( uint64 index = 0; index < InLength; ++index )
	{
		InHash = data[ index ] + ( InHash << 6 ) + ( InHash << 16 ) - InHash;
	}

	return InHash;
}

/**
 * @ingroup Core
 * @brief Fast memory hashing function that doesn't require a table lookup for each element
 *
 * @param[in] InValue The value for which is considered hash 
 * @param[in] InHash Start hash
 * @return Return calculated hash
 */
template< typename TType >
FORCEINLINE uint64 Sys_MemFastHash( const TType& InValue, uint64 InHash = 0 )
{
	return Sys_MemFastHash( &InValue, sizeof( InValue ), InHash);
}

#endif // !MEMORYBASE_H