#include "Misc/Template.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CSimpleElementVertexFactory, TEXT( "SimpleElementVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CSimpleElementVertexDeclaration >			g_SimpleElementVertexDeclaration;

/*
==================
CSimpleElementVertexDeclaration::InitRHI
==================
*/
void CSimpleElementVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		VertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SimpleElementVertexType ),		STRUCT_OFFSET( SimpleElementVertexType, position ),	VET_Float4, VEU_Position,			0 ),
		VertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SimpleElementVertexType ),		STRUCT_OFFSET( SimpleElementVertexType, texCoord ),	VET_Float2, VEU_TextureCoordinate,	0 ),
		VertexElement( CSimpleElementVertexFactory::SSS_Main,		sizeof( SimpleElementVertexType ),		STRUCT_OFFSET( SimpleElementVertexType, color ),		VET_Color,	VEU_Color,				0 )
	};
	vertexDeclarationRHI = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
}

/*
==================
CSimpleElementVertexDeclaration::ReleaseRHI
==================
*/
void CSimpleElementVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}

/*
==================
CSimpleElementVertexFactory::InitRHI
==================
*/
void CSimpleElementVertexFactory::InitRHI()
{
	InitDeclaration( g_SimpleElementVertexDeclaration.GetVertexDeclarationRHI() );
}

/*
==================
CSimpleElementVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CSimpleElementVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
