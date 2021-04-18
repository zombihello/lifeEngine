#include <SDL.h>

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

    // Print version SDL to logs
    {
		SDL_version		sdlVersion;
		SDL_GetVersion( &sdlVersion );
        LE_LOG( LT_Log, LC_General, TEXT( "SDL version: %i.%i.%i" ), sdlVersion.major, sdlVersion.minor, sdlVersion.patch );
    }
    
    GWindow->Create( TEXT( "TestbedGame" ), 1280, 720 );
    GWindow->ShowCursor();
    bool isLoop = true;

    while( isLoop )
    {
        SWindowEvent      event;
        while ( GWindow->PollEvent( event ) ) 
        {
            if ( event.type == SWindowEvent::T_WindowClose || ( event.type == SWindowEvent::T_KeyPressed && event.events.key.code == BC_KeyQ ) )
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