/**
 * LightingShaders.hlsl: Vertex shader code for calculating lights.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"
#include "CPP_GlobalConstantBuffers.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out nointerpolation SLightData OutLightData, out float4 OutPosition : SV_POSITION )
{
	OutLightData 	= VertexFactory_GetLightData( In );
	OutPosition		= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}