#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/MaterialPreviewViewportClient.h"
#include "Render/RenderUtils.h"

FMaterialPreviewViewportClient::FMaterialPreviewViewportClient( const TSharedPtr<FMaterial>& InMaterial )
	: FEditorLevelViewportClient( LVT_Perspective )
	, scene( new FScene() )
	, material( InMaterial )
	, sphereComponent( new LSphereComponent() )
{
	// Init view
	bIgnoreInput			= true;
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor;
	viewLocation			= FVector( 0.f, 0.f, -80.f );
	viewRotation			= FRotator( 0.f, 0.f, 0.f );

	// Init scene
	sphereComponent->SetRadius( 40.f );
	sphereComponent->SetMaterial( InMaterial );
	sphereComponent->SetVisibility( true );
	sphereComponent->SetRelativeRotation( FRotator( 90.f, 0.f, 0.f ) );
	scene->AddPrimitive( sphereComponent );
}

FMaterialPreviewViewportClient::~FMaterialPreviewViewportClient()
{
	FlushRenderingCommands();
	delete scene;
}

void FMaterialPreviewViewportClient::Tick( float InDeltaSeconds )
{
	FEditorLevelViewportClient::Tick( InDeltaSeconds );
	sphereComponent->AddRelativeRotate( FRotator( 0.f, 0.f, 10.f * InDeltaSeconds ) );
}

void FMaterialPreviewViewportClient::Draw( FViewport* InViewport )
{
	FSceneView*		sceneView = CalcSceneView( InViewport );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FViewportRenderCommand,
										  FMaterialPreviewViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void FMaterialPreviewViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

void FMaterialPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}