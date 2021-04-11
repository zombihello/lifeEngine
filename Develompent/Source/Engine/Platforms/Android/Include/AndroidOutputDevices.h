/**
 * @file
 * @addtogroup AndroidPlatform Android platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/27/2021
 */

#ifndef LIFEENGINE_ANDROIDOUTPUTDEVICES_H
#define LIFEENGINE_ANDROIDOUTPUTDEVICES_H

#include "OutputDevices.h"

/**
 * @ingroup AndroidPlatform
 * @brief Class for android output device
 */
class FAndroidOutputDevice : public FOutputDevice
{
public:
    /**
     * @ingroup AndroidPlatform
     * @brief Serialize message
     *
     * @param[in] InMessage Message
     * @param[in] InEvent Type event of message
     */
    virtual void Serialize( const tchar* InMessage, EEventType InEvent );
};

#endif //LIFEENGINE_ANDROIDOUTPUTDEVICES_H
