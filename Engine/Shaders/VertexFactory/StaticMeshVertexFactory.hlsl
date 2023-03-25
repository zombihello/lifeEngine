#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
	float4		normal			: NORMAL0;
	float4		tangent			: TANGENT0;
	float4		binormal		: BINORMAL0;
};

float4 VertexFactory_GetLocalPosition( FVertexFactoryInput InInput )
{
	return InInput.position;
}

float4 VertexFactory_GetLocalNormal( FVertexFactoryInput InInput )
{
	return InInput.normal;
}

float4 VertexFactory_GetLocalTangent( FVertexFactoryInput InInput )
{
	return InInput.tangent;
}

float4 VertexFactory_GetLocalBinormal( FVertexFactoryInput InInput )
{
	return InInput.binormal;
}

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalPosition( InInput ) );
}

float4 VertexFactory_GetWorldNormal( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalNormal( InInput ) );
}

float4 VertexFactory_GetWorldTangent( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalTangent( InInput ) );
}

float4 VertexFactory_GetWorldBinormal( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalBinormal( InInput ) );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return float2( InInput.texCoord0.x, -InInput.texCoord0.y );
}

float4 VertexFactory_GetColor( FVertexFactoryInput InInput, uint InColorIndex )
{
	return float4( 1.f, 1.f, 1.f, 1.f );
}

#if ENABLE_HITPROXY
float4 VertexFactory_GetHitProxyId( FVertexFactoryInput InInput )
{
	return hitProxyId;
}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
float4 VertexFactory_GetColorOverlay( FVertexFactoryInput InInput )
{
	return colorOverlay;
}
#endif // WITH_EDITOR

#endif // !VERTEXFACTORY_H