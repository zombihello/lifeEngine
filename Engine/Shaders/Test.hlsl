#include "CPP_GlobalConstantBuffers.hlsl"

struct VS_OUT
{
	float2 texCoord0	: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
#if VERTEXSHADER
#include "VertexFactory.hlsl"

void VS( in FVertexFactoryInput In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	OutPosition		= VertexFactory_GetWorldPosition( In );
	Out.texCoord0	= VertexFactory_GetTexCoord( In, 0 );
}
#endif // VERTEXSHADER

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
#if PIXELSHADER
Texture2D		diffuse			: register( t0 );
SamplerState	samplerLine		: register( s0 );

float4 PS( VS_OUT In ) : SV_Target
{
    return diffuse.Sample( samplerLine, In.texCoord0 );
}
#endif // PIXELSHADER
