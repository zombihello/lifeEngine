#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Viewport.h"

FViewport::FViewport() :
	windowHandle( nullptr ),
	sizeX( 0 ),
	sizeY( 0 )
{}

FViewport::~FViewport()
{}

void FViewport::InitRHI()
{
	// If viewport already created - we need resize
	if ( viewportRHI )
	{
		viewportRHI->Resize( sizeX, sizeY );
	}
	// Else we need create it
	else
	{
		viewportRHI = GRHI->CreateViewport( windowHandle, sizeX, sizeY );
	}
}

void FViewport::ReleaseRHI()
{
	viewportRHI.SafeRelease();
}

void FViewport::Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, void* InNewWindowHandle )
{
	bool	isNewWindowHandle = windowHandle != InNewWindowHandle;

	// Update the viewport attributes
	windowHandle = InNewWindowHandle;
	sizeX = InNewSizeX;
	sizeY = InNewSizeY;

#if DO_CHECK
	if ( !InIsDestroyed )
	{
		check( windowHandle );
	}
#endif // DO_CHECK

	// Don't reinitialize the viewport RHI if the viewport has been destroyed
	if ( InIsDestroyed || isNewWindowHandle )
	{	
		BeginReleaseResource( this );

		if ( InIsDestroyed )
		{
			return;
		}
	}

	// Initialize the viewport's resources
	BeginInitResource( this );
}

void FViewport::Draw( bool InIsShouldPresent /* = true */ )
{
	// BS yehor.pohuliaka - Place court render scene
	if ( !isInitialized )
	{
		return;
	}

	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FBeginRenderCommand,
										FViewportRHIRef, viewportRHI, viewportRHI,
										{
											FBaseDeviceContextRHI* immediateContext = GRHI->GetImmediateContext();
											GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
											immediateContext->ClearSurface( viewportRHI->GetSurface(), FColor::black );
										} );

	UNIQUE_RENDER_COMMAND_TWOPARAMETER( FEndRenderCommand,
										bool, isShouldPresent, InIsShouldPresent,
										FViewportRHIRef, viewportRHI, viewportRHI,
										{
											FBaseDeviceContextRHI* immediateContext = GRHI->GetImmediateContext();
											GRHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
										} );
}