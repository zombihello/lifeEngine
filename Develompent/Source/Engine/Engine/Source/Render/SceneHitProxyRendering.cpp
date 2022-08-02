#include "Render/SceneHitProxyRendering.h"

#if ENABLE_HITPROXY
#include "Render/SceneRendering.h"
#include "Render/Scene.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Render/SceneRenderTargets.h"
#include "Render/SceneUtils.h"

void CSceneRenderer::RenderHitProxies( ViewportRHIParamRef_t InViewportRHI )
{
	// Allocate the maximum scene render target space for the current view
	GSceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();

	// Write to the hit proxy render target
	GRHI->SetRenderTarget( immediateContext, GSceneRenderTargets.GetHitProxySurface(), GSceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( GSceneRenderTargets.GetHitProxySurface(), CColor::black );
	immediateContext->ClearDepthStencil( GSceneRenderTargets.GetSceneDepthZSurface() );

	GRHI->SetViewParameters( immediateContext, *sceneView );
	GRHI->SetDepthTest( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );

	// Build visible primitives on all SDGs
	if ( scene )
	{
		scene->BuildSDGs( *sceneView );
	}

	// Draw the hit proxy IDs for all visible primitives
	{
		SCOPED_DRAW_EVENT( EventSDGs, DEC_SCENE_ITEMS, TEXT( "HitProxies SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			SSceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup ) SDGIndex );
			if ( SDG.IsEmpty() )
			{
				continue;
			}

			// Draw hit proxies
			if ( SDG.hitProxyDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventHitProxiesSDG, DEC_SCENE_ITEMS, ÑString::Format( TEXT( "HitProxies SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup )SDGIndex ) ).c_str() );
				SDG.hitProxyDrawList.Draw( immediateContext, *sceneView );
			}
		}
	}

	// Clear all SDGs on finish of the scene render
	if ( scene )
	{
		scene->ClearSDGs();
	}
}
#endif // ENABLE_HITPROXY