#include <wx/wx.h>
#include <wx/splitter.h>

#include "Math/Math.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/ResourceIDs.h"
#include "System/WorldEdApp.h"
#include "System/Viewports.h"
#include "System/EditorFrame.h"
#include "System/ViewportsContainer.h"
#include "System/LeveViewportToolBar.h"
#include "System/EditorEngine.h"
#include "Render/Viewport.h"
#include "Core.h"

/** Default camera position for level editor perspective viewports */
//static const FVector		GDefaultPerspectiveViewLocation( -1024.0f, 512.0f, 0.f );
static const FVector		GDefaultPerspectiveViewLocation( 495.166962, 167.584518, -400.f );

/** Default camera orientation for level editor perspective viewports */
//static const FRotator		GDefaultPerspectiveViewRotation( FMath::Round( -15.0f * ( 32768.f / 180.f ) ), 0.f, 0.f );
static const FRotator		GDefaultPerspectiveViewRotation( 0,0,0 );

//----------------------------------------------------
//   FViewportConfig_Viewport
//----------------------------------------------------

FViewportConfig_Viewport::FViewportConfig_Viewport()
	: bEnabled( false )
	, bSetListenerPosition( false )
	, viewportType( LVT_Perspective )
	, showFlags( SHOW_DefaultEditor )
{}

FViewportConfig_Viewport::~FViewportConfig_Viewport()
{}

//----------------------------------------------------
//   FViewportConfig_Template
//----------------------------------------------------

FViewportConfig_Template::FViewportConfig_Template()
{}

FViewportConfig_Template::~FViewportConfig_Template()
{}

