#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "CPP_GlobalConstantBuffers.hlsl"

struct FVertexFactoryInput
{
	float4 		position		: POSITION;
	float2 		texCoord0		: TEXCOORD0;
	float4		normal			: NORMAL0;
	float4		tangent			: TANGENT0;
	float4		binormal		: BINORMAL0;
};

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
	return mul( viewProjectionMatrix, InInput.position );
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return float2( InInput.texCoord0.x, -InInput.texCoord0.y );
}

#endif // !VERTEXFACTORY_H