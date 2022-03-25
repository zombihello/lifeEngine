#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position				: POSITION;
	float2 		texCoord0				: TEXCOORD0;
	float4		normal					: NORMAL0;
	
#if USE_INSTANCING
	float4x4 	instanceLocalToWorld 	: POSITION1;
#endif // USE_INSTANCING
};

float4		textureRect;
float2		spriteSize;

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
#if USE_INSTANCING
	return MulMatrix( InInput.instanceLocalToWorld, InInput.position * float4( spriteSize, 1.f, 1.f ) );
#else
	return MulMatrix( localToWorldMatrix, InInput.position * float4( spriteSize, 1.f, 1.f ) );
#endif // USE_INSTANCING
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return textureRect.xy + ( InInput.texCoord0 * textureRect.zw );
}

#endif // !VERTEXFACTORY_H