/**
 * TheoraPixelShader.hlsl: Pixel shader code for render theora movie.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

Texture2D		texture0			: register( t0 );
SamplerState	texture0Sampler		: register( s0 );

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
	float4		color 	= float4( 1.f, 1.f, 1.f, 1.f );
	float3		yuv 	= texture0.Sample( texture0Sampler, InUV ).xyz - float3( 0.f, 0.5f, 0.5f );
    
	// Convert from YUV444 to RGBA
	yuv.x      	= 1.1643f * ( yuv.x - 0.0625f );
	color.x 	= yuv.x + 1.5958f  * yuv.z;
	color.y 	= yuv.x - 0.39173f * yuv.y - 0.81290f * yuv.z;
	color.z 	= yuv.x + 2.017f   * yuv.y;
	
	// Return final color
	return color;
}