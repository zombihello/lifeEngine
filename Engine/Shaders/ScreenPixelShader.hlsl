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
	float4		color = texture0.Sample( texture0Sampler, InUV );
    color.a = 1.f;		// BS yehor.pohuliaka - Maybe need add macro for ignore alpha channel?
	return color;
}