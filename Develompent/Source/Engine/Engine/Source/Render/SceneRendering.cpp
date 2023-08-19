#include "Containers/String.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "System/BaseEngine.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/TypesRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Render/RenderUtils.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/Texture.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/SceneRenderTargets.h"

/*
==================
CSceneRenderer::CSceneRenderer
==================
*/
CSceneRenderer::CSceneRenderer( CSceneView* InSceneView, class CScene* InScene /* = nullptr */ )
	: scene( InScene )
	, sceneView( InSceneView )
{}

/*
==================
CSceneRenderer::BeginRenderViewTarget
==================
*/
void CSceneRenderer::BeginRenderViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	CBaseDeviceContextRHI*	immediateContext	= g_RHI->GetImmediateContext();
	
	Assert( !viewportRHI );
	viewportRHI				= InViewportRHI;

	SCOPED_DRAW_EVENT( EventBeginRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Begin Render View Target" ) );
	g_SceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	g_RHI->SetViewParameters( immediateContext, *sceneView );
	immediateContext->ClearSurface( g_SceneRenderTargets.GetSceneColorLDRSurface(), sceneView->GetBackgroundColor() );

	// Build visible view on scene
	if ( scene )
	{
		scene->BuildView( *sceneView );
	}
}

/*
==================
CSceneRenderer::Render
==================
*/
void CSceneRenderer::Render( ViewportRHIParamRef_t InViewportRHI )
{
	if ( !scene )
	{
		return;
	}

	CBaseDeviceContextRHI*	immediateContext	= g_RHI->GetImmediateContext();
	ShowFlags_t				showFlags			= sceneView->GetShowFlags();
	bool					bDirty				= false;

	// Render PrePass
	bDirty |= RenderPrePass( immediateContext );

	// Render BasePass
	bDirty |= RenderBasePass( immediateContext );
	
	// Render lights
	bool		bVisibleLights = !scene->GetVisibleLights().empty();
	if ( bDirty && bVisibleLights && showFlags & SHOW_Lights
#if WITH_EDITOR
		 && !( showFlags & SHOW_Wireframe )
#endif // WITH_EDITOR
		 )
	{
		RenderLights( immediateContext );
	}

	// Render post process
	if ( bDirty && showFlags & SHOW_PostProcess
#if WITH_EDITOR
		 && !( showFlags & SHOW_Wireframe )
#endif // WITH_EDITOR
		 )
	{
		RenderPostProcess( immediateContext, bVisibleLights );
	}

#if WITH_EDITOR
	// If we in editor draw WorldEd foreground layer (gizmo and etc)
	if ( g_IsEditor )
	{
		RenderWorldEdForeground( immediateContext );
	}
#endif // WITH_EDITOR

	// Render UI
	RenderUI( immediateContext );
}

#if WITH_EDITOR
/*
==================
CSceneRenderer::RenderWorldEdForeground
==================
*/
void CSceneRenderer::RenderWorldEdForeground( class CBaseDeviceContextRHI* InDeviceContext )
{
	SCOPED_DRAW_EVENT( EventUI, DEC_SCENE_ITEMS, TEXT( "WorldEdForeground" ) );
	
	g_SceneRenderTargets.BeginRenderingSceneColorLDR( InDeviceContext );
	g_RHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<true>::GetRHI() );
	g_RHI->SetBlendState( InDeviceContext, TStaticBlendStateRHI<>::GetRHI() );	
	RenderSDG( InDeviceContext, SDG_WorldEdForeground );
	g_SceneRenderTargets.FinishRenderingSceneColorLDR( InDeviceContext );
}
#endif // WITH_EDITOR

