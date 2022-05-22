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

FStaticMeshDrawPolicy::FStaticMeshDrawPolicy()
{}

void FStaticMeshDrawPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	FMeshDrawingPolicy::SetShaderParameters( InDeviceContextRHI );

	TSharedPtr<FMaterial>		materialRef = material.ToSharedPtr();
	if ( !materialRef )
	{
		materialRef = GEngine->GetDefaultMaterial().ToSharedPtr();
		if ( !materialRef )
		{
			return;
		}
	}

	TAssetHandle<FTexture2D>		texture2d;
	materialRef->GetTextureParameterValue( TEXT( "diffuse" ), texture2d );
	if ( texture2d )
	{
		TSharedPtr<FTexture2D>		texture2DRef = texture2d.ToSharedPtr();
		GRHI->SetTextureParameter( InDeviceContextRHI, pixelShader->GetPixelShader(), texture2DRef->GetTexture2DRHI(), 0 );
		GRHI->SetSamplerState( InDeviceContextRHI, pixelShader->GetPixelShader(), GRHI->CreateSamplerState( texture2DRef->GetSamplerStateInitialiser() ), 0 );
	}
}

FORCEINLINE const tchar* GetSceneSDGName( ESceneDepthGroup SDG )
{
	switch ( SDG )
	{
	case SDG_WorldEdBackground:		return TEXT( "WorldEd Background" );
	case SDG_World:					return TEXT( "World" );
	case SDG_WorldEdForeground:		return TEXT( "WorldEd Foreground" );
	default:						return TEXT( "Unknown" );
	}
}

FSceneRenderer::FSceneRenderer( FSceneView* InSceneView, class FScene* InScene /* = nullptr */ )
	: scene( InScene )
	, sceneView( InSceneView )
{}

void FSceneRenderer::BeginRenderViewTarget( FViewportRHIParamRef InViewportRHI )
{
	SCOPED_DRAW_EVENT( EventBeginRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Begin Render View Target" ) );
	GSceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();

	GRHI->SetRenderTarget( immediateContext, GSceneRenderTargets.GetSceneColorSurface(), GSceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( GSceneRenderTargets.GetSceneColorSurface(), sceneView->GetBackgroundColor() );
	immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
	
	GRHI->SetViewParameters( immediateContext, *sceneView );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );

	// Build visible primitives on all SDGs
	if ( scene )
	{
		scene->BuildSDGs( *sceneView );
	}
}

void FSceneRenderer::Render( FViewportRHIParamRef InViewportRHI )
{
	if ( !scene )
	{
		return;
	}

	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	EShowFlags					showFlags	= sceneView->GetShowFlags();

	// Render scene layers
	{
		SCOPED_DRAW_EVENT( EventSDGs, DEC_SCENE_ITEMS, TEXT( "SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			FSceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup )SDGIndex );
			if ( SDG.IsEmpty() )
			{
				continue;
			}

			SCOPED_DRAW_EVENT( EventSDG, DEC_SCENE_ITEMS, FString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup )SDGIndex ) ).c_str() );

#if !SHIPPING_BUILD
			// Draw simple elements
			if ( showFlags & SHOW_SimpleElements && !SDG.simpleElements.IsEmpty() )
			{
				SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Simple elements" ) );
				SDG.simpleElements.Draw( immediateContext, *sceneView );
			}
#endif // !SHIPPING_BUILD

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
			if ( showFlags & SHOW_DynamicElements && SDG.dynamicMeshElements.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventDynamicElements, DEC_SIMPLEELEMENTS, TEXT( "Dynamic elements" ) );
				SDG.dynamicMeshElements.Draw( immediateContext, *sceneView );
			}
		}
	}
}

void FSceneRenderer::FinishRenderViewTarget( FViewportRHIParamRef InViewportRHI )
{
	SCOPED_DRAW_EVENT( EventFinishRenderViewTarget, DEC_SCENE_ITEMS, TEXT( "Finish Render View Target" ) );

	// Clear all SDGs on finish of the scene render
	if ( scene )
	{
		scene->ClearSDGs();
	}

	FBaseDeviceContextRHI*					immediateContext	= GRHI->GetImmediateContext();
	FTexture2DRHIRef						sceneColorTexture	= GSceneRenderTargets.GetSceneColorTexture();
	const uint32							sceneColorSizeX		= sceneColorTexture->GetSizeX();
	const uint32							sceneColorSizeY		= sceneColorTexture->GetSizeY();
	const uint32							viewportSizeX		= InViewportRHI->GetWidth();
	const uint32							viewportSizeY		= InViewportRHI->GetHeight();
	FScreenVertexShader<SVST_Default>*		screenVertexShader	= GShaderManager->FindInstance< FScreenVertexShader<SVST_Default>, FSimpleElementVertexFactory >();
	FScreenPixelShader*						screenPixelShader	= GShaderManager->FindInstance< FScreenPixelShader, FSimpleElementVertexFactory >();
	check( screenVertexShader && screenPixelShader );

	GRHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), nullptr );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "FinishRenderViewTarget" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

	screenPixelShader->SetTexture( immediateContext, sceneColorTexture );
	screenPixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	DrawDenormalizedQuad( immediateContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorSizeX, sceneColorSizeY, 1.f );
}
