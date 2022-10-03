#include "Render/SceneRenderTargets.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"

// -------------
// GLOBALS
// -------------
TGlobalResource< CSceneRenderTargets >				GSceneRenderTargets;

CSceneRenderTargets::CSceneRenderTargets()
	: bufferSizeX( 0 )
	, bufferSizeY( 0 )
{}

void CSceneRenderTargets::InitRHI()
{
	if ( bufferSizeX > 0 && bufferSizeY > 0 )
	{
		// Scene color
		renderTargets[SRTT_SceneColor].texture					= GRHI->CreateTexture2D( TEXT( "SceneColor" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, 1, TCF_ResolveTargetable );
		renderTargets[SRTT_SceneColor].surface					= GRHI->CreateTargetableSurface( TEXT( "SceneColor" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, renderTargets[SRTT_SceneColor].texture, TCF_ResolveTargetable );

		// Scene depth Z
		renderTargets[SRTT_SceneDepthZ].texture					= GRHI->CreateTexture2D( TEXT( "SceneDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, 1, TCF_ResolveTargetable | TCF_DepthStencil );
		renderTargets[SRTT_SceneDepthZ].surface					= GRHI->CreateTargetableSurface( TEXT( "SceneDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, renderTargets[SRTT_SceneDepthZ].texture, TCF_ResolveTargetable | TCF_DepthStencil );

		// Light attenuation depth Z
		renderTargets[SRRT_LightAttenuationDepthZ].texture		= GRHI->CreateTexture2D( TEXT( "LightAttenuationDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, 1, TCF_ResolveTargetable | TCF_DepthStencil );
		renderTargets[SRRT_LightAttenuationDepthZ].surface		= GRHI->CreateTargetableSurface( TEXT( "LightAttenuationDepthZ" ), bufferSizeX, bufferSizeY, PF_DepthStencil, renderTargets[SRRT_LightAttenuationDepthZ].texture, TCF_ResolveTargetable | TCF_DepthStencil );

#if ENABLE_HITPROXY
		// Hit proxy
		renderTargets[SRRT_HitProxies].texture					= GRHI->CreateTexture2D( TEXT( "HitProxy" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, 1, TCF_ResolveTargetable );
		renderTargets[SRRT_HitProxies].surface					= GRHI->CreateTargetableSurface( TEXT( "HitProxy" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, renderTargets[SRRT_HitProxies].texture, TCF_ResolveTargetable );
#endif // ENABLE_HITPROXY

		// Diffuse_Roughness GBuffer
		renderTargets[SRRT_Diffuse_Roughness_GBuffer].texture	= GRHI->CreateTexture2D( TEXT( "Diffuse_Roughness_GBuffer" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, 1, TCF_ResolveTargetable );
		renderTargets[SRRT_Diffuse_Roughness_GBuffer].surface	= GRHI->CreateTargetableSurface( TEXT( "Diffuse_Roughness_GBuffer" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, renderTargets[SRRT_Diffuse_Roughness_GBuffer].texture, TCF_ResolveTargetable );

		// Normal_Metal GBuffer
		renderTargets[SRRT_Normal_Metal_GBuffer].texture		= GRHI->CreateTexture2D( TEXT( "Normal_Metal_GBuffer" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, 1, TCF_ResolveTargetable );
		renderTargets[SRRT_Normal_Metal_GBuffer].surface		= GRHI->CreateTargetableSurface( TEXT( "Normal_Metal_GBuffer" ), bufferSizeX, bufferSizeY, PF_A8R8G8B8, renderTargets[SRRT_Normal_Metal_GBuffer].texture, TCF_ResolveTargetable );
	}
}

void CSceneRenderTargets::BeginRenderingSceneColor( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetRenderTarget( InDeviceContextRHI, renderTargets[SRTT_SceneColor].surface, renderTargets[SRTT_SceneDepthZ].surface );
}

void CSceneRenderTargets::FinishRenderingSceneColor( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{}

void CSceneRenderTargets::BeginRenderingGBuffer( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetRenderTarget( InDeviceContextRHI, renderTargets[SRRT_Diffuse_Roughness_GBuffer].surface, renderTargets[SRTT_SceneDepthZ].surface );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, renderTargets[SRRT_Normal_Metal_GBuffer].surface, 1 );
}

void CSceneRenderTargets::FinishRenderingGBuffer( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, nullptr, 0 );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, nullptr, 1 );
}

void CSceneRenderTargets::ClearGBufferTargets( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	InDeviceContextRHI->ClearSurface( renderTargets[SRRT_Diffuse_Roughness_GBuffer].surface, CColor::black );
	InDeviceContextRHI->ClearSurface( renderTargets[SRRT_Normal_Metal_GBuffer].surface, CColor::black );
}

void CSceneRenderTargets::ResolveLightAttenuationDepth( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->CopyToResolveTarget( InDeviceContextRHI, renderTargets[SRTT_SceneDepthZ].surface, SResolveParams( SResolveRect(), renderTargets[SRRT_LightAttenuationDepthZ].texture ) );
}

void CSceneRenderTargets::BeginRenderingLightAttenuation( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetRenderTarget( InDeviceContextRHI, renderTargets[SRTT_SceneColor].surface, renderTargets[SRTT_SceneDepthZ].surface );
}

void CSceneRenderTargets::FinishRenderingLightAttenuation( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{}

void CSceneRenderTargets::ReleaseRHI()
{
	for ( uint32 index = 0; index < SRTT_MaxSceneRenderTargets; ++index )
	{
		SSceneRenderTargetItem&		rtItem = renderTargets[index];
		rtItem.surface.SafeRelease();
		rtItem.texture.SafeRelease();
	}
}

void CSceneRenderTargets::Allocate( uint32 InNewSizeX, uint32 InNewSizeY )
{
	if ( bufferSizeX < InNewSizeX || bufferSizeY < InNewSizeY )
	{
		bufferSizeX = InNewSizeX;
		bufferSizeY = InNewSizeY;
		UpdateRHI();
	}
}