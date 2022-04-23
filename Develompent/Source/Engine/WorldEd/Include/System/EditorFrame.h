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
#include <wx/splitter.h>
#include <vector>

#include "System/Bitmaps.h"
#include "System/Viewports.h"

/**
 * @ingroup WorldEd
 * The overall editor frame (houses everything)
 */
class WxEditorFrame : public wxFrame
{
	// Used for dynamic creation of the window. This must be declared for any
	// subclasses of WxEditorFrame
	wxDECLARE_DYNAMIC_CLASS( WxEditorFrame );

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

	/**
	 * @brief Init UI
	 */
	virtual void SetUp();

	/**
	 * @brief Update UI
	 */
	virtual void UpdateUI();

	/**
	 * @brief Set viewport config
	 * @param InViewportConfig		New viewport config
	 */
	void SetViewportConfig( EViewportConfig InViewportConfig );

	/**
	 * @brief Is viewports resize together
	 * @return Returns whether or not we should link the vertical splitters for the top and bottom viewports
	 */
	FORCEINLINE bool IsViewportsResizeTogether() const
	{
		return bViewportResizeTogether;
	}

	/**
	 * @brief Get viewport config templates
	 * @return Return viewport config templates
	 */
	FORCEINLINE const std::vector< struct FViewportConfig_Template* >& GetViewportConfigTemplates() const
	{
		return viewportConfigTemplates;
	}

private:
	//
	// Event handlers
	//

	// Window handlers
	void OnClose( wxCloseEvent& InEvent );
	void OnSize( wxSizeEvent& InEvent );

	// Main menu handlers
	void UI_MenuFileNewMap( wxCommandEvent& InCommandEvent );
	void UI_MenuFileExit( wxCommandEvent& InCommandEvent );
	void UI_MenuHelpAbout( wxCommandEvent& InCommandEvent );
	void UI_MenuViewportResizeTogether( wxCommandEvent& InCommandEvent );
	void UI_MenuViewportConfig( wxCommandEvent& InCommandEvent );
	void MenuViewportConfig( wxUpdateUIEvent& InCommandEvent );

	bool												bViewportResizeTogether;	/**< Locks the vertical splitter for the top and bottom viewports together */
	class WxMainMenu*									mainMenuBar;				/**< Main menu bar */
	class WxMainToolBar*								mainToolBar;				/**< Main tool bar */
	class WxButtonBar*									buttonBar;					/**< Button bar on the left side of the editor */
	class WxStatusBar*									statusBar;					/**< Status bar */	
	class WxViewportsContainer*							viewportContainer;			/**< Holds all open level editing viewports */
	std::vector< struct FViewportConfig_Template* >		viewportConfigTemplates;	/**< Viewport config templates */
	class FViewportConfig_Data*							viewportConfigData;			/**< Viewport config data */
	wxPoint												framePos;					/**< Window position */
	wxSize												frameSize;					/**< Window size */
	bool												frameMaximized;				/**< Window maximized state */

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
	wxMenu*			fileMenu;				/**< File menu */
	wxMenu*			viewMenu;				/**< View menu */
	wxMenu*			viewportConfigMenu;		/**< Viewport config menu */
	wxMenu*			toolsMenu;				/**< Tools menu */
	wxMenu*			helpMenu;				/**< Help menu */
};

/**
 * The toolbar that sits at the top of the main editor frame
 */
class WxMainToolBar : public wxToolBar
{
public:
	/**
	 * Constructor
	 * 
	 * @param InParent Parent window
	 * @param InID ID
	 */
	WxMainToolBar( wxWindow* InParent, wxWindowID InID );

private:
	WxBitmap		bitmapNew;		/**< "New" image */
	WxBitmap		bitmapOpen;		/**< "Open" image */
	WxBitmap		bitmapSave;		/**< "Save" image */
	WxBitmap		bitmapSaveAll;	/**< "Save all" image */
};

#endif // !EDITORFRAME_H