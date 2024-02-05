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

#ifndef FASTHASH_H
#define FASTHASH_H

#include "Core/Core.h"

/**
 * @ingroup Core
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param InData	Pointer to data for which is considered hash
 * @param InLength	Length of data
 * @param InHash	Start hash
 * @return Return calculated hash
 */
FORCEINLINE uint64 Sys_FastHash( const void* InData, uint64 InLength, uint64 InHash = 0 )
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
FORCEINLINE uint64 Sys_FastHash( const TType& InValue, uint64 InHash = 0 )
{
	return Sys_FastHash( &InValue, sizeof( InValue ), InHash);
}

#endif // !FASTHASH_H