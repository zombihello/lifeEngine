#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
	float4		color			: COLOR;
};

float4 VertexFactory_GetLocalPosition( FVertexFactoryInput InInput )
{
	return InInput.position;
}

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalPosition( InInput ) );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return InInput.texCoord0;
}

float4 VertexFactory_GetColor( FVertexFactoryInput InInput, uint InColorIndex )
{
	return InInput.color;
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