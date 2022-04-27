/**
 * SimpleElementVertexShader.hlsl: Vertex shader code for render simple elements.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#include "CPP_GlobalConstantBuffers.hlsl"
#include "VertexFactory.hlsl"

 void MainVS( in FVertexFactoryInput In, out float2 OutUV : TEXCOORD0, out float4 OutColor : TEXCOORD1, out float4 OutPosition : SV_POSITION )
 {
     OutUV              = VertexFactory_GetTexCoord( In, 0 );
     OutColor           = VertexFactory_GetColor( In, 0 );
     OutPosition        = MulMatrix( viewProjectionMatrix, VertexFactory_GetLocalPosition( In ) );
 }