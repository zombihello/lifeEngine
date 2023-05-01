/**
 * Common.hlsl: Common shader code.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMMON_HLSL
#define COMMON_HLSL 0

#include "Definitions.hlsl"

#define PI                          3.14159265359
#define MulMatrix( Mtx, Vect )	    mul( Mtx, Vect )
#define MulBone( Mtx, Vect )		mul( Vect, Mtx )

// Pixel and vertex shader constant registers that are reserved by the Engine.
// ----------------------------------------------------------------------------------------
#define SOB_GlobalConstants			b1			// Slot for global constant buffer

#include "CPP_GlobalConstantBuffers.hlsl"

// Struct of light data (color, radius, etc)
struct SLightData
{
    float4      lightColor      : COLOR0;
    float4      specularColor   : COLOR1;
    float       intensivity     : BLENDWEIGHT0;

#if POINT_LIGHT
    float3      position        : POSITION5;
    float       radius          : BLENDWEIGHT1;
#elif SPOT_LIGHT
#elif DIRECTIONAL_LIGHT
#endif // POINT_LIGHT / SPOT_LIGHT / DIRECTIONAL_LIGHT
};

/**
 * Returns the size of the screen/view (in pixels, like (1280,720))
 */
float2 GetScreenSize()
{
	return screenAndBufferSize.xy;
}

/**
 * Returns the size of a render target (in pixels, like (1280,720))
 */
float2 GetBufferSize()
{
	return screenAndBufferSize.zw;
}

#endif // !COMMON_HLSL