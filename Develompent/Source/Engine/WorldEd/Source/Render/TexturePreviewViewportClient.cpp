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

CTexturePreviewViewportClient::CTexturePreviewViewportClient( const TSharedPtr<CTexture2D>& InTexture2D )
	: CEditorLevelViewportClient( LVT_OrthoXY )
	, texture2D( InTexture2D )
	, colorChannelMask( ÑColor::white )
{}

void CTexturePreviewViewportClient::Draw( CViewport* InViewport )
{
	check( InViewport );
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

void CTexturePreviewViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, const TSharedPtr<CTexture2D>& InTexture2D, class CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw quad with texture
	if ( InTexture2D )
	{
		SCOPED_DRAW_EVENT( EventDrawPreviewTexture, DEC_SCENE_ITEMS, ÑString::Format( TEXT( "Preview %s" ), InTexture2D->GetAssetName().c_str() ).c_str() );

		Texture2DRHIRef_t							texture2DRHI				= InTexture2D->GetTexture2DRHI();
		CScreenVertexShader<SVST_Fullscreen>*		screenVertexShader			= GShaderManager->FindInstance< CScreenVertexShader<SVST_Fullscreen>, CSimpleElementVertexFactory >();
		CTexturePreviewPixelShader*					texturePreviewPixelShader	= GShaderManager->FindInstance< CTexturePreviewPixelShader, CSimpleElementVertexFactory >();
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

CSceneView* CTexturePreviewViewportClient::CalcSceneView( uint32 InSizeX, uint32 InSizeY )
{
	// Calculate projection matrix
	const float			halfWidth			= InSizeX / 2.f;
	const float			halfHeight			= InSizeY / 2.f;
	Matrix				projectionMatrix	= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );

	// Result
	CSceneView*			sceneView = new CSceneView( projectionMatrix, SMath::matrixIdentity, InSizeX, InSizeY, GetBackgroundColor(), SHOW_DefaultEditor );
	return sceneView;
}

ÑColor CTexturePreviewViewportClient::GetBackgroundColor() const
{
	return ÑColor::black;
}