/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSMISC_H
#define WINDOWSMISC_H

#include <Windows.h>
#include "Misc/CoreGlobals.h"

/**
 * @ingroup WindowsPlatform
 * Get time in seconds. Origin is arbitrary
 * @reutrn Return time in seconds
 */
FORCEINLINE double appSeconds()
{
	LARGE_INTEGER		cycles;
	QueryPerformanceCounter( &cycles );

	// Add big number to make bugs apparent where return value is being passed to FLOAT
	return cycles.QuadPart * g_SecondsPerCycle + 16777216.0;
}

#endif // !WINDOWSMISC_H