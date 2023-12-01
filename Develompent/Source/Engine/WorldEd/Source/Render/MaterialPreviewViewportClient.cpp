#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Render/RenderingThread.h"
#include "Render/MaterialPreviewViewportClient.h"
#include "Render/RenderUtils.h"

/*
==================
CMaterialPreviewViewportClient::CMaterialPreviewViewportClient
==================
*/
CMaterialPreviewViewportClient::CMaterialPreviewViewportClient( const TSharedPtr<CMaterial>& InMaterial )
	: CEditorLevelViewportClient( LVT_Perspective )
	, scene( new CScene() )
	, material( InMaterial )
	, sphereComponent( nullptr )
	, pointLightComponent( nullptr )
{
	// Init view
	bIgnoreInput			= true;
	bSetListenerPosition	= false;
	viewportType			= LVT_Perspective;
	showFlags				= SHOW_DefaultEditor;
	viewLocation			= Vector( 0.f, 0.f, -80.f );
	viewRotationEuler		= Math::vectorZero;
	viewRotationQuat		= Math::quaternionZero;

	// Init scene
	sphereComponent = new( nullptr, NAME_None ) CSphereComponent();
	sphereComponent->AddToRoot();
	sphereComponent->SetRadius( 40.f );
	sphereComponent->SetMaterial( InMaterial->GetAssetHandle() );
	sphereComponent->SetVisibility( true );
	sphereComponent->SetRelativeRotation( CRotator( 90.f, 0.f, 0.f ) );
	scene->AddPrimitive( sphereComponent );

	pointLightComponent = new( nullptr, NAME_None ) CPointLightComponent();
	pointLightComponent->AddToRoot();
	pointLightComponent->SetRelativeLocation( Vector( 0.f, 20.f, -80.f ) );
	scene->AddLight( pointLightComponent );
}

/*
==================
CMaterialPreviewViewportClient::~CMaterialPreviewViewportClient
==================
*/
CMaterialPreviewViewportClient::~CMaterialPreviewViewportClient()
{
	FlushRenderingCommands();
	sphereComponent->RemoveFromRoot();
	pointLightComponent->RemoveFromRoot();
	delete scene;
}

/*
==================
CMaterialPreviewViewportClient::Tick
==================
*/
void CMaterialPreviewViewportClient::Tick( float InDeltaSeconds )
{
	CEditorLevelViewportClient::Tick( InDeltaSeconds );
	sphereComponent->AddRelativeRotation( CRotator( 0.f, 10.f * InDeltaSeconds, 0.f )  );
}

/*
==================
CMaterialPreviewViewportClient::Draw
==================
*/
void CMaterialPreviewViewportClient::Draw( CViewport* InViewport )
{
	Assert( InViewport );
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

/*
==================
CMaterialPreviewViewportClient::Draw_RenderThread
==================
*/
void CMaterialPreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, scene );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );

	// Draw scene
	sceneRenderer.Render( InViewportRHI );

	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

/*
==================
CMaterialPreviewViewportClient::SetViewportType
==================
*/
void CMaterialPreviewViewportClient::SetViewportType( ELevelViewportType InViewportType )
{}