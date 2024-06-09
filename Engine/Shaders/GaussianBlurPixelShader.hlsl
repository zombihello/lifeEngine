/**
 * GaussianBlurPixelShader.hlsl: Pixel shader code to gaussian blur.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"

Texture2D		        buffer			: register( t0 );
SamplerState	        bufferSampler	: register( s0 );
float2                  texOffset;

static const float      blurWeight[5]   = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

float4 MainPS( float2 InUV : TEXCOORD0 ) : SV_Target
{
    float3      result = buffer.SampleLevel( bufferSampler, InUV, 0.f ).rgb * blurWeight[0];
    for ( int index = 1; index < 5; ++index )
    {
       // result*= float3( ( float )index+texOffset.x, 1, 1 );
#if HORIZONTAL_BLUR
        // Horizontal blur
        result += buffer.SampleLevel( bufferSampler, InUV + float2( index * texOffset.x, 0.f ), 0.f ).rgb * blurWeight[index];
        result += buffer.SampleLevel( bufferSampler, InUV - float2( index * texOffset.x, 0.f ), 0.f ).rgb * blurWeight[index];
#else
        // Vertical blur
        result += buffer.SampleLevel( bufferSampler, InUV + float2( 0.f, index * texOffset.y ), 0.f ).rgb * blurWeight[index];
        result += buffer.SampleLevel( bufferSampler, InUV - float2( 0.f, index * texOffset.y ), 0.f ).rgb * blurWeight[index];
#endif // HORIZONTAL_BLUR
    }

    return float4( result.rgb, 1.f );
}