#include "Misc/Template.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FSpriteVertexFactory, TEXT( "SpriteVertexFactory.hlsl" ) )

void FSpriteVertexShaderParameters::Bind( const class FShaderParameterMap& InParameterMap )
{
	FGeneralVertexShaderParameters::Bind( InParameterMap );
	textureRectParameter.Bind( InParameterMap, TEXT( "textureRect" ) );
	spriteSizeParameter.Bind( InParameterMap, TEXT( "spriteSize" ) );
}

void FSpriteVertexShaderParameters::Set( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FSceneView* InView ) const
{
	FGeneralVertexShaderParameters::Set( InDeviceContextRHI, InVertexFactory, InView );
	FSpriteVertexFactory*		vertexFactory = ( FSpriteVertexFactory* )InVertexFactory;
	check( InVertexFactory );

	SetVertexShaderValue( InDeviceContextRHI, textureRectParameter, vertexFactory->GetTextureRect() );
	SetVertexShaderValue( InDeviceContextRHI, spriteSizeParameter, vertexFactory->GetSpriteSize() );
}

FSpriteVertexFactory::FSpriteVertexFactory()
	: textureRect( 0.f, 0.f, 1.f, 1.f )
	, spriteSize( 1.f, 1.f )
{}

void FSpriteVertexFactory::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( SSS_Main, sizeof( FSpriteVertexType ), STRUCT_OFFSET( FSpriteVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		FVertexElement( SSS_Main, sizeof( FSpriteVertexType ), STRUCT_OFFSET( FSpriteVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		FVertexElement( SSS_Main, sizeof( FSpriteVertexType ), STRUCT_OFFSET( FSpriteVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
	};

	InitDeclaration( vertexDeclElementList );
}

FVertexFactoryShaderParameters* FSpriteVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new FSpriteVertexShaderParameters() : nullptr;
}
