#include "Misc/Template.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CDynamicMeshVertexFactory, TEXT( "DynamicMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CDynamicMeshVertexDeclaration >			g_DynamicMeshVertexDeclaration;

/*
==================
CDynamicMeshVertexDeclaration::InitRHI
==================
*/
void CDynamicMeshVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, color ),		VET_Float4, VEU_Color, 0 )
	};
	vertexDeclarationRHI = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
}

/*
==================
CDynamicMeshVertexDeclaration::ReleaseRHI
==================
*/
void CDynamicMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

/*
==================
CDynamicMeshVertexFactory::InitRHI
==================
*/
void CDynamicMeshVertexFactory::InitRHI()
{
	InitDeclaration( g_DynamicMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

/*
==================
CDynamicMeshVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CDynamicMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
