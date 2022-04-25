#include "Render/Shaders/ScreenShader.h"

IMPLEMENT_SHADER_TYPE( FScreenVertexShader, TEXT( "ScreenVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( FScreenPixelShader, TEXT( "ScreenPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

void FScreenPixelShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
	FShader::Init( InShaderCacheItem );
	textureParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "texture0" ) );
	samplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "texture0Sampler") );
}