/**
 * PostProcessPixelShader.hlsl: Pixel shader code for post process.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"

Texture2D		sceneColor			: register( t0 );
SamplerState	sceneColorSampler	: register( s0 );
Texture2D		bloom				: register( t1 );
SamplerState	bloomSampler		: register( s1 );

#if APPLY_HDR
float			hdrExposure;

float3 ACESToneMap( float3 InColor )
{
	float3		result;
	float3x3	inputMatrix =
	{
		0.59719f, 0.07600f, 0.02840f,
		0.35458f, 0.90834f, 0.13383f,
		0.04823f, 0.01566f, 0.83777f
	};
	float3x3	outputMatrix =
	{
		1.60475f, -0.10208f, -0.00327f,
		-0.53108f, 1.10813f, -0.07276f,
		-0.07367f, -0.00605f, 1.07602f
	};
	float3		unitVector = { 1.f, 1.f, 1.f };
	float3		v = MulMatrix( InColor, inputMatrix );
	float3		a = v * ( v + unitVector * 0.0245786f ) - unitVector * 0.000090537f;
	float		b = v * ( 0.983729f * v + unitVector * 0.4329510f ) + unitVector * 0.238081f;
	
	v = MulMatrix( a / b, outputMatrix );
	result = saturate( v );
	return result;
}
#endif // APPLY_HDR

#if APPLY_GAMMA_CORRECTION
float			gamma;
#endif // APPLY_GAMMA_CORRECTION

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
	float4		color = sceneColor.Sample( sceneColorSampler, InUV ) + bloom.Sample( bloomSampler, InUV );

	// Apply HDR tonemapping and gamma correction if it need
#if APPLY_HDR
	color.xyz   = float3( 1.f, 1.f, 1.f ) - exp( -color.xyz * hdrExposure );
	color.xyz	= ACESToneMap( color.xyz );
#endif // APPLY_HDR

#if APPLY_GAMMA_CORRECTION
	float		gammaFactor = 1.f / gamma;
	color.xyz 	= pow( color.xyz, float3( gammaFactor, gammaFactor, gammaFactor ) ); 
#endif // APPLY_GAMMA_CORRECTION

	color.a = 1.f;		// BS yehor.pohuliaka - Maybe need add macro for ignore alpha channel?
	return color;
}