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

#ifndef BUILD_H
#define BUILD_H

// Define macroses PLATFORM_XXXBIT if they not defined
#ifndef PLATFORM_32BIT
	#define PLATFORM_32BIT 0
#endif // !PLATFORM_32BIT

#ifndef PLATFORM_64BIT
	#define PLATFORM_64BIT 0
#endif // !PLATFORM_64BIT

// If we don't build the engine as monolithic then have to define BUILD_MONOLITHIC as 0
#ifndef BUILD_MONOLITHIC
	#define BUILD_MONOLITHIC 0
#endif // !BUILD_MONOLITHIC

// Enable or disable asserts in a build
#ifndef ENABLE_ASSERT
	#define ENABLE_ASSERT !RETAIL
#endif // !ENABLE_ASSERT

// Enable or disable logging in a build
#ifndef ENABLE_LOGGING
	#define ENABLE_LOGGING !RETAIL
#endif // !ENABLE_LOGGING

#endif // !BUILD_H