/**
 * BasePassPixelShader.hlsl: Pixel shader code for base pass shader.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "BasePassCommon.hlsl"

// Output of pixel shader
struct PS_OUT
{
	float4	diffuseRoughness	: SV_TARGET0;
	float4	normalMetal			: SV_TARGET1;
};

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

void MainPS( VS_OUT In, out PS_OUT Out )
{
	float4 	diffuseColor = diffuseTexture.Sample( diffuseSampler, In.texCoord0 );
	if ( diffuseColor.a < 0.5f )
	{
		discard;
	}

	Out.diffuseRoughness.a		= roughnessTexture.Sample( roughnessSampler, In.texCoord0 ).a;
	Out.diffuseRoughness.rgb	= diffuseColor.rgb
	#if WITH_EDITOR
		+ In.colorOverlay
	#endif // WITH_EDITOR
		;

	Out.normalMetal.rgb 		= normalTexture.Sample( normalSampler, In.texCoord0 ).rgb;
	Out.normalMetal.a			= metallicTexture.Sample( metallicSampler, In.texCoord0 ).a;
}
