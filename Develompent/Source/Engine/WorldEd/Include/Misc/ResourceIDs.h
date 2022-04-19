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
	RID_Unused = wxID_HIGHEST + 1,		/**< Unused */
	RID_NewLevel,						/**< New level */
	RID_Open,							/**< Open level */
	RID_Save,							/**< Save level */
	RID_SaveAs,							/**< Save as level */
	RID_Exit,							/**< Exit from editor */
	RID_HelpAboutBox					/**< Help about box */
};

#endif // !RESOURCEIDS_H