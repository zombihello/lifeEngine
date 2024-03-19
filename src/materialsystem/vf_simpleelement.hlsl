#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "common.hlsl"

struct VertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
	float4		color			: COLOR;
}

float4 VertexFactory_GetLocalPosition( VertexFactoryInput input )
{
	return input.position;
}

float2 VertexFactory_GetTexCoord( VertexFactoryInput input, uint texCoordIndex )
{
	return input.texCoord0;
}

float4 VertexFactory_GetColor( VertexFactoryInput input, uint colorIndex )
{
	return input.color;
}

#endif // !VERTEXFACTORY_H