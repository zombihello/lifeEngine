#include "Misc/Template.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CSimpleElementVertexFactory, TEXT( "SimpleElementVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CSimpleElementVertexDeclaration >			GSimpleElementVertexDeclaration;

void CSimpleElementVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		SVertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SSimpleElementVertexType ),		STRUCT_OFFSET( SSimpleElementVertexType, position ),	VET_Float4, VEU_Position,			0 ),
		SVertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SSimpleElementVertexType ),		STRUCT_OFFSET( SSimpleElementVertexType, texCoord ),	VET_Float2, VEU_TextureCoordinate,	0 ),
		SVertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SSimpleElementVertexType ),		STRUCT_OFFSET( SSimpleElementVertexType, color ),		VET_Color,	VEU_Color,				0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void CSimpleElementVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}

uint64 CSimpleElementVertexFactory::GetTypeHash() const
{
	return staticType.GetHash();
}

void CSimpleElementVertexFactory::InitRHI()
{
	InitDeclaration( GSimpleElementVertexDeclaration.GetVertexDeclarationRHI() );
}

CVertexFactoryShaderParameters* CSimpleElementVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
