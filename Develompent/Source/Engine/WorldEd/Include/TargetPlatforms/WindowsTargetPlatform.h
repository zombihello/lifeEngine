/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSTARGETPLATFORM_H
#define WINDOWSTARGETPLATFORM_H

#include "System/BaseTargetPlatform.h"

 /**
  * @ingroup WorldEd
  * @brief Windows target platform
  */
class CWindowsTargetPlatform : public CBaseTargetPlatform
{
public:
	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CWindowsTargetPlatform& Get()
	{
		static CWindowsTargetPlatform	s_WindowsTargetPlatform;
		return s_WindowsTargetPlatform;
	}
};

#endif // !WINDOWSTARGETPLATFORM_H