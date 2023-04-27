/**
 * ScreenPixelShader.hlsl: Pixel shader code for render texture preview.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

Texture2D		texture0			: register( t0 );
SamplerState	texture0Sampler		: register( s0 );
float4			colorChannelMask;
uint            mipmapToView;

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
    float4      outColor = texture0.SampleLevel( texture0Sampler, InUV, mipmapToView ) * colorChannelMask;

    // Discard pixel if alpha channel less 0.01
    if ( colorChannelMask.a > 0.f && outColor.a < 0.01f )
	{
		discard;
	}

    return float4( outColor.xyz, 1.f );
}