#include <Windows.h>
#include <stdio.h>

void appInitConsole()
{
    AllocConsole();
    freopen( "conin$","r", stdin );
    freopen( "conout$","w", stdout );
    freopen( "conout$","w", stderr );
}

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
    // Initialize console for print log in debug
    appInitConsole();

    return 0;
}