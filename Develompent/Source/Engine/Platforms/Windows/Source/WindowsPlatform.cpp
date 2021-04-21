#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

#include "Misc/Types.h"
#include "D3D11RHI.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"

// ----
// Platform specific globals variables
// ----

BaseLogger*         GLog            = new WindowsLogger();
BaseFileSystem*     GFileSystem     = new WindowsFileSystem();
BaseWindow*         GWindow         = new WindowsWindow();
BaseRHI*            GRHI            = new D3D11RHI();

// ----
// Platform specific functions
// ----

/**
 * Get formatted string (for Unicode strings)
 */
int appGetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr )
{
    return vswprintf( InOutDest, InCount, InFormat, InArgPtr );
}

/**
 * Get formatted string (for ANSI strings)
 */
int appGetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr )
{
    return vsnprintf( InOutDest, InCount, InFormat, InArgPtr );
}