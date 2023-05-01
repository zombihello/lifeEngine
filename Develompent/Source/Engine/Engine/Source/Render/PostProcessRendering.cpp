#include <half.hpp>

#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "System/World.h"
#include "Render/SceneRendering.h"
#include "Render/SceneUtils.h"
#include "Render/SceneRenderTargets.h"
#include "Render/Scene.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/PostProcessShader.h"
#include "RHI/StaticStatesRHI.h"

static FORCEINLINE uint32 GetNearestPow2( uint32 InResolution )
{
	//Rounded exponent. Adds +0.5f to round to the nearest integer
	return SMath::Pow( 2, SMath::Log2( InResolution * 0.5f ) + 0.5f );
}

void CSceneRenderer::RenderPostProcess( class CBaseDeviceContextRHI* InDeviceContext )
{
	SCOPED_DRAW_EVENT( EventPostProcess, DEC_POSTPROCESS, TEXT( "PostProcess" ) );

	Texture2DRHIRef_t						sceneColorHDRTexture	= GSceneRenderTargets.GetSceneColorHDRTexture();
	const uint32							sceneColorHDRSizeX		= sceneColorHDRTexture->GetSizeX();
	const uint32							sceneColorHDRSizeY		= sceneColorHDRTexture->GetSizeY();
	const uint32							viewportSizeX			= viewportRHI->GetWidth();
	const uint32							viewportSizeY			= viewportRHI->GetHeight();
	CScreenVertexShader<SVST_Default>*		screenVertexShader		= GShaderManager->FindInstance< CScreenVertexShader<SVST_Default>, CSimpleElementVertexFactory >();
	TPostProcessPixelShader<true, true>*	postProcessPixelShader	= GShaderManager->FindInstance< TPostProcessPixelShader<true, true>, CSimpleElementVertexFactory >();
	check( screenVertexShader && postProcessPixelShader );

	GSceneRenderTargets.BeginRenderingSceneColorLDR( InDeviceContext );
	InDeviceContext->ClearSurface( GSceneRenderTargets.GetSceneColorLDRSurface(), sceneView->GetBackgroundColor() );

	GRHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
	GRHI->SetBoundShaderState( InDeviceContext, GRHI->CreateBoundShaderState( TEXT( "PostProcess" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), postProcessPixelShader->GetPixelShader() ) );
	
	postProcessPixelShader->SetTexture( InDeviceContext, sceneColorHDRTexture );
	postProcessPixelShader->SetSamplerState( InDeviceContext, TStaticSamplerStateRHI<>::GetRHI() );
	postProcessPixelShader->SetExposure( InDeviceContext, scene->GetExposure() );
	postProcessPixelShader->SetGamma( InDeviceContext, GEngine->GetGamma() );
	GRHI->CommitConstants( InDeviceContext );
	DrawDenormalizedQuad( InDeviceContext, 1, 1, viewportSizeX, viewportSizeY, 0, 0, viewportSizeX, viewportSizeY, viewportSizeX, viewportSizeY, sceneColorHDRSizeX, sceneColorHDRSizeY, 1.f );
	GSceneRenderTargets.FinishRenderingSceneColorLDR( InDeviceContext );

	// Calculate average luminance of scene for next frame
	if ( GEngine->IsAutoExposure() )
	{
		SCOPED_DRAW_EVENT( EventCalcAvgLuminance, DEC_LIGHT, TEXT( "CalcAvgLuminance" ) );
		
		bool									bFirstIteration		= true;
		uint32									readBuffer			= 1;
		uint32									writeBuffer			= 0;
		uint32									textureSizeX		= sceneColorHDRTexture->GetSizeX();
		uint32									textureSizeY		= sceneColorHDRTexture->GetSizeY();
		CScreenVertexShader<SVST_Fullscreen>*	screenVertexShader	= GShaderManager->FindInstance<CScreenVertexShader<SVST_Fullscreen>, CSimpleElementVertexFactory>();
		CScreenPixelShader*						screenPixelShader	= GShaderManager->FindInstance<CScreenPixelShader, CSimpleElementVertexFactory>();
		CRenderTarget							avgLuminanceBuffers[2];			// Auxiliary buffers for calculating the average luminance value
		check( screenVertexShader && screenPixelShader );

		// Get original viewport size for restore after
		uint32									originalMinX = 0;
		uint32									originalMinY = 0;
		float									originalMinZ = 0.f;
		uint32									originalMaxX = 0;
		uint32									originalMaxY = 0;
		float									originalMaxZ = 0.f;
		GRHI->GetViewport( originalMinX, originalMinY, originalMinZ, originalMaxX, originalMaxY, originalMaxZ );

		// Generate mipmaps for get 1x1 texture
		while ( textureSizeX > 1 )
		{
			// Init luminance buffer
			// TODO BS yehor.pohuliaka - Need to optimize the luminance buffer update so that it doesn't happen every frame
			textureSizeX = Max<uint32>( GetNearestPow2( textureSizeX ), 1 );
			textureSizeY = Max<uint32>( GetNearestPow2( textureSizeY ), 1 );
			avgLuminanceBuffers[writeBuffer].Update( false, textureSizeX, textureSizeY, sceneColorHDRTexture->GetFormat(), 0 );

			GRHI->SetViewport( InDeviceContext, 0, 0, 0.f, textureSizeX, textureSizeY, 1.f );
			GRHI->SetRenderTarget( InDeviceContext, avgLuminanceBuffers[writeBuffer].GetSurfaceRHI(), nullptr );
			GRHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<false>::GetRHI() );
			GRHI->SetRasterizerState( InDeviceContext, TStaticRasterizerStateRHI<>::GetRHI() );
			GRHI->SetBlendState( InDeviceContext, TStaticBlendStateRHI<>::GetRHI() );
			GRHI->SetBoundShaderState( InDeviceContext, GRHI->CreateBoundShaderState( TEXT( "CalcAvgLuminance" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), screenVertexShader->GetVertexShader(), screenPixelShader->GetPixelShader() ) );

			// Draw downsample of scene color HDR
			screenPixelShader->SetTexture( InDeviceContext, bFirstIteration ? sceneColorHDRTexture : avgLuminanceBuffers[readBuffer].GetTexture2DRHI() );
			screenPixelShader->SetSamplerState( InDeviceContext, TStaticSamplerStateRHI<SF_Trilinear>::GetRHI() );
			GRHI->DrawPrimitive( InDeviceContext, PT_TriangleList, 0, 1 );

			Swap( readBuffer, writeBuffer );
			bFirstIteration = false;
		}

		SLockedData			lockedData;
		GRHI->LockTexture2D( InDeviceContext, avgLuminanceBuffers[readBuffer].GetTexture2DRHI(), 0, false, lockedData );
		half_float::half*	luminescence	= ( half_float::half* )lockedData.data;
		float				avgLuminance	= 0.2126f * luminescence[0] + 0.7152f * luminescence[1] + 0.0722f * luminescence[2];
		float				hdrExposure		= SMath::Lerp( scene->GetExposure(), 0.023f / Max( avgLuminance, 0.0001f ), 0.005f );
		GRHI->UnlockTexture2D( InDeviceContext, avgLuminanceBuffers[readBuffer].GetTexture2DRHI(), 0, lockedData );

		// Restore viewport size and update HDR exposure
		GRHI->SetViewport( InDeviceContext, originalMinX, originalMinY, originalMinZ, originalMaxX, originalMaxY, originalMaxZ );
		scene->SetExposure( Clamp( hdrExposure, GEngine->GetExposureMin(), GEngine->GetExposureMax() ) );
	}
}