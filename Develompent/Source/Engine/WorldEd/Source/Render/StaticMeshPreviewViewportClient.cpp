#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/StaticMeshPreviewViewportClient.h"
#include "Render/RenderUtils.h"

/*
==================
CStaticMeshPreviewViewportClient::CStaticMeshPreviewViewportClient
==================
*/
CStaticMeshPreviewViewportClient::CStaticMeshPreviewViewportClient( const TSharedPtr<CStaticMesh>& InStaticMesh )
	: CEditorLevelViewportClient( LVT_Perspective )
	, scene( new CScene() )
	, staticMesh( InStaticMesh )
	, staticMeshComponent( nullptr )
{
	// Init view
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor & ~SHOW_Lights;
	viewLocation			= Vector( 0.f, 0.f, -80.f );
	viewRotationEuler		= Math::vectorZero;
	viewRotationQuat		= Math::quaternionZero;

	// Init scene
	staticMeshComponent = new( nullptr, NAME_None ) CStaticMeshComponent();
	staticMeshComponent->AddToRoot();
	staticMeshComponent->SetStaticMesh( InStaticMesh->GetAssetHandle() );
	staticMeshComponent->SetVisibility( true );
	scene->AddPrimitive( staticMeshComponent );
}

/*
==================
CStaticMeshPreviewViewportClient::~CStaticMeshPreviewViewportClient
==================
*/
CStaticMeshPreviewViewportClient::~CStaticMeshPreviewViewportClient()
{
	FlushRenderingCommands();
	staticMeshComponent->RemoveFromRoot();
	delete scene;
}

/*
==================
CStaticMeshPreviewViewportClient::Tick
==================
*/
void CStaticMeshPreviewViewportClient::Tick( float InDeltaSeconds )
{
	CEditorLevelViewportClient::Tick( InDeltaSeconds );
}

/*
==================
CStaticMeshPreviewViewportClient::Draw
==================
*/
void CStaticMeshPreviewViewportClient::Draw( CViewport* InViewport )
{
	Assert( InViewport );
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

/*
==================
CStaticMeshPreviewViewportClient::Draw_RenderThread
==================
*/
void CStaticMeshPreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
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

/*
==================
CStaticMeshPreviewViewportClient::SetViewportType
==================
*/
void CStaticMeshPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}