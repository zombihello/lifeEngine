/**
 * @file
 * @addtogroup stdlib stdlib
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

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/strtools.h"

/**
 * @ingroup stdlib
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param pData		Pointer to data for which is considered hash
 * @param size		Data size
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHash( const void* pData, uint64 size, hash_t hash = 0 )
{
	byte*	pLocalData = ( byte* )pData;
	for ( uint64 index = 0; index < size; ++index )
	{
		hash = pLocalData[index] + ( hash << 6 ) + ( hash << 16 ) - hash;
	}

	return hash;
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function that doesn't require a table lookup for each element
 *
 * @param value		The value for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
template<typename TType>
FORCEINLINE hash_t FastHash( const TType& value, hash_t hash = 0 )
{
	return FastHash( &value, sizeof( value ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param pString	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const wchar* pString, hash_t hash )
{
	return FastHash( pString, L_wcslen( pString ) * sizeof( wchar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param string	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const std::wstring& string, hash_t hash )
{
	return FastHash( string.data(), ( uint64 )string.size() * sizeof( wchar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param pString	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const achar* pString, hash_t hash )
{
	return FastHash( pString, L_strlen( pString ) * sizeof( achar ), hash );
}

/**
 * @ingroup stdlib
 * @brief Fast hashing function from string that doesn't require a table lookup for each element
 *
 * @param string	The string for which is considered hash
 * @param hash		Start hash
 * @return Return calculated hash
 */
FORCEINLINE hash_t FastHashFromString( const std::string& string, hash_t hash )
{
	return FastHash( string.data(), ( uint64 )string.size() * sizeof( achar ), hash );
}

#endif // !FASTHASH_H