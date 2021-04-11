/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/11/2021
 */

#ifndef WINDOWSOUTPUTDEVICES_H
#define WINDOWSOUTPUTDEVICES_H

#include "OutputDevices.h"

/**
 * @ingroup WindowsPlatform
 * @brief Class for windows output device
 */
class FWindowsOutputDevice : public FOutputDevice
{
public:
    /**
     * @ingroup WindowsPlatform
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void Serialize( const tchar* InMessage, EEventType InEvent );
};

#endif // !WINDOWSOUTPUTDEVICES_H
