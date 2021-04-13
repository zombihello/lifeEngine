/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/11/2021
 */

#ifndef WINDOWSOUTPUTDEVICES_H
#define WINDOWSOUTPUTDEVICES_H

#include "Misc/OutputDevices.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for windows output device
 */
class WindowsOutputDevice : public OutputDevice
{
public:
    /**
     * @ingroup WindowsPlatform
     * @brief Constructor
     */
    WindowsOutputDevice();

    /**
     * @ingroup WindowsPlatform
     * @brief Destructor
     */
    ~WindowsOutputDevice();

    /**
     * @ingroup WindowsPlatform
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void Serialize( const tchar* InMessage, EEventType InEvent );

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
    FORCEINLINE bool IsShow() const                 { return handle; }

private:
    void*           handle;
};

#endif // !WINDOWSOUTPUTDEVICES_H
