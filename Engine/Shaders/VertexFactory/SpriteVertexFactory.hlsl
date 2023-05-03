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
	
	#if ENABLE_HITPROXY
		float4		hitProxyId			: COLOR0;
	#endif // ENABLE_HITPROXY
	
	#if WITH_EDITOR
		float4		colorOverlay		: COLOR1;
	#endif // WITH_EDITOR
#endif // USE_INSTANCING
};

bool		bFlipVertical;
bool		bFlipHorizontal;
float4		textureRect;
float2		spriteSize;

float4 VertexFactory_GetLocalPosition( FVertexFactoryInput InInput )
{
	return InInput.position * float4( spriteSize / 2.f, 1.f, 1.f );
}

float4 VertexFactory_GetLocalNormal( FVertexFactoryInput InInput )
{
	return InInput.normal;
}

float4 VertexFactory_GetLocalTangent( FVertexFactoryInput InInput )
{
	return float4( 1.f, 0.f, 0.f, 0.f );
}

float4 VertexFactory_GetLocalBinormal( FVertexFactoryInput InInput )
{
	return float4( 0.f, 0.f, 1.f, 0.f );
}

float4 VertexFactory_GetWorldPosition( FVertexFactoryInput InInput )
{
#if USE_INSTANCING
	return MulMatrix( InInput.instanceLocalToWorld, VertexFactory_GetLocalPosition( InInput ) );
#else
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalPosition( InInput ) );
#endif // USE_INSTANCING
}

float4 VertexFactory_GetWorldNormal( FVertexFactoryInput InInput )
{
#if USE_INSTANCING
	return MulMatrix( InInput.instanceLocalToWorld, VertexFactory_GetLocalNormal( InInput ) );
#else
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalNormal( InInput ) );
#endif // USE_INSTANCING
}

float4 VertexFactory_GetWorldTangent( FVertexFactoryInput InInput )
{
#if USE_INSTANCING
	return MulMatrix( InInput.instanceLocalToWorld, VertexFactory_GetLocalTangent( InInput ) );
#else
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalTangent( InInput ) );
#endif // USE_INSTANCING
}

float4 VertexFactory_GetWorldBinormal( FVertexFactoryInput InInput )
{
#if USE_INSTANCING
	return MulMatrix( InInput.instanceLocalToWorld, VertexFactory_GetLocalBinormal( InInput ) );
#else
	return MulMatrix( localToWorldMatrix, VertexFactory_GetLocalBinormal( InInput ) );
#endif // USE_INSTANCING
}

float2 VertexFactory_GetTexCoord( FVertexFactoryInput InInput, uint InTexCoordIndex )
{
	// TODO BS yehor.pohuliaka - Need fix fliping, because on tile texture is not correct work
	float2	outTexCoord = textureRect.xy + ( InInput.texCoord0 * textureRect.zw );	
	if ( bFlipVertical )
	{
		outTexCoord.y *= -1.f;
	}
	
	if ( bFlipHorizontal )
	{
		outTexCoord.x *= -1.f;
	}
	return outTexCoord;
}

float4 VertexFactory_GetColor( FVertexFactoryInput InInput, uint InColorIndex )
{
	return float4( 1.f, 1.f, 1.f, 1.f );
}

#if ENABLE_HITPROXY
float4 VertexFactory_GetHitProxyId( FVertexFactoryInput InInput )
{
	#if USE_INSTANCING
		return InInput.hitProxyId;
	#else
		return hitProxyId;
	#endif // USE_INSTANCING
}
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
float4 VertexFactory_GetColorOverlay( FVertexFactoryInput InInput )
{
	#if USE_INSTANCING
		return InInput.colorOverlay;
	#else
		return colorOverlay;
	#endif // USE_INSTANCING
}
#endif // WITH_EDITOR

#endif // !VERTEXFACTORY_H