/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
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

#ifndef WINDOWSLOGGER_H
#define WINDOWSLOGGER_H

#include <chrono>

#include "Core/Logger/BaseLogger.h"
#include "Core/Platforms/Windows/WindowsArchive.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for logging on Windows
 */
class CWindowsLogger : public CBaseLogger
{
public:
    /**
     * @ingroup WindowsPlatform
     * @brief Constructor
     */
    CWindowsLogger();

    /**
     * @ingroup WindowsPlatform
     * @brief Destructor
     */
    ~CWindowsLogger();

    /**
     * @brief Initialize logger
     */
    virtual void Init() override;

    /**
     * @ingroup WindowsPlatform
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void Serialize( const tchar* InMessage, ELogType InLogType );

    /**
     * @brief Closes output device and cleans up
     *
     * Closes output device and cleans up. This can't happen in the destructor
     * as we might have to call "delete" which cannot be done for static/ global
     * objects
     */
    virtual void TearDown() override;

    /**
     * @ingroup WindowsPlatform
     * @brief Shows or hides the console window
     * 
     * @param[in] InShowWindow Whether to show or hide the console window
     */
    void Show( bool InShowWindow );
    
    /**
     * @ingroup WindowsPlatform
     * @brief Is showed console
     * @return Return true if console is shown or false if not
     */
    FORCEINLINE bool IsShow() const { return consoleHandle; }

    /**
     * @brief Set color for text in log
     *
     * @param InLogColor Log color
     */
    virtual void SetTextColor( ELogColor InLogColor ) override;

    /**
     * @brief Reset color text to default
     */
    virtual void ResetTextColor() override;

private:
    HANDLE              consoleHandle;      /**< OS handle on console*/
    CArchive*           archiveLogs;        /**< Archive of logs */
    ELogColor           textColor;          /**< Current text color */
};

#endif // !WINDOWSLOGGER_H
