/**
 * ResolveVertexShader.hlsl: Resolve vertex shader source
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"

void MainVS( in float2 InPosition : POSITION, in float2 InUV : TEXCOORD0, out float2 OutUV : TEXCOORD0, out float4 OutPosition : SV_POSITION )
{
	OutPosition = float4( InPosition, 0, 1 );
    OutUV 		= InUV;
}
