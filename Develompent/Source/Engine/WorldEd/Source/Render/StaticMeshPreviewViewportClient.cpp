#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Render/RenderUtils.h"

FStaticMeshPreviewViewportClient::FStaticMeshPreviewViewportClient( const TSharedPtr<FStaticMesh>& InStaticMesh )
	: FEditorLevelViewportClient( LVT_Perspective )
	, scene( new FScene() )
	, staticMesh( InStaticMesh )
	, staticMeshComponent( new LStaticMeshComponent() )
{
	// Init view
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor;
	viewLocation			= FVector( 0.f, 0.f, -80.f );
	viewRotation			= FRotator( 0.f, 0.f, 0.f );

	// Init scene
	staticMeshComponent->SetStaticMesh( InStaticMesh->GetAssetHandle() );
	staticMeshComponent->SetVisibility( true );
	scene->AddPrimitive( staticMeshComponent );
}

FStaticMeshPreviewViewportClient::~FStaticMeshPreviewViewportClient()
{
	FlushRenderingCommands();
	delete scene;
}

void FStaticMeshPreviewViewportClient::Tick( float InDeltaSeconds )
{
	FEditorLevelViewportClient::Tick( InDeltaSeconds );
}

void FStaticMeshPreviewViewportClient::Draw( FViewport* InViewport )
{
	FSceneView*		sceneView = CalcSceneView( InViewport );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FViewportRenderCommand,
										  FStaticMeshPreviewViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void FStaticMeshPreviewViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );

	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType, scene );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

void FStaticMeshPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}