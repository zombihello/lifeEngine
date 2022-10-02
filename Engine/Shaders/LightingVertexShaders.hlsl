/**
 * LightingShaders.hlsl: Vertex shader code for calculating lights.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out nointerpolation SLightData OutLightData, out float4 OutScreenPosition : TEXCOORD0, out float4 OutPosition : SV_POSITION )
{
	OutLightData 		= VertexFactory_GetLightData( In );
	OutPosition			= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
	OutScreenPosition	= OutPosition;
}