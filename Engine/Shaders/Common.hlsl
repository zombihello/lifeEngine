/**
 * Common.hlsl: Common shader code.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMMON_HLSL
#define COMMON_HLSL 0

#include "Definitions.hlsl"

#define MulMatrix( Mtx, Vect )	    mul( Mtx, Vect )
#define MulBone( Mtx, Vect )		mul( Vect, Mtx )

// Pixel and vertex shader constant registers that are reserved by the Engine.
// ----------------------------------------------------------------------------------------
#define SOB_GlobalConstants			b1			// Slot for global constant buffer

// Struct of light data (color, radius, etc)
struct SLightData
{
#if POINT_LIGHT
    float4      color : COLOR;
#elif SPOT_LIGHT
    float4      color : COLOR;
#elif DIRECTIONAL_LIGHT
    float4      color : COLOR;
#endif // POINT_LIGHT / SPOT_LIGHT / DIRECTIONAL_LIGHT
};

#endif // !COMMON_HLSL