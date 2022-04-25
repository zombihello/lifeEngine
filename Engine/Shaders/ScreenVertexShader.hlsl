/**
 * ScreenVertexShader.hlsl: Vertex shader code for render in screen space.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out float2 OutUV : TEXCOORD0, out float4 OutPosition : SV_POSITION )
{
	OutUV	        = VertexFactory_GetTexCoord( In, 0 );
	OutPosition		= VertexFactory_GetLocalPosition( In );
}

void FullscreenMainVS( uint InVertexID : SV_VertexID, out float2 OutUV : TEXCOORD0, out float4 OutPosition : SV_POSITION ) 
{
	OutUV 			= float2( ( InVertexID << 1 ) & 2, InVertexID & 2 );
	OutPosition 	= float4( OutUV * float2( 2.0, -2.0 ) + float2( -1.0, 1.0 ), 0.0, 1.0 );
}