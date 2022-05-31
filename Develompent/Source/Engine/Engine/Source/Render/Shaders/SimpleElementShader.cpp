#include "Render/Shaders/SimpleElementShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE(, CSimpleElementVertexShader, TEXT( "SimpleElementVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, CSimpleElementPixelShader, TEXT( "SimpleElementPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
bool CSimpleElementVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	// Shader supported only simple element vertex factory
	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}

bool CSimpleElementPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	// Shader supported only simple element vertex factory
	return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR