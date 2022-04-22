/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RESOURCEIDS_H
#define RESOURCEIDS_H

#include <wx/wx.h>

/**
 * @ingroup WorldEd
 * Resource IDs of WxWidgets
 */
enum EResourceID
{
	RID_Unused							= wxID_HIGHEST + 1,						/**< Unused */
	RID_New,																	/**< New */
	RID_Open,																	/**< Open */
	RID_Save,																	/**< Save */
	RID_SaveAs,																	/**< Save as */
	RID_SaveAll,																/**< Save all */
	RID_Exit,																	/**< Exit */
	RID_HelpAboutBox,															/**< Help about box */
	RID_SplitterWindow,															/**< Splitter window */
	RID_SplitterWindow_End				= RID_SplitterWindow + 20,				/**< Reserve enough IDs for multiple splitters in one frame */
	RID_ViewportContainer,														/**< Viewport container */
	RID_ViewportResizeTogether,													/**< Viewport resize together */
	RID_ViewportConfig,															/**< Viewport config */
	RID_ViewportConfig_Start,													/**< Start ID of viewport config */
		RID_ViewportConfig_2_2_Split	= RID_ViewportConfig_Start,				/**< Viewport config for 2x2 split */
		RID_ViewportConfig_1_2_Split,											/**< Viewport config for 1x2 split */
		RID_ViewportConfig_1_1_Split_H,											/**< Viewport config for 1x1 split horizontal */
		RID_ViewportConfig_1_1_Split_V,											/**< Viewport config for 1x1 split vertical */
	RID_ViewportConfig_End				= RID_ViewportConfig_1_1_Split_V,		/**< End ID of viewport config */
};

#endif // !RESOURCEIDS_H