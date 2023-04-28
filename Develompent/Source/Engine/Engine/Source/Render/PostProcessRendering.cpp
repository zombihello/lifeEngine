#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "Render/SceneRendering.h"
#include "Render/SceneUtils.h"
#include "Render/SceneRenderTargets.h"
#include "Render/Scene.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/PostProcessShader.h"
#include "RHI/StaticStatesRHI.h"

void CSceneRenderer::RenderPostProcess( class CBaseDeviceContextRHI* InDeviceContext )
{
	SCOPED_DRAW_EVENT( EventPostProcess, DEC_POSTPROCESS, TEXT( "PostProcess" ) );

	Texture2DRHIRef_t						lightPassTexture		= GSceneRenderTargets.GetLightPassTexture();
	const uint32							lightPassSizeX			= lightPassTexture->GetSizeX();
	const uint32							lightPassSizeY			= lightPassTexture->GetSizeY();
	const uint32							viewportSizeX			= viewportRHI->GetWidth();
	const uint32							viewportSizeY			= viewportRHI->GetHeight();
	CScreenVertexShader<SVST_Default>*		screenVertexShader		= GShaderManager->FindInstance< CScreenVertexShader<SVST_Default>, CSimpleElementVertexFactory >();
	TPostProcessPixelShader<true, true>*	postProcessPixelShader	= GShaderManager->FindInstance< TPostProcessPixelShader<true, true>, CSimpleElementVertexFactory >();
	check( screenVertexShader && postProcessPixelShader );

	GSceneRenderTargets.BeginRenderingSceneColor( InDeviceContext );
	GRHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
	GRHI->SetRasterizerState( InDeviceContext, TStaticRasterizerStateRHI<>::GetRHI() );
	GRHI->SetBlendState( InDeviceContext, TStaticBlendStateRHI<>::GetRHI() );
	GRHI->SetBoundShaderState( InDeviceContext, GRHI->CreateBoundShaderState( TEXT( "PostProcess" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), postProcessPixelShader->GetPixelShader() ) );

	postProcessPixelShader->SetTexture( InDeviceContext, lightPassTexture );
	postProcessPixelShader->SetSamplerState( InDeviceContext, TStaticSamplerStateRHI<>::GetRHI() );
	postProcessPixelShader->SetHDRExposure( InDeviceContext, GEngine->GetHDRExposure() );
	postProcessPixelShader->SetGamma( InDeviceContext, GEngine->GetGamma() );
	DrawDenormalizedQuad( InDeviceContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, lightPassSizeX, lightPassSizeY, 1.f );
	
	GSceneRenderTargets.FinishRenderingSceneColor( InDeviceContext );
}