/**
 * WireframeVertexShader.hlsl: Vertex and pixel shader code for wireframe render.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "VertexFactory.hlsl"

struct VS_OUT
{
	float2 texCoord0	: TEXCOORD0;
};

float4		wireframeColor;

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	OutPosition		= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
	Out.texCoord0	= VertexFactory_GetTexCoord( In, 0 );
}

float4 MainPS( VS_OUT In ) : SV_Target
{
	return wireframeColor;
}
