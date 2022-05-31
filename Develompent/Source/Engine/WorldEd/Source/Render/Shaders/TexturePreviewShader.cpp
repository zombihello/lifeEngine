#include "Render/Shaders/TexturePreviewShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE( , CTexturePreviewPixelShader, TEXT( "Editor/TexturePreviewPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
bool CTexturePreviewPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return GIsEditor;		// This is shader only for editor
	}

	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR

void CTexturePreviewPixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CScreenPixelShader::Init( InShaderCacheItem );
	colorChannelMask.Bind( InShaderCacheItem.parameterMap, TEXT( "colorChannelMask" ) );
}