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

#ifndef WIN_PLATFORM_PRIVATE_H
#define WIN_PLATFORM_PRIVATE_H

// Windows support _aligned_malloc
#undef PLATFORM_USE__ALIGNED_MALLOC
#define PLATFORM_USE__ALIGNED_MALLOC                1

// On Windows ANSI malloc is thread safe
#undef PLATFORM_IS_ANSI_MALLOC_THREADSAFE
#define PLATFORM_IS_ANSI_MALLOC_THREADSAFE          1

// If we on 64 bit platform then it is supports mimalloc
#undef PLATFORM_SUPPORTS_MIMALLOC
#define PLATFORM_SUPPORTS_MIMALLOC                  PLATFORM_64BIT

#endif // !WIN_PLATFORM_PRIVATE_H