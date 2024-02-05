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

#ifndef LOGGERMACROS_H
#define LOGGERMACROS_H

#include "Core/LEBuild.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/Logger/BaseLogger.h"

// If configuration is not shipping - we using logs for debug
#if !NO_LOGGING || PLATFORM_DOXYGEN
	/**
	 * @ingroup Core
	 * @brief Macro for print message to log
	 * @warning In shipping this macro is empty and logging disabled
	 * 
	 * @param[in] InMessage Message
	 * @param[in] ... Other arguments of message
	 */
	#define Logf( InMessage, ... )				g_Log->Printf( LT_Log, InMessage, __VA_ARGS__ )
	 
	 /**
	  * @ingroup Core
	  * @brief Macro for print warning to log
	  * @warning In shipping this macro is empty and logging disabled
	  *
	  * @param[in] InMessage Message
	  * @param[in] ... Other arguments of message
	  */
	#define Warnf( InMessage, ... )				g_Log->Printf( LT_Warning, InMessage, __VA_ARGS__ )

	/**
	 * @ingroup Core
	 * @brief Macro for print error to log
	 * @warning In shipping this macro is empty and logging disabled
	 *
	 * @param[in] InMessage Message
	 * @param[in] ... Other arguments of message
	 */
	#define Errorf( InMessage, ... )			g_Log->Printf( LT_Error, InMessage, __VA_ARGS__ )
#else
	#define Logf( InMessage, ... )
	#define Warnf( InMessage, ... )
	#define Errorf( InMessage, ... )
#endif // !NO_LOGGING || PLATFORM_DOXYGEN

#endif // !LOGGERMACROS_H
