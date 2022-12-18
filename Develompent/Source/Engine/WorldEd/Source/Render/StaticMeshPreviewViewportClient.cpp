#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Render/RenderUtils.h"

CStaticMeshPreviewViewportClient::CStaticMeshPreviewViewportClient( const TSharedPtr<CStaticMesh>& InStaticMesh )
	: CEditorLevelViewportClient( LVT_Perspective )
	, scene( new CScene() )
	, staticMesh( InStaticMesh )
	, staticMeshComponent( new CStaticMeshComponent() )
{
	// Init view
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor & ~SHOW_Lights;
	viewLocation			= Vector( 0.f, 0.f, -80.f );
	viewRotationEuler		= SMath::vectorZero;
	viewRotationQuat		= SMath::quaternionZero;

	// Init scene
	staticMeshComponent->SetStaticMesh( InStaticMesh->GetAssetHandle() );
	staticMeshComponent->SetVisibility( true );
	scene->AddPrimitive( staticMeshComponent );
}

CStaticMeshPreviewViewportClient::~CStaticMeshPreviewViewportClient()
{
	FlushRenderingCommands();
	delete scene;
}

void CStaticMeshPreviewViewportClient::Tick( float InDeltaSeconds )
{
	CEditorLevelViewportClient::Tick( InDeltaSeconds );
}

void CStaticMeshPreviewViewportClient::Draw( CViewport* InViewport )
{
	check( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CStaticMeshPreviewViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void CStaticMeshPreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );

	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType, scene );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

void CStaticMeshPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}