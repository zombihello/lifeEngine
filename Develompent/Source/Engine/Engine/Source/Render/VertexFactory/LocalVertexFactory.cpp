#include "Misc/Template.h"
#include "Render/VertexFactory/LocalVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FLocalVertexFactory, TEXT( "LocalVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< FLocalVertexDeclaration >			GLocalVertexDeclaration;

void FLocalVertexDeclaration::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( FLocalVertexFactory::SSS_Main,		sizeof( FLocalVertexType ),		STRUCT_OFFSET( FLocalVertexType, position ),	VET_Float4, VEU_Position,			0 ),
		FVertexElement( FLocalVertexFactory::SSS_Main,		sizeof( FLocalVertexType ),		STRUCT_OFFSET( FLocalVertexType, texCoord ),	VET_Float2, VEU_TextureCoordinate,	0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void FLocalVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}

uint64 FLocalVertexFactory::GetTypeHash() const
{
	return staticType.GetHash();
}

void FLocalVertexFactory::InitRHI()
{
	InitDeclaration( GLocalVertexDeclaration.GetVertexDeclarationRHI() );
}

FVertexFactoryShaderParameters* FLocalVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new FGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
