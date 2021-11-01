#include <Windows.h>

#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "Core.h"

double appInitTiming()
{
	LARGE_INTEGER		frequency;
	check( QueryPerformanceFrequency( &frequency ) );

	GSecondsPerCycle = 1.0 / frequency.QuadPart;
	return appSeconds();
}