/**
 * BasePassPixelShader.hlsl: Pixel shader code for base pass shader.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "BasePassCommon.hlsl"

Texture2D		diffuse			: register( t0 );
SamplerState	samplerLine		: register( s0 );

float4 MainPS( VS_OUT In ) : SV_Target
{
    return diffuse.Sample( samplerLine, In.texCoord0 );
}