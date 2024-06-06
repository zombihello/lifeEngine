/**
 * BloomPixelShader.hlsl: Pixel shader code to calculate bright pixels for bloom.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"

Texture2D		sceneColor			: register( t0 );
SamplerState	sceneColorSampler	: register( s0 );
float3          brightThreshold;

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
    // Get pixel color from buffer
    float4		color = sceneColor.Sample( sceneColorSampler, InUV );

    // Check whether the one is higher than threshold, if so it as brightness color
    float       brightness = dot( color.rgb, brightThreshold );
    if ( brightness > 1.f )
    {
        return float4( color.rgb, 1.f );
    }
    
    // Otherwise is black
    return float4( 0.f, 0.f, 0.f, 0.f );
}