/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BUTTONBAR_H
#define BUTTONBAR_H

#include <wx/wx.h>
#include <string>

#include "Misc/Types.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Level viewport tool bar
 */
class WxLevelViewportToolBar : public wxToolBar
{
public:
	/**
	 * @brief Constructor
	 */
	WxLevelViewportToolBar( wxWindow* InParent, wxWindowID InID );

	/**
	 * @brief Get height of toolbar
	 * @return Returns the level viewport toolbar height, in pixels
	 */
	static FORCEINLINE uint32 GetToolbarHeight()
	{
		return 28;
	}

private:
};

#endif // !BUTTONBAR_H