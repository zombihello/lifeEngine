struct VS_IN
{
	float3 position		: POSITION;
	float2 texCoord0	: TEXCOORD0;
};

struct VS_OUT
{
	float2 texCoord0	: TEXCOORD0;
};

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
    return float4( In.texCoord0.x, In.texCoord0.y, 0.0f, 1.0f );    // Yellow, with Alpha = 1
}
