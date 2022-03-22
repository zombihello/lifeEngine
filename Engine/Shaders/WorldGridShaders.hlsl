/**
 * WorldGridShaders.hlsl: Vertex and pixel shader for render world grid in WorldEd.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out float4 OutPosition : SV_POSITION )
{
	OutPosition	= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}

float4 MainPS() : SV_Target
{
    return float4( 0.501f, 0.501f, 0.501f, 1.f );
}
