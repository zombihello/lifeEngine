#include "Render/Shaders/BloomShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE(, CBloomPixelShader, TEXT( "BloomPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

/*
==================
CBloomPixelShader::Init
==================
*/
void CBloomPixelShader::Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );
	sceneColorParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "sceneColor" ) );
	sceneColorSamplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "sceneColorSampler" ) );
	brightThresholdParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "brightThreshold" ) );
}

#if WITH_EDITOR
/*
==================
CBloomPixelShader::ShouldCache
==================
*/
bool CBloomPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	// Shader supported only simple element vertex factory
	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR