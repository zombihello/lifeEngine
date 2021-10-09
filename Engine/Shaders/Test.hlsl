struct VS_IN
{
	float3 position		: POSITION;
	float2 texCoord0	: TEXCOORD0;
};

struct VS_OUT
{
	float2 texCoord0	: TEXCOORD0;
};

cbuffer PSConstatBuffer : register( b0 )
{
	float4		color;
}

Texture2D		diffuse			: register( t0 );
SamplerState	samplerLine		: register( s0 );

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void VS( in VS_IN In, out VS_OUT Out, out float4 OutPosition : SV_POSITION )
{
	OutPosition		= float4( In.position, 1.f );
	Out.texCoord0	= In.texCoord0;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUT In ) : SV_Target
{
    return diffuse.Sample( samplerLine, In.texCoord0 ) * color;
}
