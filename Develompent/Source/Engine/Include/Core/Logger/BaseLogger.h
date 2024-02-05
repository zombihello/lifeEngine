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

#ifndef BASELOGGER_H
#define BASELOGGER_H

#include "Core/Core.h"

/**
 * @ingroup Core
 * @brief Enumerating of log types
 */
enum ELogType
{
	LT_Log,                 /**< Normal message display */
	LT_Warning,             /**< Displaying a warning */
	LT_Error                /**< Error message */
};

/**
 * @ingroup Core
 * @brief Enumerating of log color
 */
enum ELogColor
{
    LC_Default,         /**< Default color */
    LC_Red,             /**< Red */
    LC_Yellow,          /**< Yellow */
    LC_Green            /**< Green */
};

/**
 * @ingroup Core
 * @brief Base class of logging
 */
class CBaseLogger
{
public:
    /**
     * @brief Constructor
     */
    CBaseLogger() {}

    /**
     * @brief Destructor
     */
    virtual ~CBaseLogger() {}

    /**
     * @brief Initialize logger
     */
    virtual void Init() {}

    /**
     * @ingroup Core
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InLogType Type of message
     */
    virtual void Serialize( const tchar* InMessage, ELogType InLogType ) {};

    /**
     * @ingroup Core
     * @brief Flush of output device
     */
    virtual void Flush() {};

    /**
     * @ingroup Core
     * @brief Closes output device and cleans up
     *
     * Closes output device and cleans up. This can't happen in the destructor
	 * as we might have to call "delete" which cannot be done for static/ global
	 * objects
     */
    virtual void TearDown() {};

    /**
     * @ingroup Core
     * @brief Print message to output device
     *
     * @param[in] InLogType Type of message
     * @param[in] InMessage Message
     * @param[in] ... Other arguments of message
     */
    void Printf( ELogType InLogType, const tchar* InMessage, ... );

    /**
     * @brief Set color for text in log
     * 
     * @param InLogColor Log color
     */
    virtual void SetTextColor( ELogColor InLogColor ) {}

    /**
     * @brief Reset color text to default
     */
    virtual void ResetTextColor() {}
};

#endif // !BASELOGGER_H
