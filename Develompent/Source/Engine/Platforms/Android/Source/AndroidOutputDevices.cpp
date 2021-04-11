#include <android/log.h>

#include "AndroidOutputDevices.h"

void FAndroidOutputDevice::Serialize( const tchar* InMessage, EEventType InEvent )
{
    const char*     strEventType = "";
    int             logPriority = 0;

    switch ( InEvent )
    {
        case FOutputDevice::EEventType::Log:
            strEventType = "Log";
            logPriority = ANDROID_LOG_INFO;
            break;

        case FOutputDevice::EEventType::Warning:
            strEventType = "Warn";
            logPriority = ANDROID_LOG_WARN;
            break;

        case FOutputDevice::EEventType::Critical:
            strEventType = "Error";
            logPriority = ANDROID_LOG_ERROR;
            break;

        case FOutputDevice::EEventType::Init:
            strEventType = "Init";

        default:
            logPriority = ANDROID_LOG_INFO;
            break;
    }

    __android_log_print( logPriority, strEventType, "%ls", ( const long* )InMessage );
}