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

#ifndef MEMORY_H
#define MEMORY_H

#include "Core/CoreDefines.h"

enum
{
	DEFAULT_ALIGNMENT	= 0,		/**< Default allocator alignment. If the default is specified, the allocator applies to engine rules. Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does not support allocation alignment, the alignment will be ignored */
	MIN_ALIGNMENT		= 8			/**< Minimum allocator alignment */
};

// Platform specific memory implementation
#if PLATFORM_WINDOWS
	#include "Core/Platforms/Windows/WindowsMemory.h"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

//
// C++ style memory allocation
//
FORCEINLINE void* operator new( size_t InSize )
{
	return Memory::Malloc( InSize );
}

FORCEINLINE void operator delete( void* InPtr ) noexcept
{
	return Memory::Free( InPtr );
}

FORCEINLINE void* operator new[]( size_t InSize )
{
	return Memory::Malloc( InSize );
}

FORCEINLINE void operator delete[]( void* InPtr ) noexcept
{
	return Memory::Free( InPtr );
}

#endif // !MEMORY_H