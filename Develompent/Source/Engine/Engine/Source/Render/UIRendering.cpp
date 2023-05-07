#include "Misc/EngineGlobals.h"
#include "Render/SceneRendering.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/SceneRenderTargets.h"
#include "RHI/StaticStatesRHI.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"

/*
==================
CSceneRenderer::RenderUI
==================
*/
void CSceneRenderer::RenderUI( class CBaseDeviceContextRHI* InDeviceContext )
{
	SCOPED_DRAW_EVENT( EventUI, DEC_CANVAS, TEXT( "UI" ) );
	g_SceneRenderTargets.BeginRenderingSceneColorLDR( InDeviceContext );
	g_RHI->SetBlendState( InDeviceContext, TStaticBlendStateRHI<>::GetRHI() );
	g_RHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<true>::GetRHI() );

	// Render WorldEd background and foreground layer (only in editor)
#if WITH_EDITOR
	if ( g_IsEditor )
	{
		RenderSDG( InDeviceContext, SDG_WorldEdBackground );
		InDeviceContext->ClearDepthStencil( g_SceneRenderTargets.GetSceneDepthZSurface() );
		RenderSDG( InDeviceContext, SDG_WorldEdForeground );
	}
	else
#endif // WITH_EDITOR
	{
		InDeviceContext->ClearDepthStencil( g_SceneRenderTargets.GetSceneDepthZSurface() );
	}

	RenderSDG( InDeviceContext, SDG_Foreground );
	g_SceneRenderTargets.FinishRenderingSceneColorLDR( InDeviceContext );
}