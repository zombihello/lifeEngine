#include <Windows.h>

#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "Core.h"

/*
==================
Sys_InitTiming
==================
*/
double Sys_InitTiming()
{
	LARGE_INTEGER		frequency;
	bool				result = QueryPerformanceFrequency( &frequency );
	Assert( result );

	g_SecondsPerCycle = 1.0 / frequency.QuadPart;
	return appSeconds();
}