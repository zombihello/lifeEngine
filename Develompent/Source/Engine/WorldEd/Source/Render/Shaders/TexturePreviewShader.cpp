#include "Render/Shaders/TexturePreviewShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE( , CTexturePreviewPixelShader, TEXT( "Editor/TexturePreviewPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
/*
==================
CTexturePreviewPixelShader::ShouldCache
==================
*/
bool CTexturePreviewPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return g_IsEditor;		// This is shader only for editor
	}

	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR

/*
==================
CTexturePreviewPixelShader::Init
==================
*/
void CTexturePreviewPixelShader::Init( const CShaderCache::SShaderCacheItem& InShaderCacheItem )
{
	CScreenPixelShader::Init( InShaderCacheItem );
	colorChannelMask.Bind( InShaderCacheItem.parameterMap, TEXT( "colorChannelMask" ) );
	mipmapToView.Bind( InShaderCacheItem.parameterMap, TEXT( "mipmapToView" ) );
}