/**
 * HitProxyShaders.hlsl: Vertex and pixel shader code for hit proxy id render.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */
 
#include "Common.hlsl"
#include "VertexFactory.hlsl"

struct VS_OUT
{
	nointerpolation float4 hitProxyId : COLOR0;
};

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	Out.hitProxyId	= VertexFactory_GetHitProxyId( In );
	OutPosition		= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}

float4 MainPS( VS_OUT In ) : SV_Target
{
	return In.hitProxyId;
}