/*
==================
CSceneRenderer::RenderPrePass
==================
*/
bool CSceneRenderer::RenderPrePass( class CBaseDeviceContextRHI* InDeviceContext )
{
	SSceneDepthGroup&		SDG = scene->GetSDG( SDG_World );
	CBaseDeviceContextRHI*	immediateContext = g_RHI->GetImmediateContext();

	SCOPED_DRAW_EVENT( EventPrePass, DEC_SCENE_ITEMS, TEXT( "PrePass" ) );
	g_SceneRenderTargets.BeginRenderingPrePass( immediateContext );
	immediateContext->ClearDepthStencil( g_SceneRenderTargets.GetSceneDepthZSurface() );

	if ( g_Engine->IsPrePass() && SDG.depthDrawList.GetNum() > 0 )
	{
		g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );
		g_RHI->SetBlendState( immediateContext, TStaticBlendStateRHI<>::GetRHI() );
		SDG.depthDrawList.Draw( InDeviceContext, *sceneView );
	}

	g_SceneRenderTargets.FinishRenderingPrePass( immediateContext );
	return true;
}

/*
==================
CSceneRenderer::RenderBasePass
==================
*/
bool CSceneRenderer::RenderBasePass( class CBaseDeviceContextRHI* InDeviceContext )
{
	// Do nothing if SDG_World layer is empty
	if ( scene->GetSDG( SDG_World ).IsEmpty()
#if WITH_EDITOR
		 && scene->GetSDG( SDG_WorldEdBackground ).IsEmpty()
#endif // WITH_EDITOR
		 )
	{
		return false;
	}

	bool						bDirty = false;
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	SCOPED_DRAW_EVENT( EventBasePass, DEC_SCENE_ITEMS, TEXT( "BasePass" ) );

	// Init render states
	g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );
	g_RHI->SetBlendState( immediateContext, TStaticBlendStateRHI<>::GetRHI() );

	// If SHOW_Lights setted and disabled wireframe mode, we rendering to the GBuffer
	ShowFlags_t		showFlags	= sceneView->GetShowFlags();
	bool			bGBuffer	= false;
	if ( showFlags & SHOW_Lights
#if WITH_EDITOR
		 && !( showFlags & SHOW_Wireframe )
#endif // WITH_EDITOR
		 )
	{
		g_SceneRenderTargets.BeginRenderingGBuffer( immediateContext );
		g_SceneRenderTargets.ClearGBufferTargets( immediateContext );
		bGBuffer = true;
	}
	// Otherwise to the scene color
	else
	{
		g_SceneRenderTargets.BeginRenderingSceneColorLDR( immediateContext );
		bGBuffer = false;
	}

	// Render SDG_World layer
	bDirty = RenderSDG( InDeviceContext, SDG_World );

	// Render WorldEd background
#if WITH_EDITOR
	if ( g_IsEditor )
	{
		g_SceneRenderTargets.BeginRenderingSceneColorLDR( immediateContext );
		g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<false, CF_Less>::GetRHI() );
		bDirty = RenderSDG( immediateContext, SDG_WorldEdBackground );
		g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );
		g_SceneRenderTargets.FinishRenderingSceneColorLDR( immediateContext );
	}
#endif // WITH_EDITOR

	// Finish rendering to GBuffer (only when we used him)
	if ( bGBuffer )
	{
		g_SceneRenderTargets.FinishRenderingGBuffer( InDeviceContext );
	}
	else
	{
		g_SceneRenderTargets.FinishRenderingSceneColorLDR( InDeviceContext );
	}
	return bDirty;
}

/*
==================
CSceneRenderer::RenderSDG
==================
*/
bool CSceneRenderer::RenderSDG( class CBaseDeviceContextRHI* InDeviceContext, uint32 InSDGIndex )
{
	ShowFlags_t			showFlags	= sceneView->GetShowFlags();
	SSceneDepthGroup&	SDG			= scene->GetSDG( ( ESceneDepthGroup )InSDGIndex );
	
	if ( SDG.IsEmpty() )
	{
		return false;
	}

	SCOPED_DRAW_EVENT( EventSDG, DEC_SCENE_ITEMS, CString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup )InSDGIndex ) ).c_str() );

#if WITH_EDITOR
	// Draw simple elements
	if ( showFlags & SHOW_SimpleElements && !SDG.simpleElements.IsEmpty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Simple elements" ) );
		SDG.simpleElements.Draw( InDeviceContext, *sceneView );
	}

	// Draw gizmos
	if ( showFlags & SHOW_Gizmo && SDG.gizmoDrawList.GetNum() > 0 )
	{
		SCOPED_DRAW_EVENT( EventGizmos, DEC_SPRITE, TEXT( "Gizmos" ) );
		SDG.gizmoDrawList.Draw( InDeviceContext, *sceneView );
	}
