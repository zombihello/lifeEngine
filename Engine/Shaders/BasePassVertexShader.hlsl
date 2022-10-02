/**
 * BasePassVertexShader.hlsl: Vertex shader code for base pass shader.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "Common.hlsl"
#include "BasePassCommon.hlsl"
#include "VertexFactory.hlsl"

void MainVS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	OutPosition			= MulMatrix( viewProjectionMatrix, VertexFactory_GetWorldPosition( In ) );
	Out.texCoord0		= VertexFactory_GetTexCoord( In, 0 );
	Out.normal			= VertexFactory_GetWorldNormal( In );

#if WITH_EDITOR
	Out.colorOverlay	= VertexFactory_GetColorOverlay( In );
#endif // WITH_EDITOR
}