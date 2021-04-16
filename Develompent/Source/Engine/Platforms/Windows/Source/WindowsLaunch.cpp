#include "Core.h"
#include "BaseArchive.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
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
    check( false );

    GLog->TearDown();
    return 0;
}