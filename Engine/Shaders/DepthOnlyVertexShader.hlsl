/**
 * DepthOnlyVertexShader.hlsl: Vertex shader code for depth only render.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */
 
#include "Common.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out float4 OutPosition : SV_POSITION )
{
#if WORLD_POS
	OutPosition	= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
#else
	OutPosition = VertexFactory_GetLocalPosition( In );
#endif // WORLD_POS
}