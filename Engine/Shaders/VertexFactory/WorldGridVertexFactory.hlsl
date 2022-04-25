#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
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
	return float2( 0.f, 0.f );
}

float4 VertexFactory_GetColor( FVertexFactoryInput InInput, uint InColorIndex )
{
	return InInput.color;
}

#endif // !VERTEXFACTORY_H