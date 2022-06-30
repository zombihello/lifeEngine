#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/MaterialPreviewViewportClient.h"
#include "Render/RenderUtils.h"

CMaterialPreviewViewportClient::CMaterialPreviewViewportClient( const TSharedPtr<CMaterial>& InMaterial )
	: CEditorLevelViewportClient( LVT_Perspective )
	, scene( new CScene() )
	, material( InMaterial )
	, sphereComponent( new CSphereComponent() )
{
	// Init view
	bIgnoreInput			= true;
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor;
	viewLocation			= Vector( 0.f, 0.f, -80.f );
	viewRotation			= CRotator( 0.f, 0.f, 0.f );

	// Init scene
	sphereComponent->SetRadius( 40.f );
	sphereComponent->SetMaterial( InMaterial->GetAssetHandle() );
	sphereComponent->SetVisibility( true );
	sphereComponent->SetRelativeRotation( CRotator( 90.f, 0.f, 0.f ) );
	scene->AddPrimitive( sphereComponent );
}

CMaterialPreviewViewportClient::~CMaterialPreviewViewportClient()
{
	FlushRenderingCommands();
	delete scene;
}

void CMaterialPreviewViewportClient::Tick( float InDeltaSeconds )
{
	CEditorLevelViewportClient::Tick( InDeltaSeconds );
	sphereComponent->AddRelativeRotate( CRotator( 0.f, 0.f, 10.f * InDeltaSeconds ) );
}

void CMaterialPreviewViewportClient::Draw( CViewport* InViewport )
{
	check( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CMaterialPreviewViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void CMaterialPreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

void CMaterialPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}