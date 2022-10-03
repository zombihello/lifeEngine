/**
 * ResolvePixelShader.hlsl: Resolve pixel shader source
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"

// This is only used to resolve a partial depth surface, like the shadow cache
Texture2D<float> 	unresolvedSurface;

void MainDepth( in float2 InUV : TEXCOORD0, out float OutDepth : SV_Depth )
{
	int2 	intUV 	= trunc( InUV );
	OutDepth 		= unresolvedSurface[ intUV ].x;
}