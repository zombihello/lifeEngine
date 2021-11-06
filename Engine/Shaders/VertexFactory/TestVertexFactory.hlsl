#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "CPP_GlobalConstantBuffers.hlsl"

struct FVertexFactoryInput
{
	float3 position		: POSITION;
	float2 texCoord0	: TEXCOORD0;
};

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return mul( viewProjectionMatrix, float4( InInput.position, 1.f ) );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return InInput.texCoord0;
}

#endif // !VERTEXFACTORY_H