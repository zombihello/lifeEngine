/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDEDAPP_H
#define WORLDEDAPP_H

#include "System/LaunchApp.h"

/**
 * @ingroup WorldEd
 * @brief Main class for work world editor
 */
class WxWorldEdApp : public WxLaunchApp
{
public:
	/**
	 * Gets called on initialization from within wxEntry.
	 */
	virtual bool OnInit();
};

#endif // !WORLDEDAPP_H