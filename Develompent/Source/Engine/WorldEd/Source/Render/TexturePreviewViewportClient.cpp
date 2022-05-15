#include "Math/Math.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/TexturePreviewShader.h"
#include "RHI/StaticStatesRHI.h"
#include "EngineDefines.h"

FTexturePreviewViewportClient::FTexturePreviewViewportClient( FTexture2D* InTexture2D )
	: FEditorLevelViewportClient( LVT_OrthoXY )
	, texture2D( InTexture2D )
	, colorChannelMask( FColor::white )
{}

void FTexturePreviewViewportClient::Draw( FViewport* InViewport )
{
	FSceneView*		sceneView = CalcSceneView( InViewport );

	// Draw preview texture viewport
	UNIQUE_RENDER_COMMAND_FOURPARAMETER(  FViewportRenderCommand,
										  FTexturePreviewViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FTexture2DRef, texture2D, texture2D,
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, texture2D, sceneView );
										  } );
}

void FTexturePreviewViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, FTexture2D* InTexture2D, class FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw quad with texture
	if ( InTexture2D )
	{
		SCOPED_DRAW_EVENT( EventDrawPreviewTexture, DEC_SCENE_ITEMS, FString::Format( TEXT( "Preview %s" ), InTexture2D->GetAssetName().c_str() ).c_str() );

		FTexture2DRHIRef							texture2DRHI				= InTexture2D->GetTexture2DRHI();
		FScreenVertexShader<SVST_Fullscreen>*		screenVertexShader			= GShaderManager->FindInstance< FScreenVertexShader<SVST_Fullscreen>, FSimpleElementVertexFactory >();
		FTexturePreviewPixelShader*					texturePreviewPixelShader	= GShaderManager->FindInstance< FTexturePreviewPixelShader, FSimpleElementVertexFactory >();
		check( screenVertexShader && texturePreviewPixelShader );

		GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
		GRHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
		GRHI->SetBoundShaderState( immediateContext, GRHI->CreateBoundShaderState( TEXT( "PreviewTexture" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), texturePreviewPixelShader->GetPixelShader() ) );

		texturePreviewPixelShader->SetTexture( immediateContext, texture2DRHI );
		texturePreviewPixelShader->SetSamplerState( immediateContext, GRHI->CreateSamplerState( InTexture2D->GetSamplerStateInitialiser() ) );
		texturePreviewPixelShader->SetColorChannelMask( immediateContext, colorChannelMask );
		GRHI->CommitConstants( immediateContext );
		GRHI->DrawPrimitive( immediateContext, PT_TriangleList, 0, 1 );
	}
	
	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

FSceneView* FTexturePreviewViewportClient::CalcSceneView( FViewport* InViewport )
{
	// Calculate projection matrix
	const float			halfWidth			= InViewport->GetSizeX() / 2.f;
	const float			halfHeight			= InViewport->GetSizeY() / 2.f;
	FMatrix				projectionMatrix	= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );

	// Result
	FSceneView*			sceneView = new FSceneView( projectionMatrix, FMath::matrixIdentity, InViewport->GetSizeX(), InViewport->GetSizeY(), GetBackgroundColor(), SHOW_DefaultEditor );
	return sceneView;
}

FColor FTexturePreviewViewportClient::GetBackgroundColor() const
{
	return FColor::black;
}