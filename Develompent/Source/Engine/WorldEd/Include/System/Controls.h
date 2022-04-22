/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONTROLS_H
#define CONTROLS_H

#include <wx/wx.h>
#include <string>

#include "Misc/Types.h"

/**
 * @ingroup WorldEd
 * @brief Base class for editor status bars
 */
class WxStatusBar : public wxStatusBar
{
public:
	/**
	 * @brief Constructor
	 */
	WxStatusBar();

	/**
	 * @brief Constructor
	 * 
	 * @param InParent		Parent window
	 * @param InID			ID
	 */
	WxStatusBar( wxWindow* InParent, wxWindowID InID );

	/**
	 * @brief Destructor
	 */
	virtual ~WxStatusBar();

	/**
	 * @brief Sets the provided string to the status bar field at index 'InIndex'
	 *
	 * @param InText	New status field text
	 * @param InIndex	Field to set text to
	 */
	virtual void SetStatusText( const wxString& InText, uint32 InIndex = 0 );

	/**
	 * @brief Init UI
	 */
	virtual void SetUp() = 0;

	/**
	 * @brief Update UI
	 */
	virtual void UpdateUI() = 0;

	/**
	 * @brief Update the position and size of the status bar
	 */
	virtual void RefreshPositionAndSize() = 0;

	/**
	 * @brief Sets the mouse world space position text field to the text passed in
	 * @param InStatusText	 String to use as the new text for the world space position field
	 */
	virtual void SetMouseWorldspacePositionText( const std::wstring& InStatusText );
};

#endif // !CONTROLS_H