#endif // WITH_EDITOR

	// Draw static meshes
	if ( showFlags & SHOW_StaticMesh && SDG.staticMeshDrawList.GetNum() > 0 )
	{
		SCOPED_DRAW_EVENT( EventStaticMeshes, DEC_STATIC_MESH, TEXT( "Static meshes" ) );
		SDG.staticMeshDrawList.Draw( InDeviceContext, *sceneView );
	}

	// Draw sprites
	if ( showFlags & SHOW_Sprite && SDG.spriteDrawList.GetNum() > 0 )
	{
		SCOPED_DRAW_EVENT( EventSprites, DEC_SPRITE, TEXT( "Sprites" ) );
		SDG.spriteDrawList.Draw( InDeviceContext, *sceneView );
	}

	// Draw dynamic meshes
	if ( showFlags & SHOW_DynamicElements && ( SDG.dynamicMeshElements.GetNum() > 0 ||
#if WITH_EDITOR
											   !SDG.dynamicMeshBuilders.empty()
#else
											   false
#endif // WITH_EDITOR
											   ) )
	{
		SCOPED_DRAW_EVENT( EventDynamicElements, DEC_DYNAMICELEMENTS, TEXT( "Dynamic elements" ) );

		// Draw dynamic mesh elements
		if ( SDG.dynamicMeshElements.GetNum() > 0 )
		{
			SDG.dynamicMeshElements.Draw( InDeviceContext, *sceneView );
		}

		// Draw dynamic mesh builders
#if WITH_EDITOR
		if ( !SDG.dynamicMeshBuilders.empty() )
		{
			for ( auto it = SDG.dynamicMeshBuilders.begin(), itEnd = SDG.dynamicMeshBuilders.end(); it != itEnd; ++it )
			{
				const SDynamicMeshBuilderElement& element = *it;
				if ( element.dynamicMeshBuilder )
				{
					element.dynamicMeshBuilder->Draw<CMeshDrawingPolicy>( InDeviceContext, element.localToWorldMatrix, element.material, *sceneView );
				}
			}
		}
#endif // WITH_EDITOR
	}

	return true;
}

/*
==================
CSceneRenderer::FinishRenderViewTarget
==================
*/
void CSceneRenderer::FinishRenderViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	SCOPED_DRAW_EVENT( EventFinishRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Finish Render View Target" ) );
	Assert( viewportRHI == InViewportRHI );

	// Clear visible view on finish of the scene render
	if ( scene )
	{
		scene->ClearView();
	}

	CBaseDeviceContextRHI*					immediateContext	= g_RHI->GetImmediateContext();
	Texture2DRHIRef_t						sceneColorTexture	= g_SceneRenderTargets.GetSceneColorLDRTexture();
	const uint32							sceneColorSizeX		= sceneColorTexture->GetSizeX();
	const uint32							sceneColorSizeY		= sceneColorTexture->GetSizeY();
	const uint32							viewportSizeX		= InViewportRHI->GetWidth();
	const uint32							viewportSizeY		= InViewportRHI->GetHeight();
	CScreenVertexShader<SVST_Default>*		screenVertexShader	= g_ShaderManager->FindInstance< CScreenVertexShader<SVST_Default>, CSimpleElementVertexFactory >();
	CScreenPixelShader*						screenPixelShader	= g_ShaderManager->FindInstance< CScreenPixelShader, CSimpleElementVertexFactory >();
	Assert( screenVertexShader && screenPixelShader );
	
	g_RHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), nullptr );
	g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	g_RHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	g_RHI->SetBlendState( immediateContext, TStaticBlendStateRHI<>::GetRHI() );
	g_RHI->SetBoundShaderState( immediateContext, g_RHI->CreateBoundShaderState( TEXT( "FinishRenderViewTarget" ), g_SimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

	screenPixelShader->SetTexture( immediateContext, sceneColorTexture );
	screenPixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	DrawDenormalizedQuad( immediateContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorSizeX, sceneColorSizeY, 1.f );
	viewportRHI = nullptr;
}
