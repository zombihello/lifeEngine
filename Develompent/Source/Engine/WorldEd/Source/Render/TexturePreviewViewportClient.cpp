#include "Math/Math.h"
#include "Render/TexturePreviewViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/TexturePreviewShader.h"
#include "Render/SceneRenderTargets.h"
#include "RHI/StaticStatesRHI.h"
#include "EngineDefines.h"

/*
==================
CTexturePreviewViewportClient::CTexturePreviewViewportClient
==================
*/
CTexturePreviewViewportClient::CTexturePreviewViewportClient( const TSharedPtr<CTexture2D>& InTexture2D )
	: CEditorLevelViewportClient( LVT_OrthoXY )
	, texture2D( InTexture2D )
	, colorChannelMask( CColor::white )
	, mipmapToView( 0 )
{}

/*
==================
CTexturePreviewViewportClient::Draw
==================
*/
void CTexturePreviewViewportClient::Draw( CViewport* InViewport )
{
	Assert( InViewport );
	CSceneView*		sceneView = CalcSceneView( InViewport->GetSizeX(), InViewport->GetSizeY() );

	// Draw preview texture viewport
	UNIQUE_RENDER_COMMAND_FOURPARAMETER(  CViewportRenderCommand,
										  CTexturePreviewViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  TSharedPtr<CTexture2D>, texture2D, texture2D,
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, texture2D, sceneView );
										  } );
}

/*
==================
CTexturePreviewViewportClient::Draw_RenderThread
==================
*/
void CTexturePreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, const TSharedPtr<CTexture2D>& InTexture2D, class CSceneView* InSceneView )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw quad with texture
	if ( InTexture2D )
	{
		SCOPED_DRAW_EVENT( EventDrawPreviewTexture, DEC_SCENE_ITEMS, L_Sprintf( TEXT( "Preview %s" ), InTexture2D->GetAssetName().c_str() ).c_str() );

		Texture2DRHIRef_t							texture2DRHI				= InTexture2D->GetTexture2DRHI();
		CScreenVertexShader<SVST_Fullscreen>*		screenVertexShader			= g_ShaderManager->FindInstance< CScreenVertexShader<SVST_Fullscreen>, CSimpleElementVertexFactory >();
		CTexturePreviewPixelShader*					texturePreviewPixelShader	= g_ShaderManager->FindInstance< CTexturePreviewPixelShader, CSimpleElementVertexFactory >();
		Assert( screenVertexShader && texturePreviewPixelShader );

		g_SceneRenderTargets.BeginRenderingSceneColorLDR( immediateContext );
		immediateContext->ClearSurface( g_SceneRenderTargets.GetSceneColorLDRSurface(), GetBackgroundColor() );
		g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<false>::GetRHI() );
		g_RHI->SetRasterizerState( immediateContext, TStaticRasterizerStateRHI<>::GetRHI() );
		g_RHI->SetBoundShaderState( immediateContext, g_RHI->CreateBoundShaderState( TEXT( "PreviewTexture" ), g_SimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), texturePreviewPixelShader->GetPixelShader() ) );

		texturePreviewPixelShader->SetTexture( immediateContext, texture2DRHI );
		texturePreviewPixelShader->SetSamplerState( immediateContext, g_RHI->CreateSamplerState( InTexture2D->GetSamplerStateInitialiser() ) );
		texturePreviewPixelShader->SetColorChannelMask( immediateContext, colorChannelMask );
		texturePreviewPixelShader->SetMipmap( immediateContext, mipmapToView );
		g_RHI->CommitConstants( immediateContext );
		g_RHI->DrawPrimitive( immediateContext, PT_TriangleList, 0, 1 );
		g_SceneRenderTargets.FinishRenderingSceneColorLDR( immediateContext );
	}
	
	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

/*
==================
CTexturePreviewViewportClient::CalcSceneView
==================
*/
CSceneView* CTexturePreviewViewportClient::CalcSceneView( uint32 InSizeX, uint32 InSizeY )
{
	// Calculate projection matrix
	const float			halfWidth			= InSizeX / 2.f;
	const float			halfHeight			= InSizeY / 2.f;
	Matrix				projectionMatrix	= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );

	// Result
	CSceneView*			sceneView = new CSceneView( Math::vectorZero, projectionMatrix, Math::matrixIdentity, InSizeX, InSizeY, GetBackgroundColor(), SHOW_DefaultEditor & ~SHOW_Lights );
	return sceneView;
}

/*
==================
CTexturePreviewViewportClient::GetBackgroundColor
==================
*/
CColor CTexturePreviewViewportClient::GetBackgroundColor() const
{
	return CColor::black;
}