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

CStaticMeshDrawPolicy::CStaticMeshDrawPolicy()
{}

void CStaticMeshDrawPolicy::SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI )
{
	CMeshDrawingPolicy::SetShaderParameters( InDeviceContextRHI );

	TSharedPtr<CMaterial>		materialRef = material.ToSharedPtr();
	if ( !materialRef )
	{
		materialRef = GEngine->GetDefaultMaterial().ToSharedPtr();
		if ( !materialRef )
		{
			return;
		}
	}

	TAssetHandle<CTexture2D>		texture2d;
	materialRef->GetTextureParameterValue( TEXT( "diffuse" ), texture2d );
	if ( texture2d.IsAssetValid() )
	{
		TSharedPtr<CTexture2D>		texture2DRef = texture2d.ToSharedPtr();
		GRHI->SetTextureParameter( InDeviceContextRHI, pixelShader->GetPixelShader(), texture2DRef->GetTexture2DRHI(), 0 );
		GRHI->SetSamplerState( InDeviceContextRHI, pixelShader->GetPixelShader(), GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ), 0 );
	}
}

CSceneRenderer::CSceneRenderer( CSceneView* InSceneView, class CScene* InScene /* = nullptr */ )
	: scene( InScene )
	, sceneView( InSceneView )
{}

void CSceneRenderer::BeginRenderViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	SCOPED_DRAW_EVENT( EventBeginRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Begin Render View Target" ) );
	GSceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();

	GRHI->SetRenderTarget( immediateContext, GSceneRenderTargets.GetSceneColorSurface(), GSceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( GSceneRenderTargets.GetSceneColorSurface(), sceneView->GetBackgroundColor() );
	immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
	
	GRHI->SetViewParameters( immediateContext, *sceneView );

	// If enabled wireframe mode, we disable depth text
#if WITH_EDITOR
	if ( sceneView->GetShowFlags() & SHOW_Wireframe )
	{
		GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
	}
	else
#endif // WITH_EDITOR
	{
		GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );
	}

	// Build visible primitives on all SDGs
	if ( scene )
	{
		scene->BuildSDGs( *sceneView );
	}
}

void CSceneRenderer::Render( ViewportRHIParamRef_t InViewportRHI )
{
	if ( !scene )
	{
		return;
	}

	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	ShowFlags_t					showFlags	= sceneView->GetShowFlags();

	// Render scene layers
	{
		SCOPED_DRAW_EVENT( EventSDGs, DEC_SCENE_ITEMS, TEXT( "SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			SSceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup )SDGIndex );
			if ( SDG.IsEmpty() )
			{
				continue;
			}

			SCOPED_DRAW_EVENT( EventSDG, DEC_SCENE_ITEMS, ÑString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup )SDGIndex ) ).c_str() );


			// Clear depth buffer for foreground layer
			if ( SDGIndex == SDG_Foreground 
#if WITH_EDITOR
				 || SDGIndex == SDG_WorldEdForeground 
#endif // WITH_EDITOR
				 )
			{
				immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
			}

#if WITH_EDITOR
			// Draw simple elements
			if ( showFlags & SHOW_SimpleElements && !SDG.simpleElements.IsEmpty() )
			{
				SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Simple elements" ) );
				SDG.simpleElements.Draw( immediateContext, *sceneView );
			}

			// Draw gizmos
			if ( showFlags & SHOW_Gizmo && SDG.gizmoDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventGizmos, DEC_SPRITE, TEXT( "Gizmos" ) );
				SDG.gizmoDrawList.Draw( immediateContext, *sceneView );
			}
#endif // WITH_EDITOR

			// Draw static meshes
			if ( showFlags & SHOW_StaticMesh && SDG.staticMeshDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventStaticMeshes, DEC_STATIC_MESH, TEXT( "Static meshes" ) );
				SDG.staticMeshDrawList.Draw( immediateContext, *sceneView );
			}

			// Draw sprites
			if ( showFlags & SHOW_Sprite && SDG.spriteDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventSprites, DEC_SPRITE, TEXT( "Sprites" ) );
				SDG.spriteDrawList.Draw( immediateContext, *sceneView );
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
					SDG.dynamicMeshElements.Draw( immediateContext, *sceneView );
				}

				// Draw dynamic mesh builders
#if WITH_EDITOR
				if ( !SDG.dynamicMeshBuilders.empty() )
				{
					for ( auto it = SDG.dynamicMeshBuilders.begin(), itEnd = SDG.dynamicMeshBuilders.end(); it != itEnd; ++it )
					{
						const SDynamicMeshBuilderElement&		element = *it;
						if ( element.dynamicMeshBuilder )
						{
							element.dynamicMeshBuilder->Draw<CStaticMeshDrawPolicy>( immediateContext, element.localToWorldMatrix, element.material, *sceneView );
						}
					}
				}
#endif // WITH_EDITOR
			}
		}
	}
}

void CSceneRenderer::FinishRenderViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	SCOPED_DRAW_EVENT( EventFinishRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Finish Render View Target" ) );

	// Clear all SDGs on finish of the scene render
	if ( scene )
	{
		scene->ClearSDGs();
	}

	CBaseDeviceContextRHI*					immediateContext	= GRHI->GetImmediateContext();
	Texture2DRHIRef_t						sceneColorTexture	= GSceneRenderTargets.GetSceneColorTexture();
	const uint32							sceneColorSizeX		= sceneColorTexture->GetSizeX();
	const uint32							sceneColorSizeY		= sceneColorTexture->GetSizeY();
	const uint32							viewportSizeX		= InViewportRHI->GetWidth();
	const uint32							viewportSizeY		= InViewportRHI->GetHeight();
	CScreenVertexShader<SVST_Default>*		screenVertexShader	= GShaderManager->FindInstance< CScreenVertexShader<SVST_Default>, CSimpleElementVertexFactory >();
	CScreenPixelShader*						screenPixelShader	= GShaderManager->FindInstance< CScreenPixelShader, CSimpleElementVertexFactory >();
	check( screenVertexShader && screenPixelShader );

	GRHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), nullptr );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "FinishRenderViewTarget" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

	screenPixelShader->SetTexture( immediateContext, sceneColorTexture );
	screenPixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	DrawDenormalizedQuad( immediateContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorSizeX, sceneColorSizeY, 1.f );
}
