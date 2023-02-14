/**
 * DepthOnlyPixelShader.hlsl: Pixel shader code for depth only render.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */
 
#include "Common.hlsl"
#include "VertexFactory.hlsl"

float4 MainPS() : SV_Target
{
	return float4( 0.f, 0.f, 0.f, 0.f );
}