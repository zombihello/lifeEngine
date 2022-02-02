#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "CPP_GlobalConstantBuffers.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
};

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return mul( viewProjectionMatrix, InInput.position );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return float2( 0.f, 0.f );
}

#endif // !VERTEXFACTORY_H