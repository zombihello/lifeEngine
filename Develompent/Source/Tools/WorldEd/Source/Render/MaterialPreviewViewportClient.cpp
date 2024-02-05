/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/Render/RenderUtils.h"
#include "Render/MaterialPreviewViewportClient.h"

/*
==================
CMaterialPreviewViewportClient::CMaterialPreviewViewportClient
==================
*/
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
	viewRotationEuler		= Math::vectorZero;
	viewRotationQuat		= Math::quaternionZero;

	// Init scene
	sphereComponent->SetRadius( 40.f );
	sphereComponent->SetMaterial( InMaterial->GetAssetHandle() );
	sphereComponent->SetVisibility( true );
	sphereComponent->SetRelativeRotation( CRotator( 90.f, 0.f, 0.f ) );
	scene->AddPrimitive( sphereComponent );

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