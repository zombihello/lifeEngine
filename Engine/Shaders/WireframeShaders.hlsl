/**
 * WireframeVertexShader.hlsl: Vertex and pixel shader code for wireframe render.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"
#include "VertexFactory.hlsl"

struct VS_OUT
{
	float4 color		: COLOR0;

#if WITH_EDITOR
	float4 colorOverlay	: COLOR1;
#endif // WITH_EDITOR
};

float4		wireframeColor;

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
#if WITH_EDITOR
	Out.colorOverlay	= VertexFactory_GetColorOverlay( In );
#endif // WITH_EDITOR

	Out.color			= VertexFactory_GetColor( In, 0 );
	OutPosition			= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
}

float4 MainPS( VS_OUT In ) : SV_Target
{
	return
		#if WITH_EDITOR
			lerp( wireframeColor, In.colorOverlay, length( In.colorOverlay ) )
		#else
			wireframeColor
		#endif // WITH_EDITOR
			* In.color;
}