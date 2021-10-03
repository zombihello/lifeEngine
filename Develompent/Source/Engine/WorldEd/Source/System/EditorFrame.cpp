#include "System/EditorFrame.h"
#include "Containers/String.h"
#include "System/Config.h"
#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/ResourceIDs.h"
#include "WorldEdApp.h"

// Windows
#include "Windows/WindowAbout.h"

// Used for dynamic creation of the window. This must be declared for any
// subclasses of WxEditorFrame
IMPLEMENT_DYNAMIC_CLASS( WxEditorFrame, wxFrame );

//----------------------------------------------------
//   WxMainMenu
//----------------------------------------------------

WxMainMenu::WxMainMenu()
{
	// Allocations
	fileMenu = new wxMenu();
	toolsMenu = new wxMenu();
	helpMenu = new wxMenu();

	// File menu
	{
		fileMenu->Append( RID_NewLevel, TEXT( "New level" ), nullptr, TEXT( "Create new level" ) );
		fileMenu->Append( RID_Open, TEXT( "Open" ), nullptr, TEXT( "Open level" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( RID_Save, TEXT( "Save" ), nullptr, TEXT( "Save level" ) );
		fileMenu->Append( RID_SaveAs, TEXT( "Save as" ), nullptr, TEXT( "Save as level" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( RID_Exit, TEXT( "Exit" ), nullptr, TEXT( "Exit from editor" ) );

		Append( fileMenu, TEXT( "File" ) );
	}

	// Tools menu
	{
		Append( toolsMenu, TEXT( "Tools" ) );
	}

	// Help menu
	{
		helpMenu->Append( RID_HelpAboutBox, FString::Format( TEXT( "About %s" ), WxWorldEdApp::GetEditorName().c_str() ), nullptr, TEXT( "About" ) );

		Append( helpMenu, TEXT( "Help" ) );
	}
}

WxMainMenu::~WxMainMenu()
{}

//----------------------------------------------------
//   WxEditorFrame
//----------------------------------------------------

WxEditorFrame::WxEditorFrame() :
	mainMenuBar( nullptr ),
	framePos( 0, 0 ),
	frameSize( 1280, 720 ),
	frameMaximized( true )
{}

WxEditorFrame::~WxEditorFrame()
{}

void WxEditorFrame::Create()
{
	// Get frame position from config
	{
		FConfigObject		pos = GEditorConfig.GetValue( TEXT( "Editor.EditorFrame" ), TEXT( "Pos" ) ).GetObject();
		framePos.x = pos.GetValue( TEXT( "X" ) ).GetInt();
		framePos.y = pos.GetValue( TEXT( "Y" ) ).GetInt();
	}

	// Get frame size from config
	{
		FConfigObject		size = GEditorConfig.GetValue( TEXT( "Editor.EditorFrame" ), TEXT( "Size" ) ).GetObject();
		frameSize.x = size.GetValue( TEXT( "X" ) ).GetInt();
		frameSize.y = size.GetValue( TEXT( "Y" ) ).GetInt();
	}

	// Get IsMaximized window from config
	{
		frameMaximized = GEditorConfig.GetValue( TEXT( "Editor.EditorFrame" ), TEXT( "IsMaximized" ) ).GetBool();
	}

	// Assert if this fails
	const bool			success = wxFrame::Create( nullptr, -1, WxWorldEdApp::GetEditorName(), framePos, frameSize );
	check( success );

	if ( frameMaximized )
	{
		Maximize( true );
	}

	// Create components of editor frame
	mainMenuBar = new WxMainMenu();
	SetMenuBar( mainMenuBar );
}

void WxEditorFrame::UI_MenuFileNewMap( wxCommandEvent& InCommandEvent )
{}

void WxEditorFrame::UI_MenuFileExit( wxCommandEvent& InCommandEvent )
{
	Close();
}

void WxEditorFrame::OnClose( wxCloseEvent& InEvent )
{
	Destroy();
}

void WxEditorFrame::UI_MenuHelpAbout( wxCommandEvent& InCommandEvent )
{
	WxWindowAbout::Create( this );
}

//----------------------------------------------------
//   Events of WxEditorFrame
//----------------------------------------------------

BEGIN_EVENT_TABLE( WxEditorFrame, wxFrame )
	EVT_CLOSE( WxEditorFrame::OnClose )

	EVT_MENU( RID_NewLevel, WxEditorFrame::UI_MenuFileNewMap )
	EVT_MENU( RID_Exit, WxEditorFrame::UI_MenuFileExit )
	EVT_MENU( RID_HelpAboutBox, WxEditorFrame::UI_MenuHelpAbout )
END_EVENT_TABLE()