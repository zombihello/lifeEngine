/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Render/SceneHitProxyRendering.h"

#if ENABLE_HITPROXY
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "Engine/Render/SceneRendering.h"
#include "Engine/Render/Scene.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/SceneRenderTargets.h"
#include "Engine/Render/SceneUtils.h"

/*
==================
CSceneRenderer::BeginRenderHitProxiesViewTarget
==================
*/
void CSceneRenderer::BeginRenderHitProxiesViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	// Allocate the maximum scene render target space for the current view
	g_SceneRenderTargets.Allocate( InViewportRHI->GetWidth(), InViewportRHI->GetHeight() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();

	// Write to the hit proxy render target
	g_RHI->SetRenderTarget( immediateContext, g_SceneRenderTargets.GetHitProxySurface(), g_SceneRenderTargets.GetSceneDepthZSurface() );
	immediateContext->ClearSurface( g_SceneRenderTargets.GetHitProxySurface(), CColor::black );
	immediateContext->ClearDepthStencil( g_SceneRenderTargets.GetSceneDepthZSurface() );

	g_RHI->SetViewParameters( immediateContext, *sceneView );
	g_RHI->SetDepthState( immediateContext, TStaticDepthStateRHI<true>::GetRHI() );

	// Build visible view on scene
	if ( scene )
	{
		scene->BuildView( *sceneView );
	}
}

/*
==================
CSceneRenderer::RenderHitProxies
==================
*/
void CSceneRenderer::RenderHitProxies( ViewportRHIParamRef_t InViewportRHI, EHitProxyLayer InHitProxyLayer /* = HPL_World */ )
{
	ShowFlags_t					showFlags = sceneView->GetShowFlags();
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	if ( !( showFlags & SHOW_HitProxy ) )
	{
		return;
	}
	Assert( InHitProxyLayer >= 0 && InHitProxyLayer < HPL_Num );

	// Draw the hit proxy IDs for all visible primitives
	{
		SCOPED_DRAW_EVENT( EventHitProxiesSDGs, DEC_SCENE_ITEMS, TEXT( "HitProxies SDGs" ) );
		for ( uint32 SDGIndex = 0; SDGIndex < SDG_Max; ++SDGIndex )
		{
			SceneDepthGroup&		SDG = scene->GetSDG( ( ESceneDepthGroup ) SDGIndex );
			HitProxyLayer&			hitProxyLayer = SDG.hitProxyLayers[ InHitProxyLayer ];
			if ( hitProxyLayer.IsEmpty() )
			{
				continue;
			}

			SCOPED_DRAW_EVENT( EventHitProxiesSDG, DEC_SCENE_ITEMS, CString::Format( TEXT( "SDG %s" ), GetSceneSDGName( ( ESceneDepthGroup )SDGIndex ) ).c_str() );			

#if WITH_EDITOR
			// Draw simple elements
			if ( showFlags & SHOW_SimpleElements && !hitProxyLayer.simpleHitProxyElements.IsEmpty() )
			{
				SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Simple elements" ) );
				hitProxyLayer.simpleHitProxyElements.Draw( immediateContext, *sceneView );
			}

			// Draw dynamic meshes
			if ( showFlags & SHOW_DynamicElements && !hitProxyLayer.dynamicHitProxyMeshBuilders.empty() )
			{
				SCOPED_DRAW_EVENT( EventDynamicElements, DEC_DYNAMICELEMENTS, TEXT( "Dynamic elements" ) );
				for ( auto it = hitProxyLayer.dynamicHitProxyMeshBuilders.begin(), itEnd = hitProxyLayer.dynamicHitProxyMeshBuilders.end(); it != itEnd; ++it )
				{
					const DynamicMeshBuilderElement&		element = *it;
					if ( element.dynamicMeshBuilder )
					{
						element.dynamicMeshBuilder->Draw<CHitProxyDrawingPolicy>( immediateContext, element.localToWorldMatrix, element.material, *sceneView );
					}
				}
			}
#endif // WITH_EDITOR

			// Draw hit proxies
			if ( hitProxyLayer.hitProxyDrawList.GetNum() > 0 )
			{
				SCOPED_DRAW_EVENT( EventHitProxyMeshes, DEC_STATIC_MESH, TEXT( "HitProxy Meshes" ) );
				hitProxyLayer.hitProxyDrawList.Draw( immediateContext, *sceneView );
			}
		}
	}
}

/*
==================
CSceneRenderer::FinishRenderHitProxiesViewTarget
==================
*/
void CSceneRenderer::FinishRenderHitProxiesViewTarget( ViewportRHIParamRef_t InViewportRHI )
{
	// Clear visible view on finish of the scene render
	if ( scene )
	{
		scene->ClearView();
	}
}
#endif // ENABLE_HITPROXY