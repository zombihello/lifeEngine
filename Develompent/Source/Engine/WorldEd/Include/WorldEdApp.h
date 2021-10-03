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
	 * Constructor
	 */
	WxWorldEdApp();

	/**
	 * Gets called on initialization from within wxEntry.
	 */
	virtual bool OnInit();

	/**
	 * Get editor name
	 */
	static std::wstring GetEditorName();

private:
	/**
	 * Uses file configuration to determine which class to use to create
	 * the editor's frame. Will fall back to hardcoded frame if it is
	 * unable to create the configured one properly
	 * 
	 * @return Return pointer to editor frame
	 */
	class WxEditorFrame* CreateEditorFrame();

	class WxEditorFrame*			editorFrame;		/**< The overall editor frame (houses everything) */
};

#endif // !WORLDEDAPP_H