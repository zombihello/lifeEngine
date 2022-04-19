#include "Misc/EngineGlobals.h"
#include "Misc/AudioGlobals.h"
#include "Misc/UIGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/RenderingThread.h"
#include "Render/Viewport.h"
#include "Render/SceneRenderTargets.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "System/CameraManager.h"
#include "System/AudioDevice.h"
#include "UIEngine.h"

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

	GSceneRenderTargets.Allocate( sizeX, sizeY );
}

void FViewport::UpdateRHI()
{
	InitRHI();
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

	// Initialize the viewport's resources if viewport RHI not created
	if ( !viewportRHI )
	{
		BeginInitResource( this );
	}
	// Else update resource
	else
	{
		BeginUpdateResource( this );
	}
}
#include "Logger/LoggerMacros.h"
void FViewport::Draw( bool InIsShouldPresent /* = true */ )
{
	if ( !IsInitialized() )
	{
		return;
	}

	TRefCountPtr< LCameraComponent >        cameraComponent = GCameraManager->GetActiveCamera();
	FSceneView*                             sceneView = new FSceneView();
	if ( cameraComponent )
	{
		sceneView->SetCameraView( cameraComponent );
	}

	// Update audio listener spatial
	{
		const FCameraView&		cameraView = sceneView->GetCameraView();
		GAudioDevice.SetListenerSpatial( cameraView.location, cameraView.rotation.RotateVector( FMath::vectorForward ), cameraView.rotation.RotateVector( FMath::vectorUp ) );
	}

	struct Helper
	{
		static void Execute( FViewportRHIRef viewportRHI, FSceneView* sceneView, bool isShouldPresent )
		{
			FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
			FSceneRenderer				sceneRenderer( sceneView );
			GRHI->BeginDrawingViewport( immediateContext, viewportRHI );
			
			// Scene render
			sceneRenderer.Render( viewportRHI );
			
			// UI render
			GUIEngine->BeginDraw();
			GUIEngine->EndDraw();

			GRHI->EndDrawingViewport( immediateContext, viewportRHI, isShouldPresent, false );
			
			// Trigger event of end rendering frame
			if ( GRenderFrameFinished )
			{
				GRenderFrameFinished->Trigger();
			}

			// Delete scene view
			delete sceneView;
		}
	};

	// Wait while render thread is rendering of the frame
	WaitWhileRenderingFrame();

	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FRenderFrameCommand,
										  FViewportRHIRef, viewportRHI, viewportRHI,
										  FSceneView*, sceneView, sceneView,
										  bool, isShouldPresent, InIsShouldPresent,
										  {
											  Helper::Execute( viewportRHI, sceneView, isShouldPresent );
										  } );
}
