/**
 * BasePassVertexShader.hlsl: Vertex shader code for base pass shader.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "BasePassCommon.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	OutPosition		= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
	Out.texCoord0	= VertexFactory_GetTexCoord( In, 0 );
}