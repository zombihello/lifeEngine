/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/11/2021
 */

#ifndef WINDOWSLOGGER_H
#define WINDOWSLOGGER_H

#include "Logger/BaseLogger.h"
#include "WindowsArchive.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for logging on Windows
 */
class WindowsLogger : public BaseLogger
{
public:
    /**
     * @ingroup WindowsPlatform
     * @brief Constructor
     */
                            WindowsLogger();

    /**
     * @ingroup WindowsPlatform
     * @brief Destructor
     */
                            ~WindowsLogger();

    /**
     * @brief Initialize logger
     */
    virtual void            Init() override;

    /**
     * @ingroup WindowsPlatform
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void            Serialize( const tchar* InMessage, ELogType InLogType, ELogCategory InLogCategory );

    /**
     * @brief Closes output device and cleans up
     *
     * Closes output device and cleans up. This can't happen in the destructor
     * as we might have to call "delete" which cannot be done for static/ global
     * objects
     */
    virtual void            TearDown() override;

    /**
     * @ingroup WindowsPlatform
     * @brief Shows or hides the console window
     * 
     * @param[in] InShowWindow Whether to show or hide the console window
     */
    void                    Show( bool InShowWindow );
    
    /**
     * @ingroup WindowsPlatform
     * @brief Is showed console
     * @return Return true if console is shown or false if not
     */
    FORCEINLINE bool        IsShow() const                 { return handle; }

private:
    void*               handle;
    BaseArchive*        archiveLogs;
};

#endif // !WINDOWSLOGGER_H
