#include "System/EditorFrame.h"
#include "Containers/String.h"
#include "System/Config.h"
#include "Render/Viewport.h"
#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/ResourceIDs.h"
#include "System/EditorEngine.h"
#include "System/ButtonBar.h"
#include "System/StatusBars.h"
#include "System/ViewportsContainer.h"
#include "System/Viewports.h"
#include "System/WorldEdApp.h"
#include "WorldEd.h"

// Windows
#include "Windows/WindowAbout.h"

// Used for dynamic creation of the window. This must be declared for any
// subclasses of WxEditorFrame
wxIMPLEMENT_DYNAMIC_CLASS( WxEditorFrame, wxFrame );

//----------------------------------------------------
//   WxMainMenu
//----------------------------------------------------

WxMainMenu::WxMainMenu()
{
	// Allocations
	fileMenu			= new wxMenu();
	viewMenu			= new wxMenu();
	viewportConfigMenu	= new wxMenu();
	toolsMenu			= new wxMenu();
	helpMenu			= new wxMenu();

	// File menu
	{
		fileMenu->Append( RID_New, TEXT( "New Level" ), nullptr, TEXT( "Create new level" ) );
		fileMenu->Append( RID_Open, TEXT( "Open" ), nullptr, TEXT( "Open level" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( RID_Save, TEXT( "Save" ), nullptr, TEXT( "Save level" ) );
		fileMenu->Append( RID_SaveAs, TEXT( "Save As" ), nullptr, TEXT( "Save as level" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( RID_Exit, TEXT( "Exit" ), nullptr, TEXT( "Exit from editor" ) );

		Append( fileMenu, TEXT( "File" ) );
	}

	// View menu
	{
		// Viewport configuration
		{
			const std::vector< struct FViewportConfig_Template* >&		viewportConfigTemplates = GApp->GetEditorFrame()->GetViewportConfigTemplates();
			for ( uint32 index = 0, count = viewportConfigTemplates.size(); index < count; ++index )
			{
				FViewportConfig_Template*		viewportConfigTemplate = viewportConfigTemplates[ index ];
				viewportConfigMenu->AppendCheckItem( RID_ViewportConfig_Start + index, viewportConfigTemplate->description.c_str(), FString::Format( TEXT( "Switch to the '%s' viewport configuration" ), viewportConfigTemplate->description.c_str() ).c_str() );
			}
		}

		viewMenu->AppendSeparator();
		viewMenu->Append( RID_ViewportConfig, TEXT( "Viewport configuration" ), viewportConfigMenu );
		viewMenu->AppendCheckItem( RID_ViewportResizeTogether, TEXT( "Resize Top And Bottom Viewports Together" ), TEXT( "Locks the vertical sash so that the top and bottom viewports share the same width" ) )->Check( GApp->GetEditorFrame()->IsViewportsResizeTogether() );
		
		Append( viewMenu, TEXT( "View" ) );
	}

	// Tools menu
	{
		Append( toolsMenu, TEXT( "Tools" ) );
	}

	// Help menu
	{
		helpMenu->Append( RID_HelpAboutBox, FString::Format( TEXT( "About %s" ), appGetWorldEdName().c_str() ), nullptr, TEXT( "About" ) );

		Append( helpMenu, TEXT( "Help" ) );
	}
}

WxMainMenu::~WxMainMenu()
{}

//----------------------------------------------------
//   WxMainToolBar
//----------------------------------------------------
WxMainToolBar::WxMainToolBar( wxWindow* InParent, wxWindowID InID )
	: wxToolBar( InParent, InID, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER )
{
	// Load bitmaps
	bitmapNew.Load( TEXT( "New" ) );
	bitmapOpen.Load( TEXT( "Open" ) );
	bitmapSave.Load( TEXT( "Save" ) );
	bitmapSaveAll.Load( TEXT( "SaveAll" ) );

	// Make tool bar
	AddSeparator();
	AddTool( RID_New, TEXT( "" ), bitmapNew, TEXT( "Create new level" ) );
	AddTool( RID_Open, TEXT( "" ), bitmapOpen, TEXT( "Open level" ) );
	AddTool( RID_Save, TEXT( "" ), bitmapSave, TEXT( "Save level" ) );
	AddTool( RID_SaveAll, TEXT( "" ), bitmapSaveAll, TEXT( "Save all levels" ) );
	AddSeparator();

	Realize();
}

//----------------------------------------------------
//   WxEditorFrame
//----------------------------------------------------

WxEditorFrame::WxEditorFrame() 
	: bViewportResizeTogether( true )
	, mainMenuBar( nullptr )
	, mainToolBar( nullptr )
	, buttonBar( nullptr )
	, statusBar( nullptr )
	, viewportContainer( nullptr )
	, viewportConfigData( nullptr )
	, framePos( 0, 0 )
	, frameSize( 1280, 720 )
	, frameMaximized( true )
{}

WxEditorFrame::~WxEditorFrame()
{
	for ( uint32 index = 0, count = viewportConfigTemplates.size(); index < count; ++index )
	{
		delete viewportConfigTemplates[ index ];
	}
	delete viewportConfigData;
}

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
	const bool			success = wxFrame::Create( nullptr, -1, appGetWorldEdName(), framePos, frameSize );
	check( success );
}

void WxEditorFrame::SetUp()
{
	// Set icon for window
#if PLATFORM_WINDOWS
	SetIcon( wxICON( IDI_ICON1 ) );
#endif // PLATFORM_WINDOWS

	if ( frameMaximized )
	{
		Maximize( true );
		frameSize = GetSize();
	}

	// Viewport configuration options
	FViewportConfig_Template*		templateViewportConfig = nullptr;
	
	// 2x2 split
	templateViewportConfig = new FViewportConfig_Template();
	templateViewportConfig->Set( VC_2_2_Split );
	viewportConfigTemplates.push_back( templateViewportConfig );

	// 1x2 split
	templateViewportConfig = new FViewportConfig_Template();
	templateViewportConfig->Set( VC_1_2_Split );
	viewportConfigTemplates.push_back( templateViewportConfig );

	// 1x1 split horizontal
	templateViewportConfig = new FViewportConfig_Template();
	templateViewportConfig->Set( VC_1_1_SplitH );
	viewportConfigTemplates.push_back( templateViewportConfig );

	// 1x1 split vertical
	templateViewportConfig = new FViewportConfig_Template();
	templateViewportConfig->Set( VC_1_1_SplitV );
	viewportConfigTemplates.push_back( templateViewportConfig );

	// Create components of editor frame
	viewportContainer = new WxViewportsContainer( this, RID_ViewportContainer );
	viewportContainer->SetLabel( wxT( "ViewportContainer" ) );

	mainMenuBar = new WxMainMenu();
	SetMenuBar( mainMenuBar );

	mainToolBar = new WxMainToolBar( this, -1 );
	SetToolBar( mainToolBar );

	buttonBar = new WxButtonBar();
	buttonBar->Create( this, -1 );
	buttonBar->Show();

	statusBar = new WxStatusBarStandard();
	statusBar->Create( this, -1 );
	statusBar->SetUp();
	SetStatusBar( statusBar );
	
	// Create viewport for render
	viewportConfigData = new FViewportConfig_Data();
	if ( !viewportConfigData->LoadFromConfig() )
	{
		viewportConfigData->SetTemplate( VC_2_2_Split );
	}
	viewportConfigData->Apply( viewportContainer );

	// Clean up
	wxSizeEvent		dummyEvent;
	OnSize( dummyEvent );
}

void WxEditorFrame::UpdateUI()
{}

void WxEditorFrame::SetViewportConfig( EViewportConfig InViewportConfig )
{
	FViewportConfig_Data*		saveConfig = viewportConfigData;
	viewportConfigData			= new FViewportConfig_Data();

	viewportContainer->DestroyChildren();
	viewportConfigData->SetTemplate( InViewportConfig );
	viewportConfigData->Apply( viewportContainer );
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

void WxEditorFrame::OnSize( wxSizeEvent& InEvent )
{
	if ( !mainToolBar )
	{
		return;
	}

	// Resize button bar
	wxRect		clientRect = GetClientRect();
	buttonBar->SetSize( 0, 0, 72, clientRect.GetHeight() );

	// Figure out the client area remaining for viewports once the docked windows are taken into account
	wxSize		oldSize = viewportContainer->GetSize();
	wxSize		newSize( clientRect.GetWidth() - 72, clientRect.GetHeight() );
	if ( viewportConfigData )
	{
		viewportConfigData->ResizeProportionally( ( float )newSize.x / oldSize.x, ( float )newSize.y / oldSize.y, false );
	}
	viewportContainer->SetSize( 72, 0, newSize.x, newSize.y );
}

void WxEditorFrame::UI_MenuHelpAbout( wxCommandEvent& InCommandEvent )
{
	WxWindowAbout::Create( this );
}

void WxEditorFrame::UI_MenuViewportConfig( wxCommandEvent& InCommandEvent )
{
	EViewportConfig			viewportConfig = VC_2_2_Split;
	switch ( InCommandEvent.GetId() )
	{
	case RID_ViewportConfig_2_2_Split:		viewportConfig = VC_2_2_Split;		break;
	case RID_ViewportConfig_1_2_Split:		viewportConfig = VC_1_2_Split;		break;
	case RID_ViewportConfig_1_1_Split_H:	viewportConfig = VC_1_1_SplitH;		break;
	case RID_ViewportConfig_1_1_Split_V:	viewportConfig = VC_1_1_SplitV;		break;
	}

	SetViewportConfig( viewportConfig );
}

void WxEditorFrame::UI_MenuViewportResizeTogether( wxCommandEvent& InCommandEvent )
{
	if ( InCommandEvent.IsChecked() )
	{
		bViewportResizeTogether = true;

		// Make the splitter sashes match up
		viewportContainer->MatchSplitterPositions();
	}
	else
	{
		bViewportResizeTogether = false;
	}
}

void WxEditorFrame::MenuViewportConfig( wxUpdateUIEvent& InCommandEvent )
{
	switch ( InCommandEvent.GetId() )
	{
	case RID_ViewportConfig_2_2_Split:			InCommandEvent.Check( viewportConfigData->GetTemplate() == VC_2_2_Split );			break;
	case RID_ViewportConfig_1_2_Split:			InCommandEvent.Check( viewportConfigData->GetTemplate() == VC_1_2_Split );			break;
	case RID_ViewportConfig_1_1_Split_H:		InCommandEvent.Check( viewportConfigData->GetTemplate() == VC_1_1_SplitH );			break;
	case RID_ViewportConfig_1_1_Split_V:		InCommandEvent.Check( viewportConfigData->GetTemplate() == VC_1_1_SplitV );			break;
	}
}

//----------------------------------------------------
//   Events of WxEditorFrame
//----------------------------------------------------

BEGIN_EVENT_TABLE( WxEditorFrame, wxFrame )
	EVT_CLOSE( WxEditorFrame::OnClose )
	EVT_SIZE( WxEditorFrame::OnSize )

	EVT_MENU( RID_New, WxEditorFrame::UI_MenuFileNewMap )
	EVT_MENU( RID_Exit, WxEditorFrame::UI_MenuFileExit )
	EVT_MENU( RID_HelpAboutBox, WxEditorFrame::UI_MenuHelpAbout )
	EVT_MENU_RANGE( RID_ViewportConfig_Start, RID_ViewportConfig_End, WxEditorFrame::UI_MenuViewportConfig )
	EVT_MENU( RID_ViewportResizeTogether, WxEditorFrame::UI_MenuViewportResizeTogether )

	EVT_UPDATE_UI_RANGE( RID_ViewportConfig_Start, RID_ViewportConfig_End, WxEditorFrame::MenuViewportConfig )

END_EVENT_TABLE()