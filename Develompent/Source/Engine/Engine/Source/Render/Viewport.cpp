#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Viewport.h"
#include "Render/SceneRenderTargets.h"
#include "Render/Scene.h"

CViewport::CViewport() 
	: windowHandle( nullptr )
	, viewportClient( nullptr )
	, sizeX( 0 )
	, sizeY( 0 )
{}

CViewport::~CViewport()
{}

void CViewport::InitRHI()
{
	// If viewport already created - we need resize
	if ( viewportRHI )
	{
		viewportRHI->Resize( sizeX, sizeY );
		if ( renderTarget )
		{
			check( renderTarget->GetSurfaceRHI() );
			viewportRHI->SetSurface( renderTarget->GetSurfaceRHI() );
		}
	}
	// Else we need create it
	else
	{
		if ( windowHandle )
		{
			viewportRHI = GRHI->CreateViewport( windowHandle, sizeX, sizeY );
		}
		else if ( renderTarget )
		{
			check( renderTarget->GetSurfaceRHI() );
			viewportRHI = GRHI->CreateViewport( renderTarget->GetSurfaceRHI(), sizeX, sizeY );
		}
	}
}

void CViewport::UpdateRHI()
{
	InitRHI();
}

void CViewport::ReleaseRHI()
{
	// Release RHI if him is not have parent
	viewportRHI.SafeRelease();
}

void CViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, CRenderTarget* InNewRenderTarget )
{
	bool			bNewRenderTarget = renderTarget != InNewRenderTarget;

	// Update the viewport attributes
	windowHandle	= nullptr;
	renderTarget	= InNewRenderTarget;

	UpdateInternal( InIsDestroyed, InNewSizeX, InNewSizeY, bNewRenderTarget );
}

void CViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, WindowHandle_t InNewWindowHandle )
{
	bool		bNewWindowHandle = windowHandle != InNewWindowHandle;

	// Update the viewport attributes
	renderTarget.SafeRelease();
	windowHandle = InNewWindowHandle;

	UpdateInternal( InIsDestroyed, InNewSizeX, InNewSizeY, bNewWindowHandle );
}

void CViewport::UpdateInternal( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, bool InIsNewHandle )
{
	checkMsg( InIsDestroyed || ( windowHandle || renderTarget ) && !( windowHandle && renderTarget ), TEXT( "Must be stated only one" ) );
	bool		bNeedUpdateViewportRHI = InIsNewHandle || sizeX != InNewSizeX || sizeY != InNewSizeY;

	// Update the viewport attributes
	sizeX = InNewSizeX;
	sizeY = InNewSizeY;

	// Don't reinitialize the viewport RHI if the viewport has been destroyed
	if ( InIsDestroyed )
	{
		BeginReleaseResource( this );
		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Update the viewport's resources if it need
	if ( bNeedUpdateViewportRHI )
	{
		BeginUpdateResource( this );
	}
}

void CViewport::Tick( float InDeltaSeconds )
{
	if ( viewportClient )
	{
		viewportClient->Tick( InDeltaSeconds );
	}
}

void CViewport::Draw( bool InIsShouldPresent /* = true */ )
{
	if ( !IsInitialized() || !IsValid() )
	{
		return;
	}

	// Begin drawing viewport
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CBeginRenderCommand,
										ViewportRHIRef_t, viewportRHI, viewportRHI,
										{
											CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
											GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
										} );

	// Draw viewport
	if ( viewportClient )
	{
		viewportClient->Draw( this );
	}

	// End drawing viewport
	UNIQUE_RENDER_COMMAND_TWOPARAMETER( CEndRenderCommand,
										ViewportRHIRef_t, viewportRHI, viewportRHI,
										bool, isShouldPresent, InIsShouldPresent,
										{
											CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
											GRHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
										} );
}