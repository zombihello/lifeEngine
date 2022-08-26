/**
 * BasePassPixelShader.hlsl: Pixel shader code for base pass shader.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "BasePassCommon.hlsl"

// Diffuse texture
Texture2D		diffuseTexture;
SamplerState	diffuseSampler;

// Normal texture
Texture2D		normalTexture;
SamplerState	normalSampler;

// Metallic texture
Texture2D		metallicTexture;
SamplerState	metallicSampler;

// Roughness texture
Texture2D		roughnessTexture;
SamplerState	roughnessSampler;

float4 MainPS( VS_OUT In ) : SV_Target
{
	float4		outColor = diffuseTexture.Sample( diffuseSampler, In.texCoord0 );
	if ( outColor.a < 0.5f )
	{
		discard;
	}

    return outColor
#if WITH_EDITOR
		+ In.colorOverlay
#endif // WITH_EDITOR
		;
}
