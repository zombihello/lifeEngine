#include "Misc/WorldEdGlobals.h"
#include "Windows/ViewportWindow.h"
#include "System/EditorEngine.h"

#include "Render/RenderingThread.h"

CViewportWindow::CViewportWindow( const std::wstring& InName, ELevelViewportType InViewportType /* = LVT_Perspective */ )
	: CImGUILayer( InName )
	, viewport( new CViewport() )
	, renderTarget( new CRenderTarget() )
{
	viewportClient.SetViewportType( InViewportType );
	viewport->SetViewportClient( &viewportClient );
}

void CViewportWindow::Init()
{
	CImGUILayer::Init();
	GEditorEngine->AddViewport( viewport );
}

void CViewportWindow::OnTick()
{
	if (  renderTarget->IsValid() )
	{
		ImGui::Image( renderTarget->GetTexture2DRHI()->GetHandle(), ImVec2( GetSizeX(), GetSizeY() ) );
	}
}

void CViewportWindow::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Process ImGUI events
	if ( InWindowEvent.bImGUIEvent )
	{
		switch ( InWindowEvent.type )
		{
		case SWindowEvent::T_WindowResize:
			renderTarget->Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, PF_A8R8G8B8, 0, CString::Format( TEXT( "%s_ViewportRT" ), GetName().c_str() ).c_str() );
			viewport->Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, renderTarget );
			break;
		}
	}

	// Process event in viewport client
	viewportClient.ProcessEvent( InWindowEvent );
}