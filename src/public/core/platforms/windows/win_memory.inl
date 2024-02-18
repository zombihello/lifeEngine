/**
 * @file
 * @addtogroup core core
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

#ifndef WIN_MEMORY_INL
#define WIN_MEMORY_INL

/*
 ==================
 L_memmove
 ==================
 */
FORCEINLINE void* L_memmove( void* pDest, const void* pSrc, size_t numBytes )
{
	return memmove( pDest, pSrc, numBytes );
}

/*
 ==================
 L_memcmp
 ==================
 */
FORCEINLINE int32 L_memcmp( const void* pBuf1, const void* pBuf2, size_t numBytes )
{
	return memcmp( pBuf1, pBuf2, numBytes );
}

/*
 ==================
 L_memset
 ==================
 */
FORCEINLINE void* L_memset( void* pDest, uint8 c, size_t numBytes )
{
	return memset( pDest, c, numBytes );
}

/*
 ==================
 L_memzero
 ==================
 */
FORCEINLINE void* L_memzero( void* pDest, size_t numBytes )
{
	return memset( pDest, 0, numBytes );
}

/*
 ==================
 L_memcpy
 ==================
 */
FORCEINLINE void* L_memcpy( void* pDest, const void* pSrc, size_t numBytes )
{
	return memcpy( pDest, pSrc, numBytes );
}

#endif // !WIN_MEMORY_INL