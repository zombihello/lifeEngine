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

float DistributionGGX( float3 InNormal, float3 InViewLightDirection, float InRoughness )
{
    float roughness2    = pow( pow( InRoughness, 2.f ), 2.f );
    float NdotH2        = pow( max( dot( InNormal, InViewLightDirection ), 0.f ), 2.f );
    float denom         = NdotH2 * ( roughness2 - 1.f ) + 1.f;
    return roughness2 / ( PI * pow( denom, 2.f ) );
}

float GeometrySchlickGGX( float InNdotV, float InRoughness )
{
    float coefficient     = pow( InRoughness + 1.f, 2.f ) / 8.f;
    return InNdotV / ( InNdotV * ( 1.f - coefficient ) + coefficient );
}

float GeometrySmith( float3 InNormal, float3 InViewDirection, float3 InLightDirection, float InRoughness )
{
    float NdotV     = max( dot( InNormal, InViewDirection ), 0.f );
    float NdotL     = max( dot( InNormal, InLightDirection ), 0.f );
    return GeometrySchlickGGX( NdotL, InRoughness ) * GeometrySchlickGGX( NdotV, InRoughness );
}

float3 FresnelSchlick( float InCosTheta, float3 InF0 )
{
    return InF0 + ( 1.f - InF0 ) * pow( clamp( 1.f - InCosTheta, 0.f, 1.f ), 5.f );
}

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
    float4      emissionAO          = emissionGBufferTexture.Sample( emissionGBufferSampler, bufferUV );

    // Getting postion of fragment and view direction
    float3      positionFragment    = ReconstructPosition( screenUV, bufferUV );
    float3      viewDirection       = normalize( position.xyz - positionFragment );

#if POINT_LIGHT
    // Calculate reflectance at normal incidence
    // If dia-electric (like plastic) use F0 of 0.04 
    // And if it's a metal, use the albedo color as F0 (metallic workflow)    
    float3      f0 = float3( 0.04f, 0.04f, 0.04f ); 
    f0          = lerp( f0, diffuseRoughness.xyz, normalMetal.w );

    // Calculate per-light radiance
    float3      lightDirection      = InLightData.position - positionFragment;
    float       distance            = length( lightDirection );
    lightDirection                  = normalize( lightDirection );
    float3      viewLightDirection  = normalize( viewDirection + lightDirection );
    float       attenuation         = pow( saturate( 1.f - pow( distance / InLightData.radius, 4.f ) ), 2.f ) / ( pow( distance, 2.f ) + 1.f );
    float3      radiance            = InLightData.lightColor * InLightData.intensivity * attenuation;

     // Cook-Torrance BRDF
    float       ggx                 = DistributionGGX( normalMetal.xyz, viewLightDirection, diffuseRoughness.w );   
    float       geometry            = GeometrySmith( normalMetal.xyz, viewDirection, lightDirection, diffuseRoughness.w );      
    float3      fresnel             = FresnelSchlick( max( dot( viewLightDirection, viewDirection ), 0.f ), f0 );
    float3      specular            = ( ggx * geometry * fresnel ) / ( 4.f * max( dot( normalMetal.xyz, viewDirection ), 0.f ) * max( dot( normalMetal.xyz, lightDirection ), 0.f) + 0.0001f ); // +0.0001 to prevent divide by zero

    // kS is equal to Fresnel
    float3      kS                  = fresnel;
    
    // For energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS
    float3      kD                  = float3( 1.f, 1.f, 1.f ) - kS;
    
    // Multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals have no diffuse light)
    kD *= 1.0 - normalMetal.w;	  

    // Scale light by NdotL
    float       NdotL               = max( dot( normalMetal.xyz, lightDirection ), 0.f );        

    // Reflectance equation
    float3      reflectance         = ( kD * diffuseRoughness.xyz / PI + specular ) * radiance * NdotL;     // Note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    // Ambient lighting
    // TODO BS yehor.pohuliaka - Need replace this ambient lighting with environment lighting
    float3      ambient             = emissionAO.xyz * diffuseRoughness.xyz * emissionAO.w;
    float3      finalColor          = ambient + reflectance;
    
    // HDR tonemapping
    finalColor                      = float3( 1.f, 1.f, 1.f ) - exp( -finalColor * 1.f );
    
    // gamma correct
    finalColor = pow( finalColor, float3( 1.f / 2.2f, 1.f / 2.2f, 1.f / 2.2f ) ); 
    return float4( finalColor, 1.f );
#else
    return float4( positionFragment, 1.f );
#endif // POINT_LIGHT
}