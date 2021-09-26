/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LAUNCHAPP_H
#define LAUNCHAPP_H

#include <wx/wx.h>

/**
 * @ingroup WorldEd
 * Base wxApp implemenation used for WorldEdApp to inherit from.
 */
class WxLaunchApp : public wxApp
{
public:
	/**
	 * Destructor
	 */
	virtual ~WxLaunchApp();

	/**
	 * Gets called on initialization from within wxEntry.
	 */
	virtual bool OnInit();

	/**
	 * Gets called after leaving main loop before wxWindows is shut down.
	 */
	virtual int OnExit();

	/**
	 * Callback from wxWindows' main loop to signal that we should tick the engine.
	 */
	virtual void TickLE();

	/**
	 * Performs any required cleanup in the case of a fatal error.
	 */
	virtual void ShutdownAfterError();
};

#endif // !LAUNCHAPP_H