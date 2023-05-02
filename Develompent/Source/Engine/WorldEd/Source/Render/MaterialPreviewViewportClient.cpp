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
	, pointLightComponent( new CPointLightComponent() )
{
	// Init view
	bIgnoreInput			= true;
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor;
	viewLocation			= Vector( 0.f, 0.f, -80.f );
	viewRotationEuler		= SMath::vectorZero;
	viewRotationQuat		= SMath::quaternionZero;

	// Init scene
	sphereComponent->SetRadius( 40.f );
	sphereComponent->SetMaterial( InMaterial->GetAssetHandle() );
	sphereComponent->SetVisibility( true );
	sphereComponent->SetRelativeRotation( SMath::AnglesToQuaternionXYZ( Vector( 90.f, 0.f, 0.f ) ) );
	scene->AddPrimitive( sphereComponent );

	pointLightComponent->SetRelativeLocation( Vector( 0.f, 20.f, -80.f ) );
	scene->AddLight( pointLightComponent );
}

CMaterialPreviewViewportClient::~CMaterialPreviewViewportClient()
{
	FlushRenderingCommands();
	delete scene;
}

void CMaterialPreviewViewportClient::Tick( float InDeltaSeconds )
{
	CEditorLevelViewportClient::Tick( InDeltaSeconds );
	sphereComponent->AddRelativeRotate( SMath::AnglesToQuaternionXYZ( Vector( 0.f, 10.f * InDeltaSeconds, 0.f ) ) );
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