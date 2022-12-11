#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"
#include "Widgets/ViewportWidget.h"

CViewportWidget::CViewportWidget( bool InIsEnabled /* = true */, CViewportClient* InViewportClient /* = nullptr */, bool InDeleteViewportClient /* = false */ )
	: bInit( false )
	, bEnabled( InIsEnabled )
	, bDeleteViewportClient( InDeleteViewportClient )
	, bViewportOnDrawing( false )
	, viewport( new CViewport() )
	, renderTarget( new CRenderTarget() )
	, viewportClient( InViewportClient )
	, size( 200, 200 )
{
	viewport->SetViewportClient( InViewportClient );
	SetEnabled( bEnabled );
	SetSize( size );
}

void CViewportWidget::Init()
{
	bInit = true;
	if ( IsEnabled() )
	{
		GEditorEngine->AddViewport( viewport );
		bViewportOnDrawing = true;
	}
}

void CViewportWidget::Tick()
{
	check( bInit );
	if ( !renderTarget->IsDirty() && renderTarget->IsValid() )		// FIXME: Need fix flickering when widget is resizing
	{
		ImGui::Image( renderTarget->GetTexture2DRHI()->GetHandle(), ImVec2{ ( float )size.x, ( float )size.y } );
	}
}

void CViewportWidget::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Process ImGUI events
	if ( InWindowEvent.bImGUIEvent )
	{
		switch ( InWindowEvent.type )
		{
		case SWindowEvent::T_WindowResize:
			SetSize( Vector2D( InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height ) );
			break;
		}
	}
}

void CViewportWidget::SetEnabled( bool InIsEnabled )
{
	bEnabled = InIsEnabled;
	if ( IsInit() )
	{
		if ( InIsEnabled && !bViewportOnDrawing )
		{
			GEditorEngine->AddViewport( viewport );
			bViewportOnDrawing = true;
		}
		else if ( !InIsEnabled && bViewportOnDrawing )
		{
			GEditorEngine->RemoveViewport( viewport );
			bViewportOnDrawing = false;
		}
	}
}