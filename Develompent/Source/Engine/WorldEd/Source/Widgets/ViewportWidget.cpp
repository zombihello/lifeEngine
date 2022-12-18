#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "System/EditorEngine.h"
#include "Widgets/ViewportWidget.h"
#include "ImGUI/ImGUIEngine.h"

CViewportWidget::CViewportWidget( bool InIsEnabled /* = true */, CViewportClient* InViewportClient /* = nullptr */, bool InDeleteViewportClient /* = false */ )
	: bInit( false )
	, bEnabled( InIsEnabled )
	, bDeleteViewportClient( InDeleteViewportClient )
	, bViewportOnDrawing( false )
	, bHovered( false )
	, viewport( new CViewport() )
	, renderTarget( new CRenderTarget() )
	, viewportClient( InViewportClient )
	, size( 200, 200 )
{
	viewport->SetViewportClient( InViewportClient );
	SetEnabled( bEnabled );
	SetSize( size );
}

CViewportWidget::~CViewportWidget()
{
	SetEnabled( false );
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
	if ( renderTarget->IsValid() )
	{
		ImGui::Image( GImGUIEngine->LockTexture( renderTarget->GetTexture2DRHI() ), ImVec2{ ( float )size.x, ( float )size.y } );
		bHovered = ImGui::IsItemHovered();
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