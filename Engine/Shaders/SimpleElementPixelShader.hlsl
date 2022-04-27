/**
 * SimpleElementPixelShader.hlsl: Pixel shader code for render simple elements.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

 float4 MainPS( float2 InUV : TEXCOORD0, float4 InColor : TEXCOORD1 ) : SV_Target
 {
     return InColor;
 }