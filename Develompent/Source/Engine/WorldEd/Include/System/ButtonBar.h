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

/**
 * @ingroup WorldEd
 * @brief The button bar on the left side of the editor
 */
class WxButtonBar : public wxPanel
{
public:
	/**
	 * @brief Create button bar
	 * 
	 * @param InParent		Parent window
	 * @param InID			ID
	 * @param InPosition	Position of button bar
	 * @param InSize		Size of button bar
	 * @param InStyle		Style
	 * @param InName		Name
	 * @return Return TRUE if seccussed create, else return FALSE
	 */
	bool Create( wxWindow* InParent, wxWindowID InID, const wxPoint& InPosition = wxDefaultPosition, const wxSize& InSize = wxDefaultSize, uint64 InStyle = wxTAB_TRAVERSAL, const std::wstring& InName = TEXT( "Panel" ) );
};

#endif // !BUTTONBAR_H