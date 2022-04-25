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
#include "Render/Shaders/ScreenShader.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/Texture.h"
#include "Render/SceneRenderTargets.h"
#include "Render/VertexFactory/LocalVertexFactory.h"

/**
 * @ingroup Engine
 * Draws a quad with the given vertex positions and UVs in denormalized pixel/texel coordinates.
 * Note that the positions are affected by the current viewport
 * 
 * @param InDeviceContextRHI		RHI Device context
 * @param InX						Position by X in screen pixels of the top left corner of the quad
 * @param InY						Position by Y in screen pixels of the top left corner of the quad
 * @param InSizeX					Size by X in pixels of the quad
 * @param InSizeY					Size by Y in pixels of the quad
 * @param InU						Position by U in texels of the top left corner of the quad's UV's
 * @param InV						Position by V in texels of the top left corner of the quad's UV's
 * @param InTargetSizeX				Size by X in screen pixels of the target surface
 * @param InTargetSizeY				Size by Y in screen pixels of the target surface
 * @param InTextureSizeX			Size by X in texels of the source texture
 * @param InTextureSyzeY			Size by Y in texels of the source texture
 * @param InClipSpaceQuadZ			Clip space quad by Z
 */
void DrawDenormalizedQuad( class FBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ )
{
	// Set up the vertices
	FLocalVertexType		vertices[ 4 ];

	vertices[ 0 ].position = FVector4D( InX,			InY,				InClipSpaceQuadZ, 1.f );
	vertices[ 1 ].position = FVector4D( InX + InSizeX,	InY,				InClipSpaceQuadZ, 1.f );
	vertices[ 2 ].position = FVector4D( InX,			InY + InSizeY,		InClipSpaceQuadZ, 1.f );
	vertices[ 3 ].position = FVector4D( InX + InSizeX,	InY + InSizeY,		InClipSpaceQuadZ, 1.f );

	vertices[ 0 ].texCoord = FVector2D( InU,				InV );
	vertices[ 1 ].texCoord = FVector2D( InU + InSizeU,		InV );
	vertices[ 2 ].texCoord = FVector2D( InU,				InV + InSizeV );
	vertices[ 3 ].texCoord = FVector2D( InU + InSizeU,		 InV + InSizeV );

	for ( uint32 vertexIndex = 0; vertexIndex < 4; ++vertexIndex )
	{
		vertices[ vertexIndex ].position.x		= -1.0f + 2.0f * ( vertices[ vertexIndex ].position.x - GPixelCenterOffset ) / ( float )InTargetSizeX;
		vertices[ vertexIndex ].position.y		= +1.0f - 2.0f * ( vertices[ vertexIndex ].position.y - GPixelCenterOffset ) / ( float )InTargetSizeY;

		vertices[ vertexIndex ].texCoord.x		= vertices[ vertexIndex ].texCoord.x / ( float )InTextureSizeX;
		vertices[ vertexIndex ].texCoord.y		= vertices[ vertexIndex ].texCoord.y / ( float )InTextureSizeY;
	}

	static uint32 indices[] =		{ 0, 1, 3, 0, 3, 2 };

	// Draw quad
	GRHI->DrawIndexedPrimitiveUP( InDeviceContextRHI, PT_TriangleList, 0, 2, 4, indices, sizeof( indices[ 0 ] ), vertices, sizeof( vertices[ 0 ] ) );
}

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

	GRHI->SetRenderTarget( immediateContext, InViewportRHI->GetSurface(), nullptr );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
	GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "FinishRenderViewTarget" ), GLocalVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

	screenPixelShader->SetTexture( immediateContext, sceneColorTexture );
	screenPixelShader->SetSamplerState( immediateContext, TStaticSamplerStateRHI<>::GetRHI() );
	DrawDenormalizedQuad( immediateContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorSizeX, sceneColorSizeY, 1.f );
}
