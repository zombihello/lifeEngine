#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
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
	return float4( 1.f, 1.f, 1.f, 1.f );
}

#endif // !VERTEXFACTORY_H