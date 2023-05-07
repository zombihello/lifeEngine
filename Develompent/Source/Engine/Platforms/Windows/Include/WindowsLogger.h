/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSLOGGER_H
#define WINDOWSLOGGER_H

#include <chrono>

#include "Logger/BaseLogger.h"
#include "WindowsArchive.h"

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
