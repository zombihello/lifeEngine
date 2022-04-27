#include "Misc/Template.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FSimpleElementVertexFactory, TEXT( "SimpleElementVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< FSimpleElementVertexDeclaration >			GSimpleElementVertexDeclaration;

void FSimpleElementVertexDeclaration::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( FSimpleElementVertexFactory::SSS_Main,		sizeof( FSimpleElementVertexType ),		STRUCT_OFFSET( FSimpleElementVertexType, position ),	VET_Float4, VEU_Position,			0 ),
		FVertexElement( FSimpleElementVertexFactory::SSS_Main,		sizeof( FSimpleElementVertexType ),		STRUCT_OFFSET( FSimpleElementVertexType, texCoord ),	VET_Float2, VEU_TextureCoordinate,	0 ),
		FVertexElement( FSimpleElementVertexFactory::SSS_Main,		sizeof( FSimpleElementVertexType ),		STRUCT_OFFSET( FSimpleElementVertexType, color ),		VET_Color,	VEU_Color,				0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void FSimpleElementVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}

uint64 FSimpleElementVertexFactory::GetTypeHash() const
{
	return staticType.GetHash();
}

void FSimpleElementVertexFactory::InitRHI()
{
	InitDeclaration( GSimpleElementVertexDeclaration.GetVertexDeclarationRHI() );
}

FVertexFactoryShaderParameters* FSimpleElementVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new FGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
