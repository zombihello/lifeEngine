/**
 * Common.hlsl: Common shader code.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMMON_HLSL
#define COMMON_HLSL 0

#define MulMatrix( Mtx, Vect )	    mul( Mtx, Vect )
#define MulBone( Mtx, Vect )		mul( Vect, Mtx )

// Pixel and vertex shader constant registers that are reserved by the Engine.
// ----------------------------------------------------------------------------------------
#define SOB_GlobalConstants			b1			// Slot for global constant buffer

#endif // !COMMON_HLSL