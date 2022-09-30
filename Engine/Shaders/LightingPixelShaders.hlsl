/**
 * LightingShaders.hlsl: Pixel shader code for calculating lights.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "VertexFactory.hlsl"

// DiffuseRoughnessGBuffer texture
Texture2D		diffuseRoughnessGBufferTexture;
SamplerState	diffuseRoughnessGBufferSampler;

// NormalMetalGBuffer texture
Texture2D		normalMetalGBufferTexture;
SamplerState	normalMetalGBufferSampler;

float4 MainPS( nointerpolation SLightData InLightData ) : SV_TARGET0
{
    return InLightData.color;
}