#include <Windows.h>
#include "core/core.h"
#include "core/debug.h"
#include "stdlib/strtools.h"
#include "core/threading.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	std::string	a = "hello";
	L_strupper( (achar*)a.c_str() );

	std::wstring	z = L"hello";
	L_wstrupper( (wchar*)z.c_str() );

	CThreadMutex qq;
	qq.Lock();
	qq.Unlock();

	int32 q = 0;
	Sys_InterlockedIncrement( &q );
	Assert( Sys_IsInMainThread() );

	achar* pStr = ( achar* )malloc( 10 * sizeof( achar ) );
	achar* pSrt2 = new achar[10];

	Msg( "Hello Msg %i %s", Sys_BuildNumber(), a.c_str() );
	Warning( "Hello Warning" );
	Error( "Hello Error" );
	Assert( false );
	Sys_Error( "Hello Sys_Error" );
	return 0;
}