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
	OutPosition	= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}