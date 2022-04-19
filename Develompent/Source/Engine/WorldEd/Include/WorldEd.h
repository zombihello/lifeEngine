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
int32 appWorldEdEntry( const tchar* InCmdLine );

/**
 * @ingroup WorldEd
 * Get WorldEd name
 *
 * @return Return WorldEd name
 */
std::wstring appGetWorldEdName();

#endif // !WORLDED_H