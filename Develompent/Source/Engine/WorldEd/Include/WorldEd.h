/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_H
#define WORLDED_H

#include <string>

#include "Misc/Types.h"

/**
 * @ingroup WorldEd
 * Main function for start WorldEd
 * 
 * @param[in] InCmdLine Arguments of start
 * @return Returns the error level, 0 if successful and > 0 if there were errors
 */
int32 WorldEdEntry( const tchar* InCmdLine );

#endif // !WORLDED_H