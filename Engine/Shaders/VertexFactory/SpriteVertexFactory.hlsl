#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
	float4		normal			: NORMAL0;
};

float4		textureRect;
float2		spriteSize;

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return MulMatrix( localToWorldMatrix, InInput.position * float4( spriteSize, 1.f, 1.f ) );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return textureRect.xy + ( InInput.texCoord0 * textureRect.zw );
}

#endif // !VERTEXFACTORY_H