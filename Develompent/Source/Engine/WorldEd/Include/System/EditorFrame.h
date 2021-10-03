/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORFRAME_H
#define EDITORFRAME_H

#include <wx/wx.h>

/**
 * @ingroup WorldEd
 * The overall editor frame (houses everything)
 */
class WxEditorFrame : public wxFrame
{
	// Used for dynamic creation of the window. This must be declared for any
	// subclasses of WxEditorFrame
	DECLARE_DYNAMIC_CLASS( WxEditorFrame );

public:
	/**
	 * Constructor
	 */
	WxEditorFrame();

	/**
	 * Destructor
	 */
	virtual ~WxEditorFrame();

	/**
	 * Create editor frame
	 */
	virtual void Create();

private:
	/**
	 * Event of close frame
	 */
	void OnClose( wxCloseEvent& InEvent );

	/**
	 * Callback of click on file -> new map
	 */
	void UI_MenuFileNewMap( wxCommandEvent& InCommandEvent );

	/**
	 * Callback of click on file -> exit
	 */
	void UI_MenuFileExit( wxCommandEvent& InCommandEvent );

	/**
	 * Callback of click on help -> about
	 */
	void UI_MenuHelpAbout( wxCommandEvent& InCommandEvent );

	class WxMainMenu*		mainMenuBar;	/**< Main menu bar */
	wxPoint					framePos;		/**< Window position */
	wxSize					frameSize;		/**< Window size */
	bool					frameMaximized;	/**< Window maximized state */

	DECLARE_EVENT_TABLE();
};

/**
 * The menu that sits at the top of the main editor frame
 */
class WxMainMenu : public wxMenuBar
{
public:
	/**
	 * Constructor
	 */
	WxMainMenu();

	/**
	 * Destructor
	 */
	virtual ~WxMainMenu();

private:
	wxMenu*			fileMenu;		/**< File menu */
	wxMenu*			toolsMenu;		/**< Tools menu */
	wxMenu*			helpMenu;		/**< Help menu */
};

#endif // !EDITORFRAME_H