void FViewportConfig_Template::Set( EViewportConfig InViewportConfig )
{
	viewportConfig = InViewportConfig;
	switch ( viewportConfig )
	{
	case VC_2_2_Split:
		description = TEXT( "2x2 Split" );

		viewportTemplates[ 0 ].bEnabled				= true;
		viewportTemplates[ 0 ].viewportType			= LVT_OrthoXY;
		viewportTemplates[ 0 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;

		viewportTemplates[ 1 ].bEnabled				= true;
		viewportTemplates[ 1 ].viewportType			= LVT_OrthoXZ;
		viewportTemplates[ 1 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;

		viewportTemplates[ 2 ].bEnabled				= true;
		viewportTemplates[ 2 ].bSetListenerPosition = true;
		viewportTemplates[ 2 ].viewportType			= LVT_Perspective;
		viewportTemplates[ 2 ].showFlags			= SHOW_DefaultEditor;

		viewportTemplates[ 3 ].bEnabled				= true;
		viewportTemplates[ 3 ].viewportType			= LVT_OrthoYZ;
		viewportTemplates[ 3 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;
		break;

	case VC_1_2_Split:
		description = TEXT( "1x2 Split" );

		viewportTemplates[ 0 ].bEnabled				= true;
		viewportTemplates[ 0 ].bSetListenerPosition	= true;
		viewportTemplates[ 0 ].viewportType			= LVT_Perspective;
		viewportTemplates[ 0 ].showFlags			= SHOW_DefaultEditor;

		viewportTemplates[ 1 ].bEnabled				= true;
		viewportTemplates[ 1 ].viewportType			= LVT_OrthoXY;
		viewportTemplates[ 1 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;

		viewportTemplates[ 2 ].bEnabled				= true;
		viewportTemplates[ 2 ].viewportType			= LVT_OrthoXZ;
		viewportTemplates[ 2 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;
		break;

	case VC_1_1_SplitH:
		description = TEXT( "1x1 Split Horizontal" );

		viewportTemplates[ 0 ].bEnabled				= true;
		viewportTemplates[ 0 ].bSetListenerPosition = true;
		viewportTemplates[ 0 ].viewportType			= LVT_Perspective;
		viewportTemplates[ 0 ].showFlags			= SHOW_DefaultEditor;

		viewportTemplates[ 1 ].bEnabled				= true;
		viewportTemplates[ 1 ].viewportType			= LVT_OrthoXY;
		viewportTemplates[ 1 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;
		break;

	case VC_1_1_SplitV:
		description = TEXT( "1x1 Split Vertical" );

		viewportTemplates[ 0 ].bEnabled				= true;
		viewportTemplates[ 0 ].bSetListenerPosition = true;
		viewportTemplates[ 0 ].viewportType			= LVT_Perspective;
		viewportTemplates[ 0 ].showFlags			= SHOW_DefaultEditor;

		viewportTemplates[ 1 ].bEnabled				= true;
		viewportTemplates[ 1 ].viewportType			= LVT_OrthoXY;
		viewportTemplates[ 1 ].showFlags			= SHOW_DefaultEditor | SHOW_Wireframe;
		break;

	default:
		check( false );			// Unknown viewport config
		break;
	}
}

//----------------------------------------------------
//   FVCD_Viewport
//----------------------------------------------------

FVCD_Viewport::FVCD_Viewport()
	: viewportWindow( nullptr )
{}

FVCD_Viewport::~FVCD_Viewport()
{}

//----------------------------------------------------
//   WxLevelViewportWindow
//----------------------------------------------------

WxLevelViewportWindow::WxLevelViewportWindow()
	: toolBar( nullptr )
	, viewport( nullptr )
{}

WxLevelViewportWindow::~WxLevelViewportWindow()
{
	// Destroy viewport
	viewport->Update( true, 0, 0, nullptr );
	viewport->SetViewportClient( nullptr );
	GEditorEngine->RemoveViewport( viewport );

	// Wait while viewport RHI is not deleted
	while ( viewport->IsValid() )
	{
		appSleep( 0.1f );
	}

	delete viewport;
}

void WxLevelViewportWindow::SetUp( ELevelViewportType InViewportType, bool InSetListenerPosition, EShowFlags InShowFlags )
{
	wxRect	rc				= GetClientRect();
	bSetListenerPosition	= InSetListenerPosition;
	viewportType			= InViewportType;
	showFlags				= InShowFlags;

	// Create render viewport
	viewport	= new FViewport();
	viewport->SetViewportClient( this );
	viewport->Update( false, rc.GetWidth(), rc.GetHeight(), GetHWND() );
	GEditorEngine->AddViewport( viewport );

	// Assign default camera location/rotation for perspective camera
	if ( InViewportType == LVT_Perspective )
	{
		viewLocation = GDefaultPerspectiveViewLocation;
		viewRotation = GDefaultPerspectiveViewRotation;
	}

	// ToolBar
	toolBar = new WxLevelViewportToolBar( this, -1 );
	toolBar->SetLabel( wxT( "ToolBar" ) );

	wxSizeEvent dummyEvent;
	OnSize( dummyEvent );
}

void WxLevelViewportWindow::OnSize( wxSizeEvent& InEvent )
{
	if ( toolBar )
	{
		wxRect		rc = GetClientRect();
		rc.y		+= WxLevelViewportToolBar::GetToolbarHeight();
		rc.height	-= WxLevelViewportToolBar::GetToolbarHeight();
		toolBar->SetSize( rc.GetWidth(), WxLevelViewportToolBar::GetToolbarHeight() );

		uint32		newViewportSizeX = Max( rc.GetWidth() - 2, 1 );
		uint32		newViewportSizeY = Max( rc.GetHeight() - 2, 1 );
		viewport->Update( false, newViewportSizeX, newViewportSizeY, viewport->GetWindowHandle() );
	}

	InEvent.Skip();
}

//
// Events
//

BEGIN_EVENT_TABLE( WxLevelViewportWindow, wxWindow )
	EVT_SIZE( WxLevelViewportWindow::OnSize )
END_EVENT_TABLE()

//----------------------------------------------------
//   FViewportConfig_Data
//----------------------------------------------------

FViewportConfig_Data::FViewportConfig_Data()
	: sizer( nullptr )
{}

FViewportConfig_Data::~FViewportConfig_Data()
{}

void FViewportConfig_Data::SetTemplate( EViewportConfig InTemplate )
{
	templateViewport = InTemplate;

	// Find the template
	FViewportConfig_Template*									viewportConfigTemplate = nullptr;
	const std::vector< struct FViewportConfig_Template* >&		viewportConfigTemplates = GApp->GetEditorFrame()->GetViewportConfigTemplates();
	for ( uint32 index = 0, count = viewportConfigTemplates.size(); index < count; ++index  )
	{
		FViewportConfig_Template*		tmpViewportConfigTemplate = viewportConfigTemplates[ index ];
		if ( tmpViewportConfigTemplate->viewportConfig == InTemplate )
		{
			viewportConfigTemplate = tmpViewportConfigTemplate;
			break;
		}
	}

	check( viewportConfigTemplate );	// If NULL, the template config type is unknown

	// Copy the templated data into our local vars
	*this = *viewportConfigTemplate;
}

void FViewportConfig_Data::Apply( class WxViewportsContainer* InParent )
{
	for ( uint32 index = 0; index < 4; ++index )
	{
		FVCD_Viewport&			viewport = viewports[ index ];
		if ( !viewport.bEnabled || !viewport.viewportWindow )
		{
			continue;
		}

		viewport.viewportWindow->DestroyChildren();
		viewport.viewportWindow->Destroy();
		viewport.viewportWindow = nullptr;
	}

	for ( uint32 index = 0, count = splitterWindows.size(); index < count; ++index )
	{
		wxSplitterWindow*		splitterWindow = splitterWindows[ index ];
		splitterWindow->DestroyChildren();
		splitterWindow->Destroy();
	}
	splitterWindows.clear();

	wxRect		rc = InParent->GetClientRect();

	// Set up the splitters and viewports as per the template defaults
	wxSplitterWindow*		mainSplitter = nullptr;
	uint32					sashPos = 0;
	std::wstring			key;

	switch ( templateViewport )
	{
	case VC_2_2_Split:
	{
		wxSplitterWindow*	topSplitter		= nullptr;
		wxSplitterWindow*	bottomSplitter	= nullptr;

		mainSplitter		= new wxSplitterWindow( InParent, RID_SplitterWindow, wxDefaultPosition, wxSize( rc.GetWidth(), rc.GetHeight() ), wxSP_3D );
		topSplitter			= new wxSplitterWindow( mainSplitter, RID_SplitterWindow + 1, wxDefaultPosition, wxSize( rc.GetWidth(), rc.GetHeight() / 2 ), wxSP_3D);
		bottomSplitter		= new wxSplitterWindow( mainSplitter, RID_SplitterWindow + 2, wxDefaultPosition, wxSize( rc.GetWidth(), rc.GetHeight() / 2 ), wxSP_3D);
		splitterWindows.push_back( mainSplitter );
		splitterWindows.push_back( topSplitter );
		splitterWindows.push_back( bottomSplitter );
		
		// Set min and max size for spliters
		mainSplitter->SetMinSize( wxSize( -1, -1 ) );
		mainSplitter->SetMaxSize( wxSize( -1, -1 ) );

		topSplitter->SetMinSize( wxSize( -1, -1 ) );
		topSplitter->SetMaxSize( wxSize( -1, -1 ) );

		bottomSplitter->SetMaxSize( wxSize( -1, -1 ) );
		bottomSplitter->SetMinSize( wxSize( -1, -1 ) );

		// Connect splitter events so we can link the top and bottom splitters if the ViewportResizeTogether option is set
		InParent->ConnectSplitterEvents( topSplitter, bottomSplitter );

		for ( uint32 index = 0; index < 4; ++index )
		{
			FVCD_Viewport&		vcdViewport = viewports[ index ];
			if ( !vcdViewport.bEnabled )
			{
				continue;
			}
		
			vcdViewport.viewportWindow = new WxLevelViewportWindow();
			vcdViewport.viewportWindow->Create( mainSplitter, -1 );
			vcdViewport.viewportWindow->SetUp( vcdViewport.viewportType, vcdViewport.bSetListenerPosition, vcdViewport.showFlags );
			vcdViewport.viewportWindow->SetLabel( wxT( "LevelViewport" ) );
		}
		
		viewports[ 0 ].viewportWindow->Reparent( topSplitter );
		viewports[ 1 ].viewportWindow->Reparent( topSplitter );
		viewports[ 2 ].viewportWindow->Reparent( bottomSplitter );
		viewports[ 3 ].viewportWindow->Reparent( bottomSplitter );

		mainSplitter->SetLabel( wxT( "MainSplitter" ) );
		topSplitter->SetLabel( wxT( "TopSplitter" ) );
		bottomSplitter->SetLabel( wxT( "BottomSplitter" ) );

		mainSplitter->SplitHorizontally( topSplitter, bottomSplitter, 0 );
		topSplitter->SplitVertically( viewports[ 0 ].viewportWindow, viewports[ 1 ].viewportWindow, 0 );
		bottomSplitter->SplitVertically( viewports[ 2 ].viewportWindow, viewports[ 3 ].viewportWindow, 0 );
	}
	break;

	case VC_1_2_Split:
	{
		wxSplitterWindow*		rightSplitter = nullptr;

		mainSplitter	= new wxSplitterWindow( InParent, RID_SplitterWindow, wxDefaultPosition, wxSize( rc.GetWidth(), rc.GetHeight() ), wxSP_3D );
		rightSplitter	= new wxSplitterWindow( mainSplitter, RID_SplitterWindow + 1,wxDefaultPosition, wxSize( rc.GetWidth(), rc.GetHeight() / 2 ), wxSP_3D );
		splitterWindows.push_back( mainSplitter );
		splitterWindows.push_back( rightSplitter );

		// Set min and max size for spliters
		mainSplitter->SetMinSize( wxSize( -1, -1 ) );
		mainSplitter->SetMaxSize( wxSize( -1, -1 ) );

		rightSplitter->SetMinSize( wxSize( -1, -1 ) );
		rightSplitter->SetMaxSize( wxSize( -1, -1 ) );

		// Disconnect splitter Events
		InParent->DisconnectSplitterEvents();

		for ( uint32 index = 0; index < 4; ++index )
		{
			FVCD_Viewport&		vcdViewport = viewports[ index ];
			if ( !vcdViewport.bEnabled )
			{
				continue;
			}

			vcdViewport.viewportWindow = new WxLevelViewportWindow();
			vcdViewport.viewportWindow->Create( mainSplitter, -1 );
			vcdViewport.viewportWindow->SetUp( vcdViewport.viewportType, vcdViewport.bSetListenerPosition, vcdViewport.showFlags );
			vcdViewport.viewportWindow->SetLabel( wxT( "LevelViewport" ) );
		}

		viewports[ 0 ].viewportWindow->Reparent( mainSplitter );
		viewports[ 1 ].viewportWindow->Reparent( rightSplitter );
		viewports[ 2 ].viewportWindow->Reparent( rightSplitter );

		mainSplitter->SetLabel( wxT( "MainSplitter" ) );
		rightSplitter->SetLabel( wxT( "RightSplitter" ) );

		mainSplitter->SplitVertically( viewports[ 0 ].viewportWindow, rightSplitter, 0 );
		rightSplitter->SplitHorizontally( viewports[ 1 ].viewportWindow, viewports[ 2 ].viewportWindow, 0 );
	}
	break;

	case VC_1_1_SplitH:
	case VC_1_1_SplitV:
	{
		mainSplitter	= new wxSplitterWindow( InParent, RID_SplitterWindow, wxDefaultPosition, wxSize( rc.GetWidth(),rc.GetHeight() ), wxSP_3D );
		splitterWindows.push_back( mainSplitter );

		// Set min and max size for spliters
		mainSplitter->SetMinSize( wxSize( -1, -1 ) );
		mainSplitter->SetMaxSize( wxSize( -1, -1 ) );

		// Disconnect Splitter Events
		InParent->DisconnectSplitterEvents();

		for ( uint32 index = 0; index < 4; ++index )
		{
			FVCD_Viewport&		vcdViewport = viewports[ index ];
			if ( !vcdViewport.bEnabled )
			{
				continue;
			}

			vcdViewport.viewportWindow = new WxLevelViewportWindow();
			vcdViewport.viewportWindow->Create( mainSplitter, -1 );
			vcdViewport.viewportWindow->SetUp( vcdViewport.viewportType, vcdViewport.bSetListenerPosition, vcdViewport.showFlags );
			vcdViewport.viewportWindow->SetLabel( wxT( "LevelViewport" ) );
		}

		viewports[ 0 ].viewportWindow->Reparent( mainSplitter );
		viewports[ 1 ].viewportWindow->Reparent( mainSplitter );
		
		mainSplitter->SetLabel( wxT( "MainSplitter" ) );
		if ( templateViewport == VC_1_1_SplitH )
		{
			mainSplitter->SplitHorizontally( viewports[ 0 ].viewportWindow, viewports[ 1 ].viewportWindow, 0 );
		}
		else
		{
			mainSplitter->SplitVertically( viewports[ 0 ].viewportWindow, viewports[ 1 ].viewportWindow, 0 );
		}
	}
	break;
	}

	// Make sure the splitters will resize with the editor
	wxBoxSizer*		sizerHorizontal = new wxBoxSizer( wxHORIZONTAL );
	sizerHorizontal->Add( mainSplitter, 1, wxEXPAND | wxALL, 0 );

	sizer = new wxBoxSizer( wxVERTICAL );
	sizer->Add( sizerHorizontal, 1, wxEXPAND | wxALL, 0 );
	InParent->SetSizer( sizer );
}

bool FViewportConfig_Data::LoadFromConfig()
{
	return false;
}

void FViewportConfig_Data::SaveToConfig()
{
	check( false && "Need implement" );
}

void FViewportConfig_Data::ResizeProportionally( float InScaleX, float InScaleY, bool InIsNeedRedraw /* = true */ )
{
	for ( uint32 index = 0, count = splitterWindows.size(); index < count; ++index )
	{
		wxSplitterWindow*		splitter	= splitterWindows[ index ];
		wxSize					windowSize	= splitter->GetSize();
		float					scale		= 0.f;
		float					oldSize		= 0.f;

		if ( splitter->GetSplitMode() == wxSPLIT_HORIZONTAL )
		{
			scale = InScaleY;
			oldSize = windowSize.y;
		}
		else
		{
			scale = InScaleX;
			oldSize = windowSize.x;
		}

		float		newSize		= ( float )oldSize * scale;
		float		proportion	= ( float )splitter->GetSashPosition() / oldSize;
		int			sash		= FMath::Trunc( proportion * newSize );
		splitter->SetSashPosition( sash, InIsNeedRedraw );
	}
}