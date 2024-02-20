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

#ifndef COREAPI_H
#define COREAPI_H

#include "core/build.h"
#include "core/platform.h"

#if !defined( BUILD_STATIC_LIB ) && !defined( BUILD_MONOLITHIC_ENGINE )
	#ifdef CORE_DLL_EXPORT
		#define CORE_INTERFACE		extern "C" DLL_EXPORT
		#define CORE_CLASS			DLL_EXPORT
		#define CORE_GLOBAL			extern DLL_EXPORT
	#else
		#define CORE_INTERFACE		extern "C" DLL_IMPORT
		#define CORE_CLASS			DLL_IMPORT
		#define CORE_GLOBAL			extern DLL_IMPORT
	#endif // CORE_DLL_EXPORT
#else
	#define CORE_INTERFACE			extern "C"
	#define CORE_CLASS
	#define CORE_GLOBAL				extern
#endif // !BUILD_STATIC_LIB || BUILD_MONOLITHIC_ENGINE

#endif // !COREAPI_H