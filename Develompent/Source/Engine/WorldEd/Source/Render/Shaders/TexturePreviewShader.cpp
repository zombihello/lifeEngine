#include "Render/Shaders/TexturePreviewShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE( , FTexturePreviewPixelShader, TEXT( "Editor/TexturePreviewPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
bool FTexturePreviewPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return GIsEditor;		// This is shader only for editor
	}

	return InVFMetaType->GetHash() == FSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR

void FTexturePreviewPixelShader::Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem )
{
	FScreenPixelShader::Init( InShaderCacheItem );
	colorChannelMask.Bind( InShaderCacheItem.parameterMap, TEXT( "colorChannelMask" ) );
}