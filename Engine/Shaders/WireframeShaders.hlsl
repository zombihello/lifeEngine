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
	float4 color		: TEXCOORD1;
};

float4		wireframeColor;

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	Out.color		= VertexFactory_GetColor( In, 0 );
	OutPosition		= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}

float4 MainPS( VS_OUT In ) : SV_Target
{
	return wireframeColor * In.color;
}