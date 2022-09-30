#ifndef VERTEXFACTORY_H
#define VERTEXFACTORY_H 0

#include "Common.hlsl"
#include "VertexFactory/VertexFactoryCommon.hlsl"

struct FVertexFactoryInput
{
	float4 		position					: POSITION;
	
#if POINT_LIGHT || SPOT_LIGHT
	float4x4	instanceLocalToWorld		: POSITION1;
#endif // POINT_LIGHT || SPOT_LIGHT

	SLightData	lightData;
};

float4 VertexFactory_GetLocalPosition( FVertexFactoryInput InInput )
{
	return InInput.position;
}

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
#if POINT_LIGHT || SPOT_LIGHT
	#if USE_INSTANCING
		return MulMatrix( InInput.instanceLocalToWorld, VertexFactory_GetLocalPosition( InInput ) );
	#else
		return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalPosition( InInput ) );
	#endif // USE_INSTANCING
#else
	return VertexFactory_GetLocalPosition( InInput );
#endif // POINT_LIGHT || SPOT_LIGHT
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	return float2( 0.f, 0.f );
}

float4 VertexFactory_GetColor( FVertexFactoryInput InInput, uint InColorIndex )
{
	return float4( 1.f, 1.f, 1.f, 1.f );
}

#if ENABLE_HITPROXY
float4 VertexFactory_GetHitProxyId( FVertexFactoryInput InInput )
{
	return float4( 0.f, 0.f, 0.f, 0.f );
}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
float4 VertexFactory_GetColorOverlay( FVertexFactoryInput InInput )
{
	return float4( 0.f, 0.f, 0.f, 0.f );
}
#endif // WITH_EDITOR

SLightData VertexFactory_GetLightData( FVertexFactoryInput InInput )
{
	return InInput.lightData;
}

#endif // !VERTEXFACTORY_H