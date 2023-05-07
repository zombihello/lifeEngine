#include "Render/Shaders/ScreenShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE(, CScreenPixelShader, TEXT( "ScreenPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
/*
==================
CScreenPixelShader::ShouldCache
==================
*/
bool CScreenPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR

/*
==================
CScreenPixelShader::Init
==================
*/
void CScreenPixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CShader::Init( InShaderCacheItem );
	textureParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "texture0" ) );
	samplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "texture0Sampler") );
}