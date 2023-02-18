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
		renderTargets[SRTT_SceneColor].Update( false, bufferSizeX, bufferSizeY, PF_A8R8G8B8, 0, TEXT( "SceneColor" ) );

		// Scene depth Z
		renderTargets[SRTT_SceneDepthZ].Update( false, bufferSizeX, bufferSizeY, PF_DepthStencil, TCF_DepthStencil, TEXT( "SceneDepthZ" ) );

		// Light attenuation depth Z
		renderTargets[SRTT_LightPassDepthZ].Update( false, bufferSizeX, bufferSizeY, PF_DepthStencil, TCF_DepthStencil, TEXT( "LightPassDepthZ" ) );

#if ENABLE_HITPROXY
		// Hit proxy
		renderTargets[SRTT_HitProxies].Update( false, bufferSizeX, bufferSizeY, PF_A8R8G8B8, 0, TEXT( "HitProxy" ) );
#endif // ENABLE_HITPROXY

		// Diffuse_Roughness GBuffer
		renderTargets[SRTT_Diffuse_Roughness_GBuffer].Update( false, bufferSizeX, bufferSizeY, PF_A8R8G8B8, 0, TEXT( "Diffuse_Roughness_GBuffer" ) );

		// Normal_Metal GBuffer
		renderTargets[SRTT_Normal_Metal_GBuffer].Update( false, bufferSizeX, bufferSizeY, PF_FloatRGBA, 0, TEXT( "Normal_Metal_GBuffer" ) );

		// Emission GBuffer
		renderTargets[SRTT_Emission_GBuffer].Update( false, bufferSizeX, bufferSizeY, PF_A8R8G8B8, 0, TEXT( "Emission_GBuffer" ) );
	}
}

void CSceneRenderTargets::BeginRenderingSceneColor( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetRenderTarget( InDeviceContextRHI, renderTargets[SRTT_SceneColor].GetSurfaceRHI(), renderTargets[SRTT_SceneDepthZ].GetSurfaceRHI() );
}

void CSceneRenderTargets::FinishRenderingSceneColor( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{}

void CSceneRenderTargets::BeginRenderingPrePass( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );

	// Set the scene depth surface and a DUMMY buffer or NULL buffer as color buffer 
	// (In case using DUMMY buffer him must be the same size as the depth buffer)
	GRHI->SetRenderTarget( InDeviceContextRHI, nullptr, renderTargets[SRTT_SceneDepthZ].GetSurfaceRHI() );

	// Disable color writes since we only want z depths
	GRHI->SetColorWriteEnable( InDeviceContextRHI, false );
}

void CSceneRenderTargets::FinishRenderingPrePass( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	// Re-enable color writes
	check( InDeviceContextRHI );
	GRHI->SetColorWriteEnable( InDeviceContextRHI, true );
}

void CSceneRenderTargets::BeginRenderingGBuffer( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetRenderTarget( InDeviceContextRHI, renderTargets[SRTT_Diffuse_Roughness_GBuffer].GetSurfaceRHI(), renderTargets[SRTT_SceneDepthZ].GetSurfaceRHI() );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, renderTargets[SRTT_Normal_Metal_GBuffer].GetSurfaceRHI(), 1 );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, renderTargets[SRTT_Emission_GBuffer].GetSurfaceRHI(), 2 );
}

void CSceneRenderTargets::FinishRenderingGBuffer( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, nullptr, 1 );
	GRHI->SetMRTRenderTarget( InDeviceContextRHI, nullptr, 2 );
}

void CSceneRenderTargets::ClearGBufferTargets( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	InDeviceContextRHI->ClearSurface( renderTargets[SRTT_Diffuse_Roughness_GBuffer].GetSurfaceRHI(), CColor::black );
	InDeviceContextRHI->ClearSurface( renderTargets[SRTT_Normal_Metal_GBuffer].GetSurfaceRHI(), CColor::black );
	InDeviceContextRHI->ClearSurface( renderTargets[SRTT_Emission_GBuffer].GetSurfaceRHI(), CColor::black );
}

void CSceneRenderTargets::ResolveLightPassDepth( class CBaseDeviceContextRHI* InDeviceContextRHI ) const
{
	check( InDeviceContextRHI );
	GRHI->CopyToResolveTarget( InDeviceContextRHI, renderTargets[SRTT_SceneDepthZ].GetSurfaceRHI(), SResolveParams( SResolveRect(), renderTargets[SRTT_LightPassDepthZ].GetTexture2DRHI() ) );
}

void CSceneRenderTargets::ReleaseRHI()
{
	for ( uint32 index = 0; index < SRTT_MaxSceneRenderTargets; ++index )
	{
		renderTargets[index].Update( true, 0, 0, PF_Unknown );
	}
}

void CSceneRenderTargets::Allocate( uint32 InNewSizeX, uint32 InNewSizeY )
{
	if ( bufferSizeX < InNewSizeX || bufferSizeY < InNewSizeY )
	{
		bufferSizeX = InNewSizeX;
		bufferSizeY = InNewSizeY;
		BeginUpdateResource( this );
	}
}