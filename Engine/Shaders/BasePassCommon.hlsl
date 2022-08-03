/**
 * BasePassCommon.hlsl: Common shader code for base pass shaders.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEPASSCOMMON_HLSL
#define BASEPASSCOMMON_HLSL 0

struct VS_OUT
{
	float2 texCoord0	: TEXCOORD0;
	
#if WITH_EDITOR
	float4 colorOverlay	: COLOR0;
#endif // WITH_EDITOR
};

#endif // !BASEPASSCOMMON_HLSL