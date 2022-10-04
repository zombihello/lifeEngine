/**
 * LightingShaders.hlsl: Pixel shader code for calculating lights.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"
#include "VertexFactory.hlsl"

// DiffuseRoughnessGBuffer texture
Texture2D		diffuseRoughnessGBufferTexture;
SamplerState	diffuseRoughnessGBufferSampler;

// NormalMetalGBuffer texture
Texture2D		normalMetalGBufferTexture;
SamplerState	normalMetalGBufferSampler;

// EmissionGBuffer texture
Texture2D       emissionGBufferTexture;
SamplerState    emissionGBufferSampler;

// Depth buffer texture
Texture2D		depthBufferTexture;
SamplerState	depthBufferSampler;

float3 ReconstructPosition( float2 InScreenUV, float2 InBufferUV ) 
{
    // Getting depth from depth buffer
    float       depth = depthBufferTexture.Sample( depthBufferSampler, InBufferUV ).x;

    // Inverting Y coord, because in DirectX screen's coord Y starting from Up of the viewport
    InScreenUV.y = 1.f - InScreenUV.y;

    // Restore world position
    float4      worldPosition = MulMatrix( invViewProjectionMatrix, float4( InScreenUV * 2.f - 1.f, depth, 1.f ) );
    return worldPosition.xyz / worldPosition.w;
}

// Main function for calculate light attenuation
float4 MainPS( in nointerpolation SLightData InLightData, in float4 InScreenPosition : TEXCOORD0 ) : SV_TARGET0
{
    // Calculate screen UV
    float2      screenUV            = InScreenPosition.xy / InScreenPosition.w * float2( 0.5f, -0.5f ) + float2( 0.5f, 0.5f );
    float2      bufferUV            = screenUV * ( GetScreenSize() / GetBufferSize() );

    // Getting data from GBuffer
    float4      diffuseRoughness    = diffuseRoughnessGBufferTexture.Sample( diffuseRoughnessGBufferSampler, bufferUV );
    float4      normalMetal         = normalMetalGBufferTexture.Sample( normalMetalGBufferSampler, bufferUV );
    float4      emission            = emissionGBufferTexture.Sample( emissionGBufferSampler, bufferUV );

    // Getting postion of fragment and view direction
    float3      positionFragment    = ReconstructPosition( screenUV, bufferUV );
    float3      viewDirection       = normalize( position.xyz - positionFragment );

#if POINT_LIGHT
    float3      lightDirection      = InLightData.position - positionFragment;
    float       distance            = length( lightDirection  );
    lightDirection                  = normalize( lightDirection  );


    float3	    halfwayDirection    = normalize( lightDirection + viewDirection ); 		
	float 	    NdotL               = max( dot( normalMetal.xyz, lightDirection ), 0.f ); 
    float       attenuation         = pow( saturate( 1.f - pow( distance / InLightData.radius, 4.f ) ), 2.f ) / ( pow( distance, 2.f ) + 1.f );
    float	    specularFactor      = max( pow( dot( reflect( -lightDirection, normalMetal.xyz ), viewDirection ), diffuseRoughness.a ) * normalMetal.a, 0.f );

    return float4( diffuseRoughness.xyz, 1.f ) * (emission+(  InLightData.lightColor * InLightData.intensivity + InLightData.lightColor * specularFactor * InLightData.intensivity ) * attenuation * NdotL);
#else
    return float4( positionFragment, 1.f );
#endif // POINT_LIGHT
}