/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWABOUT_H
#define WINDOWABOUT_H

#include <wx/wx.h>

class WxWindowAbout : public wxDialog
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InParent Parent window
	 */
	WxWindowAbout( wxWindow* InParent );

	/**
	 * Destructor
	 */
	virtual ~WxWindowAbout();

	/**
	 * Create window about
	 * 
	 * @param[in] InParent Parent window
	 */
	static void Create( wxWindow* InParent );

private:
	/**
	 * Event of close
	 * 
	 * @param[in] InEvent Event
	 */
	void OnClose( wxCloseEvent& InEvent );

	DECLARE_EVENT_TABLE();
};

#endif // !WINDOWABOUT_H