#include "Render/Shaders/SimpleElementShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

IMPLEMENT_SHADER_TYPE(, FSimpleElementVertexShader, TEXT( "SimpleElementVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, FSimpleElementPixelShader, TEXT( "SimpleElementPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );

#if WITH_EDITOR
bool FSimpleElementVertexShader::ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	// Shader supported only simple element vertex factory
	return InVFMetaType->GetHash() == FSimpleElementVertexFactory::staticType.GetHash();
}

bool FSimpleElementPixelShader::ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType /* = nullptr */ )
{
	if ( !InVFMetaType )
	{
		return true;
	}

	// Shader supported only simple element vertex factory
	return InVFMetaType->GetHash() == FSimpleElementVertexFactory::staticType.GetHash();
}
#endif // WITH_EDITOR