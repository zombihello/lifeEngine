#include "Misc/WorldEdGlobals.h"
#include "Windows/ViewportWindow.h"
#include "System/EditorEngine.h"

#include "Render/RenderingThread.h"

CViewportWindow::CViewportWindow( const std::wstring& InName, bool InVisibility /* = true */, ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: CImGUILayer( InName )
	, viewportWidget( true, &viewportClient )
{
	viewportClient.SetViewportType( InViewportType );
	SetVisibility( InVisibility );
	SetPadding( Vector2D( 0.f, 0.f ) );
}

void CViewportWindow::Init()
{
	CImGUILayer::Init();
	viewportWidget.Init();
}

void CViewportWindow::OnTick()
{
	viewportWidget.Tick();
}

void CViewportWindow::OnVisibilityChanged( bool InNewVisibility )
{
	viewportWidget.SetEnabled( InNewVisibility );
}

void CViewportWindow::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Process ImGUI events
	viewportWidget.ProcessEvent( InWindowEvent );

	// Process event in viewport client
	viewportClient.ProcessEvent( InWindowEvent );
}