#include "Containers/String.h"
#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
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
#include "Render/VertexFactory/LocalVertexFactory.h"
#include "Render/SceneRenderTargets.h"

FStaticMeshDrawPolicy::FStaticMeshDrawPolicy()
{}

void FStaticMeshDrawPolicy::SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI )
{
	FMeshDrawingPolicy::SetShaderParameters( InDeviceContextRHI );

	FTexture2DRef		texture2d;
	if ( material->GetTextureParameterValue( TEXT( "diffuse" ), texture2d ) )
	{
		GRHI->SetTextureParameter( InDeviceContextRHI, pixelShader->GetPixelShader(), texture2d->GetTexture2DRHI(), 0 );
		GRHI->SetSamplerState( InDeviceContextRHI, pixelShader->GetPixelShader(), GRHI->CreateSamplerState( texture2d->GetSamplerStateInitialiser() ), 0 );
	}
}

FORCEINLINE const tchar* GetSceneSDGName( ESceneDepthGroup SDG )
{
	switch ( SDG )
	{
	case SDG_World:		return TEXT( "World" );
	default:			return TEXT( "Unknown" );
	}
}

FSceneRenderer::FSceneRenderer( FSceneView* InSceneView )
	: sceneView( InSceneView )
{}

void FSceneRenderer::BeginRenderViewTarget( FViewportRHIParamRef InViewportRHI )
{
	GSceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();

	GRHI->SetRenderTarget( immediateContext, GSceneRenderTargets.GetSceneColorSurface(), GSceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( GSceneRenderTargets.GetSceneColorSurface(), sceneView->GetBackgroundColor() );
	immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
	
	GRHI->SetViewParameters( immediateContext, *sceneView );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );
}

void FSceneRenderer::Render( FViewportRHIParamRef InViewportRHI )
{
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	EShowFlags					showFlags	= sceneView->GetShowFlags();
	FScene*						scene		= ( FScene* )GWorld->GetScene();
    scene->BuildSDGs( *sceneView );

	// Render scene layers
	{
		SCOPED_DRAW_EVENT( EventSDGs, DEC_SCENE_ITEMS, TEXT( "SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			SCOPED_DRAW_EVENT( EventSDG, DEC_SCENE_ITEMS, FString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup ) SDGIndex ) ).c_str() );
			FSceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup ) SDGIndex );

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
		}
	}
}

void FSceneRenderer::FinishRenderViewTarget( FViewportRHIParamRef InViewportRHI )
{
	FBaseDeviceContextRHI*		immediateContext	= GRHI->GetImmediateContext();
	FTexture2DRHIRef			sceneColorTexture	= GSceneRenderTargets.GetSceneColorTexture();
	const uint32				sceneColorSizeX		= sceneColorTexture->GetSizeX();
	const uint32				sceneColorSizeY		= sceneColorTexture->GetSizeY();
	const uint32				viewportSizeX		= InViewportRHI->GetWidth();
	const uint32				viewportSizeY		= InViewportRHI->GetHeight();
	FScreenVertexShader*		screenVertexShader	= GShaderManager->FindInstance< FScreenVertexShader, FLocalVertexFactory >();
	FScreenPixelShader*			screenPixelShader	= GShaderManager->FindInstance< FScreenPixelShader, FLocalVertexFactory >();
	check( screenVertexShader && screenPixelShader );

	GRHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), nullptr );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "FinishRenderViewTarget" ), GLocalVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

	screenPixelShader->SetTexture( immediateContext, sceneColorTexture );
	screenPixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	DrawDenormalizedQuad( immediateContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorSizeX, sceneColorSizeY, 1.f );
}
