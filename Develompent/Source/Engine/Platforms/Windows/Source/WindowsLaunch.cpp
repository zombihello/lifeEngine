#include "Core.h"
#include "WindowsOutputDevices.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
    static_cast< WindowsOutputDevice* >( GLog )->Show( true );

    // This for test
    debugf( TEXT( "This is test" ) );
    warnf( OutputDevice::EEventType::Warning, TEXT( "This is warning" ) );
    check( false );

    return 0;
}