#include "Core.h"
#include "System/BaseArchive.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"
#include "Logger/LoggerMacros.h"

/**
 * Main function
 */
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
    GLog->Init();
    static_cast< WindowsLogger* >( GLog )->Show( true );

    // This for test
    LE_LOG( LT_Log, LC_General, TEXT( "This is test. Value = %i" ), 25 );
    LE_LOG( LT_Warning, LC_General, TEXT( "This is warning" ) );
    
    GWindow->Create( TEXT( "TestbedGame" ), 1280, 720 );
    GWindow->ShowCursor();
    bool isLoop = true;

    while( isLoop )
    {
        SWindowEvent      event;
        while ( GWindow->PollEvent( event ) ) 
        {
            if ( event.type == SWindowEvent::T_WindowClose )
            {
                isLoop = false;
                GWindow->Close();
                break;
            }
        }
    }

    //GWindow->Close();
    GLog->TearDown();
    return 0;
}