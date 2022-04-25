/**
 * ScreenPixelShader.hlsl: Pixel shader code for render in screen space.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

Texture2D		texture0			: register( t0 );
SamplerState	texture0Sampler		: register( s0 );

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
    return texture0.Sample( texture0Sampler, InUV );
}