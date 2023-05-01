#include "Misc/EngineGlobals.h"
#include "Render/SceneRendering.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/SceneRenderTargets.h"
#include "RHI/StaticStatesRHI.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"

void CSceneRenderer::RenderUI( class CBaseDeviceContextRHI* InDeviceContext )
{
	SCOPED_DRAW_EVENT( EventUI, DEC_CANVAS, TEXT( "UI" ) );
	GSceneRenderTargets.BeginRenderingSceneColorLDR( InDeviceContext );
	GRHI->SetBlendState( InDeviceContext, TStaticBlendStateRHI<>::GetRHI() );
	GRHI->SetDepthState( InDeviceContext, TStaticDepthStateRHI<true>::GetRHI() );

	// Render WorldEd background and foreground layer (only in editor)
#if WITH_EDITOR
	if ( GIsEditor )
	{
		RenderSDG( InDeviceContext, SDG_WorldEdBackground );
		InDeviceContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
		RenderSDG( InDeviceContext, SDG_WorldEdForeground );
	}
	else
#endif // WITH_EDITOR
	{
		InDeviceContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );
	}

	RenderSDG( InDeviceContext, SDG_Foreground );
	GSceneRenderTargets.FinishRenderingSceneColorLDR( InDeviceContext );
}