#include "Render/SceneRenderTargets.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

// -------------
// GLOBALS
// -------------
TGlobalResource< FSceneRenderTargets >				GSceneRenderTargets;

FSceneRenderTargets::FSceneRenderTargets()
	: bufferSizeX( 0 )
	, bufferSizeY( 0 )
{}

void FSceneRenderTargets::InitRHI()
{
	if ( bufferSizeX > 0 && bufferSizeY > 0 )
	{
		// Scene color
		renderTargets[ SRTT_SceneColor ].texture = GRHI->CreateTexture2D( TEXT( "SceneColor" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, 1, TCF_ResolveTargetable );
		renderTargets[ SRTT_SceneColor ].surface = GRHI->CreateTargetableSurface( TEXT( "SceneColor" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, renderTargets[ SRTT_SceneColor ].texture, TCF_ResolveTargetable );

		// Scene depth Z
		renderTargets[ SRTT_SceneDepthZ ].texture = GRHI->CreateTexture2D( TEXT( "SceneDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, 1, TCF_ResolveTargetable | TCF_DepthStencil );
		renderTargets[ SRTT_SceneDepthZ ].surface = GRHI->CreateTargetableSurface( TEXT( "SceneDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, renderTargets[ SRTT_SceneDepthZ ].texture, TCF_ResolveTargetable | TCF_DepthStencil );
	}
}

void FSceneRenderTargets::ReleaseRHI()
{
	for ( uint32 index = 0; index < SRTT_MaxSceneRenderTargets; ++index )
	{
		FSceneRenderTargetItem&		rtItem = renderTargets[ index ];
		rtItem.surface.SafeRelease();
		rtItem.texture.SafeRelease();
	}
}

void FSceneRenderTargets::Allocate( uint32 InNewSizeX, uint32 InNewSizeY )
{
	bufferSizeX = InNewSizeX;
	bufferSizeY = InNewSizeY;
	UpdateRHI();
}