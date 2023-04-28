/**
 * PostProcessPixelShader.hlsl: Pixel shader code for post process.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

Texture2D		sceneColor			: register( t0 );
SamplerState	sceneColorSampler	: register( s0 );

#if APPLY_HDR
	float			hdrExposure;
#endif // APPLY_HDR

#if APPLY_GAMMA_CORRECTION
	float			gamma;
#endif // APPLY_GAMMA_CORRECTION

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
	float4		color = sceneColor.Sample( sceneColorSampler, InUV );

	// Apply HDR tonemapping and gamma correction if it need
#if APPLY_HDR
	color.xyz   = float3( 1.f, 1.f, 1.f ) - exp( -color.xyz * hdrExposure );
#endif // APPLY_HDR

#if APPLY_GAMMA_CORRECTION
	float		gammaFactor = 1.f / gamma;
	color.xyz 	= pow( color.xyz, float3( gammaFactor, gammaFactor, gammaFactor ) ); 
#endif // APPLY_GAMMA_CORRECTION

	color.a = 1.f;		// BS yehor.pohuliaka - Maybe need add macro for ignore alpha channel?
	return color;
}